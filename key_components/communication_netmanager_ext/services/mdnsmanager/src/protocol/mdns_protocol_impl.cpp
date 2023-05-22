/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mdns_protocol_impl.h"

#include <arpa/inet.h>
#include <iostream>
#include <numeric>
#include <random>
#include <unistd.h>

#include "mdns_packet_parser.h"
#include "netmgr_ext_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {

namespace {

constexpr uint32_t DEFAULT_TTL = 120;
constexpr uint16_t MDNS_FLUSH_CACHE_BIT = 0x8000;

constexpr int PHASE_PTR = 1;
constexpr int PHASE_SRV = 2;
constexpr int PHASE_DOMAIN = 3;

std::string AddrToString(const std::any &addr)
{
    char buf[INET6_ADDRSTRLEN] = {0};
    if (std::any_cast<in_addr>(&addr)) {
        if (inet_ntop(AF_INET, std::any_cast<in_addr>(&addr), buf, sizeof(buf)) == nullptr) {
            return std::string{};
        }
    } else if (std::any_cast<in6_addr>(&addr)) {
        if (inet_ntop(AF_INET6, std::any_cast<in6_addr>(&addr), buf, sizeof(buf)) == nullptr) {
            return std::string{};
        }
    } else {
        return std::string{};
    }
    return std::string(buf);
}

} // namespace

MDnsProtocolImpl::MDnsProtocolImpl()
{
    Init();
}

void MDnsProtocolImpl::Init()
{
    listener_.CloseAllSocket();
    if (config_.configAllIface) {
        listener_.OpenSocketForEachIface(config_.ipv6Support, config_.configLo);
    } else {
        listener_.OpenSocketForDefault(config_.ipv6Support);
    }
    listener_.SetReceiveHandler(
        [this](int sock, const MDnsPayload &payload) { return this->ReceivePacket(sock, payload); });
    listener_.SetRefreshHandler([this](int sock) { return this->OnRefresh(sock); });
}

void MDnsProtocolImpl::SetConfig(const MDnsConfig &config)
{
    config_ = config;
}

const MDnsConfig &MDnsProtocolImpl::GetConfig() const
{
    return config_;
}

void MDnsProtocolImpl::SetHandler(const Handler &handler)
{
    handler_ = handler;
}

std::string MDnsProtocolImpl::Decorated(const std::string &name) const
{
    return name + config_.topDomain;
}

std::string MDnsProtocolImpl::ExtractInstance(const Result &info) const
{
    return Decorated(info.serviceName + MDNS_DOMAIN_SPLITER_STR + info.serviceType);
}

int32_t MDnsProtocolImpl::Register(const Result &info)
{
    if (!(IsNameValid(info.serviceName) && IsTypeValid(info.serviceType) && IsPortValid(info.port))) {
        return NET_MDNS_ERR_ILLEGAL_ARGUMENT;
    }
    std::string name = ExtractInstance(info);
    if (!IsDomainValid(name)) {
        return NET_MDNS_ERR_ILLEGAL_ARGUMENT;
    }
    {
        std::lock_guard<std::recursive_mutex> guard(mutex_);
        if (srvMap_.find(name) != srvMap_.end()) {
            return NET_MDNS_ERR_SERVICE_INSTANCE_DUPLICATE;
        }
        srvMap_.emplace(name, info);
    }

    listener_.Start();
    return Announce(info, false);
}

int32_t MDnsProtocolImpl::Discovery(const std::string &serviceType)
{
    if (!IsTypeValid(serviceType)) {
        return NET_MDNS_ERR_ILLEGAL_ARGUMENT;
    }
    std::string name = Decorated(serviceType);
    if (!IsDomainValid(name)) {
        return NET_MDNS_ERR_ILLEGAL_ARGUMENT;
    }
    {
        std::lock_guard<std::recursive_mutex> guard(mutex_);
        ++reqMap_[name];
    }
    MDnsPayloadParser parser;
    MDnsMessage msg{};
    msg.questions.emplace_back(DNSProto::Question{
        .name = name,
        .qtype = DNSProto::RRTYPE_PTR,
        .qclass = DNSProto::RRCLASS_IN,
    });
    msg.header.qdcount = msg.questions.size();

    listener_.Start();
    ssize_t size = listener_.MulticastAll(parser.ToBytes(msg));

    return size > 0 ? NETMANAGER_EXT_SUCCESS : NET_MDNS_ERR_SEND;
}

void MDnsProtocolImpl::HandleResolveInstanceLater(const Result &result)
{
    cacheMap_[result.domain];
    ResolveFromNet(result.domain);
    RunTaskLater(
        [r = result, this]() mutable {
            if (cacheMap_.find(r.domain) == cacheMap_.end() || cacheMap_[r.domain].addr.empty()) {
                NETMGR_EXT_LOG_D("instance %{public}s, domain: %{public}s not found", r.serviceName.c_str(),
                                 r.domain.c_str());
                return false;
            }
            r.ipv6 = cacheMap_[r.domain].ipv6;
            r.addr = cacheMap_[r.domain].addr;
            handler_(r, NETMANAGER_EXT_SUCCESS);
            return true;
        },
        false);
}

bool MDnsProtocolImpl::ResolveInstanceFromCache(const std::string &name)
{
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    if (cacheMap_.find(name) != cacheMap_.end() && !cacheMap_[name].domain.empty()) {
        Result r = cacheMap_[name];
        if (cacheMap_.find(r.domain) != cacheMap_.end() && !cacheMap_[r.domain].addr.empty()) {
            r.ipv6 = cacheMap_[r.domain].ipv6;
            r.addr = cacheMap_[r.domain].addr;
            RunTaskLater([r, this]() {
                handler_(r, NETMANAGER_EXT_SUCCESS);
                return true;
            });
        } else {
            HandleResolveInstanceLater(r);
        }
        return true;
    }
    return false;
}

bool MDnsProtocolImpl::ResolveInstanceFromNet(const std::string &name)
{
    MDnsPayloadParser parser;
    MDnsMessage msg{};
    msg.questions.emplace_back(DNSProto::Question{
        .name = name,
        .qtype = DNSProto::RRTYPE_SRV,
        .qclass = DNSProto::RRCLASS_IN,
    });
    msg.questions.emplace_back(DNSProto::Question{
        .name = name,
        .qtype = DNSProto::RRTYPE_TXT,
        .qclass = DNSProto::RRCLASS_IN,
    });
    msg.header.qdcount = msg.questions.size();

    listener_.Start();
    ssize_t size = listener_.MulticastAll(parser.ToBytes(msg));

    return size > 0;
}

bool MDnsProtocolImpl::ResolveFromCache(const std::string &domain)
{
    if (cacheMap_.find(domain) != cacheMap_.end() && !cacheMap_[domain].addr.empty()) {
        RunTaskLater([r = cacheMap_[domain], this]() {
            handler_(r, NETMANAGER_EXT_SUCCESS);
            return true;
        });
        return true;
    }
    return false;
}

bool MDnsProtocolImpl::ResolveFromNet(const std::string &domain)
{
    MDnsPayloadParser parser;
    MDnsMessage msg{};
    msg.questions.emplace_back(DNSProto::Question{
        .name = domain,
        .qtype = DNSProto::RRTYPE_A,
        .qclass = DNSProto::RRCLASS_IN,
    });
    msg.questions.emplace_back(DNSProto::Question{
        .name = domain,
        .qtype = DNSProto::RRTYPE_AAAA,
        .qclass = DNSProto::RRCLASS_IN,
    });
    msg.header.qdcount = msg.questions.size();

    listener_.Start();
    ssize_t size = listener_.MulticastAll(parser.ToBytes(msg));

    return size > 0;
}

int32_t MDnsProtocolImpl::ResolveInstance(const std::string &instance)
{
    if (!IsInstanceValid(instance)) {
        return NET_MDNS_ERR_ILLEGAL_ARGUMENT;
    }
    std::string name = Decorated(instance);
    if (!IsDomainValid(name)) {
        return NET_MDNS_ERR_ILLEGAL_ARGUMENT;
    }
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    if (ResolveInstanceFromCache(name)) {
        return NETMANAGER_EXT_SUCCESS;
    }
    ++reqMap_[name];
    return ResolveInstanceFromNet(name) ? NETMANAGER_EXT_SUCCESS : NET_MDNS_ERR_SEND;
}

int32_t MDnsProtocolImpl::Resolve(const std::string &domain)
{
    if (!IsDomainValid(domain)) {
        return NET_MDNS_ERR_ILLEGAL_ARGUMENT;
    }
    std::string name = domain;
    if (!IsDomainValid(name)) {
        return NET_MDNS_ERR_ILLEGAL_ARGUMENT;
    }
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    if (ResolveFromCache(name)) {
        return NETMANAGER_EXT_SUCCESS;
    }
    ++reqMap_[name];
    return ResolveFromNet(name) ? NETMANAGER_EXT_SUCCESS : NET_MDNS_ERR_SEND;
}

int32_t MDnsProtocolImpl::UnRegister(const std::string &key)
{
    std::string name = Decorated(key);
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    if (srvMap_.find(name) != srvMap_.end()) {
        Announce(srvMap_[name], true);
        srvMap_.erase(name);
        return NETMANAGER_EXT_SUCCESS;
    }
    return NET_MDNS_ERR_SERVICE_INSTANCE_NOT_FOUND;
}

int32_t MDnsProtocolImpl::StopDiscovery(const std::string &key)
{
    return Stop(Decorated(key));
}

int32_t MDnsProtocolImpl::StopResolveInstance(const std::string &key)
{
    return Stop(Decorated(key));
}

int32_t MDnsProtocolImpl::StopResolve(const std::string &key)
{
    return Stop(key);
}

int32_t MDnsProtocolImpl::Stop(const std::string &key)
{
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    if (reqMap_.find(key) != reqMap_.end() && reqMap_[key] > 0) {
        --reqMap_[key];
    }
    return NETMANAGER_EXT_SUCCESS;
}

int32_t MDnsProtocolImpl::Announce(const Result &info, bool off)
{
    MDnsMessage response{};
    response.header.flags = DNSProto::MDNS_ANSWER_FLAGS;
    std::string name = Decorated(info.serviceName + MDNS_DOMAIN_SPLITER_STR + info.serviceType);
    response.answers.emplace_back(DNSProto::ResourceRecord{.name = Decorated(info.serviceType),
                                                           .rtype = static_cast<uint16_t>(DNSProto::RRTYPE_PTR),
                                                           .rclass = DNSProto::RRCLASS_IN | MDNS_FLUSH_CACHE_BIT,
                                                           .ttl = off ? 0U : DEFAULT_TTL,
                                                           .rdata = name});
    response.answers.emplace_back(DNSProto::ResourceRecord{.name = name,
                                                           .rtype = static_cast<uint16_t>(DNSProto::RRTYPE_SRV),
                                                           .rclass = DNSProto::RRCLASS_IN | MDNS_FLUSH_CACHE_BIT,
                                                           .ttl = off ? 0U : DEFAULT_TTL,
                                                           .rdata = DNSProto::RDataSrv{
                                                               .priority = 0,
                                                               .weight = 0,
                                                               .port = static_cast<uint16_t>(info.port),
                                                               .name = GetHostDomain(),
                                                           }});
    response.answers.emplace_back(DNSProto::ResourceRecord{.name = name,
                                                           .rtype = static_cast<uint16_t>(DNSProto::RRTYPE_TXT),
                                                           .rclass = DNSProto::RRCLASS_IN | MDNS_FLUSH_CACHE_BIT,
                                                           .ttl = off ? 0U : DEFAULT_TTL,
                                                           .rdata = info.txt});
    MDnsPayloadParser parser;
    ssize_t size = listener_.MulticastAll(parser.ToBytes(response));
    return size > 0 ? NETMANAGER_EXT_SUCCESS : NET_MDNS_ERR_SEND;
}

void MDnsProtocolImpl::ReceivePacket(int sock, const MDnsPayload &payload)
{
    if (payload.size() == 0) {
        NETMGR_EXT_LOG_W("empty payload received");
        return;
    }
    MDnsPayloadParser parser;
    MDnsMessage msg = parser.FromBytes(payload);
    if (parser.GetError() != 0) {
        NETMGR_EXT_LOG_W("payload parse failed");
        return;
    }
    if ((msg.header.flags & DNSProto::HEADER_FLAGS_QR_MASK) == 0) {
        ProcessQuestion(sock, msg);
    } else {
        ProcessAnswer(sock, msg);
    }
}

void MDnsProtocolImpl::OnRefresh(int sock)
{
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    std::queue<Task> tmp;
    while (taskQueue_.size() > 0) {
        auto func = taskQueue_.front();
        taskQueue_.pop();
        if (!func()) {
            tmp.push(func);
        }
    }
    tmp.swap(taskQueue_);
}

void MDnsProtocolImpl::AppendRecord(std::vector<DNSProto::ResourceRecord> &rrlist, DNSProto::RRType type,
                                    const std::string &name, const std::any &rdata)
{
    rrlist.emplace_back(DNSProto::ResourceRecord{.name = name,
                                                 .rtype = static_cast<uint16_t>(type),
                                                 .rclass = DNSProto::RRCLASS_IN | MDNS_FLUSH_CACHE_BIT,
                                                 .ttl = DEFAULT_TTL,
                                                 .rdata = rdata});
}

void MDnsProtocolImpl::ProcessQuestion(int sock, const MDnsMessage &msg)
{
    const sockaddr *saddrIf = listener_.GetSockAddr(sock);
    if (saddrIf == nullptr) {
        return;
    }
    std::any anyAddr;
    DNSProto::RRType anyAddrType;
    if (saddrIf->sa_family == AF_INET6) {
        anyAddr = reinterpret_cast<const sockaddr_in6 *>(saddrIf)->sin6_addr;
        anyAddrType = DNSProto::RRTYPE_AAAA;
    } else {
        anyAddr = reinterpret_cast<const sockaddr_in *>(saddrIf)->sin_addr;
        anyAddrType = DNSProto::RRTYPE_A;
    }
    int phase = 0;
    MDnsMessage response{};
    response.header.flags = DNSProto::MDNS_ANSWER_FLAGS;
    for (size_t i = 0; i < msg.header.qdcount; ++i) {
        ProcessQuestionRecord(anyAddr, anyAddrType, msg.questions[i], phase, response);
    }
    if (phase < PHASE_DOMAIN) {
        AppendRecord(response.additional, anyAddrType, GetHostDomain(), anyAddr);
    }
    if (phase != 0 && response.answers.size() > 0) {
        listener_.Multicast(sock, MDnsPayloadParser().ToBytes(response));
    }
}

void MDnsProtocolImpl::ProcessQuestionRecord(const std::any &anyAddr, const DNSProto::RRType &anyAddrType,
                                             const DNSProto::Question &qu, int &phase, MDnsMessage &response)
{
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    std::string name = qu.name;
    if (qu.qtype == DNSProto::RRTYPE_ANY || qu.qtype == DNSProto::RRTYPE_PTR) {
        std::for_each(srvMap_.begin(), srvMap_.end(), [&](const auto &elem) -> void {
            if (EndsWith(elem.first, name)) {
                AppendRecord(response.answers, DNSProto::RRTYPE_PTR, name, elem.first);
                AppendRecord(response.additional, DNSProto::RRTYPE_SRV, elem.first,
                             DNSProto::RDataSrv{
                                 .priority = 0,
                                 .weight = 0,
                                 .port = static_cast<uint16_t>(elem.second.port),
                                 .name = GetHostDomain(),
                             });
                AppendRecord(response.additional, DNSProto::RRTYPE_TXT, elem.first, elem.second.txt);
            }
        });
        phase = std::max(phase, PHASE_PTR);
    }
    if (qu.qtype == DNSProto::RRTYPE_ANY || qu.qtype == DNSProto::RRTYPE_SRV) {
        auto iter = srvMap_.find(name);
        if (iter == srvMap_.end()) {
            return;
        }
        AppendRecord(response.answers, DNSProto::RRTYPE_SRV, name,
                     DNSProto::RDataSrv{
                         .priority = 0,
                         .weight = 0,
                         .port = static_cast<uint16_t>(iter->second.port),
                         .name = GetHostDomain(),
                     });
        phase = std::max(phase, PHASE_SRV);
    }
    if (qu.qtype == DNSProto::RRTYPE_ANY || qu.qtype == DNSProto::RRTYPE_TXT) {
        auto iter = srvMap_.find(name);
        if (iter == srvMap_.end()) {
            return;
        }
        AppendRecord(response.answers, DNSProto::RRTYPE_TXT, name, iter->second.txt);
        phase = std::max(phase, PHASE_SRV);
    }
    if (qu.qtype == DNSProto::RRTYPE_ANY || qu.qtype == DNSProto::RRTYPE_A || qu.qtype == DNSProto::RRTYPE_AAAA) {
        if (name != GetHostDomain() || (qu.qtype != DNSProto::RRTYPE_ANY && anyAddrType != qu.qtype)) {
            return;
        }
        AppendRecord(response.answers, anyAddrType, name, anyAddr);
        phase = std::max(phase, PHASE_DOMAIN);
    }
}

void MDnsProtocolImpl::ProcessAnswer(int sock, const MDnsMessage &msg)
{
    const sockaddr *saddrIf = listener_.GetSockAddr(sock);
    if (saddrIf == nullptr) {
        return;
    }
    bool v6 = (saddrIf->sa_family == AF_INET6);

    std::vector<Result> matches;
    std::set<std::string> changed;
    for (size_t i = 0; i < msg.answers.size(); ++i) {
        ProcessAnswerRecord(v6, msg.answers[i], &matches, changed);
    }

    for (size_t i = 0; i < msg.additional.size(); ++i) {
        ProcessAnswerRecord(v6, msg.additional[i], nullptr, changed);
    }

    for (size_t i = 0; i < msg.additional.size(); ++i) {
        ProcessAnswerRecord(v6, msg.additional[i], nullptr, changed);
    }

    for (auto i = matches.begin(); i != matches.end() && handler_ != nullptr; ++i) {
        handler_(*i, NETMANAGER_EXT_SUCCESS);
    }

    for (auto i = changed.begin(); i != changed.end() && handler_ != nullptr; ++i) {
        if (cacheMap_.find(*i) == cacheMap_.end()) {
            continue;
        }
        cacheMap_[*i].iface = listener_.GetIface(sock);
        if (cacheMap_[*i].type == INSTANCE_RESOLVED && ResolveInstanceFromCache(*i)) {
            continue;
        }
        handler_(cacheMap_[*i], NETMANAGER_EXT_SUCCESS);
    }
}

void MDnsProtocolImpl::ProcessPtrRecord(bool v6, const DNSProto::ResourceRecord &rr, std::vector<Result> *matches)
{
    const std::string *data = std::any_cast<std::string>(&rr.rdata);
    if (data == nullptr) {
        return;
    }
    Result result;
    ExtractNameAndType(*data, result.serviceName, result.serviceType);
    if (std::find_if(matches->begin(), matches->end(), [&](const auto &elem) {
            return elem.serviceName == result.serviceName && elem.serviceType == result.serviceType;
        }) == matches->end()) {
        result.type = (rr.ttl == 0) ? SERVICE_LOST : SERVICE_FOUND;
        matches->emplace_back(std::move(result));
        if (rr.ttl == 0) {
            cacheMap_.erase(*data);
        } else {
            cacheMap_[*data];
        }
    }
}

void MDnsProtocolImpl::ProcessAnswerRecord(bool v6, const DNSProto::ResourceRecord &rr, std::vector<Result> *matches,
                                           std::set<std::string> &changed)
{
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    std::string name = rr.name;
    bool isReq = reqMap_.find(name) != reqMap_.end() && reqMap_[name] > 0;
    if (!isReq && cacheMap_.find(name) == cacheMap_.end()) {
        return;
    }
    if (rr.rtype == DNSProto::RRTYPE_PTR && matches) {
        ProcessPtrRecord(v6, rr, matches);
    } else if (rr.rtype == DNSProto::RRTYPE_SRV) {
        const DNSProto::RDataSrv *srv = std::any_cast<DNSProto::RDataSrv>(&rr.rdata);
        if (rr.ttl == 0) {
            return cacheMap_[name].domain.clear();
        }
        if (srv == nullptr) {
            return;
        }
        Result &result = cacheMap_[name];
        result.type = INSTANCE_RESOLVED;
        ExtractNameAndType(name, result.serviceName, result.serviceType);
        result.domain = srv->name;
        result.port = srv->port;
        cacheMap_[srv->name];
        if (isReq) {
            changed.emplace(name);
        }
    } else if (rr.rtype == DNSProto::RRTYPE_TXT) {
        const TxtRecordEncoded *txt = std::any_cast<TxtRecordEncoded>(&rr.rdata);
        if (rr.ttl == 0 || txt == nullptr) {
            return;
        }
        Result &result = cacheMap_[name];
        result.txt = *txt;
    } else if (rr.rtype == DNSProto::RRTYPE_A || rr.rtype == DNSProto::RRTYPE_AAAA) {
        if (v6 != (rr.rtype == DNSProto::RRTYPE_AAAA)) {
            return;
        }
        if (rr.ttl == 0) {
            return cacheMap_[name].domain.clear();
        }
        Result &result = cacheMap_[name];
        result.type = DOMAIN_RESOLVED;
        result.domain = name;
        result.ipv6 = (rr.rtype == DNSProto::RRTYPE_AAAA);
        result.addr = AddrToString(rr.rdata);
        if (isReq) {
            changed.emplace(name);
        }
    } else {
        NETMGR_EXT_LOG_D("Unknown packet received, type=[%{public}d]", rr.rtype);
    }
}

std::string MDnsProtocolImpl::GetHostDomain()
{
    if (config_.hostname.empty()) {
        char buffer[MDNS_MAX_DOMAIN_LABEL];
        if (gethostname(buffer, sizeof(buffer)) == 0) {
            config_.hostname = buffer;
            static auto uid = []() {
                std::random_device rd;
                return rd();
            }();
            config_.hostname += std::to_string(uid);
        }
    }
    return Decorated(config_.hostname);
}

void MDnsProtocolImpl::RunTaskLater(const Task &task, bool atonce)
{
    {
        std::lock_guard<std::recursive_mutex> guard(mutex_);
        taskQueue_.push(task);
    }
    if (atonce) {
        listener_.TriggerRefresh();
    }
}

} // namespace NetManagerStandard
} // namespace OHOS
