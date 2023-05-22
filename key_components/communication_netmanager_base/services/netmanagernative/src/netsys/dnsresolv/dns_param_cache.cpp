/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "dns_param_cache.h"

#include <algorithm>

#include "netmanager_base_common_utils.h"

namespace OHOS::nmd {
using namespace OHOS::NetManagerStandard::CommonUtils;
namespace {
void GetVectorData(const std::vector<std::string> &data, std::string &result)
{
    result.append("{ ");
    std::for_each(data.begin(), data.end(), [&result](const auto &str) { result.append(ToAnonymousIp(str) + ", "); });
    result.append("}\n");
}
constexpr int RES_TIMEOUT = 5000;    // min. milliseconds between retries
constexpr int RES_DEFAULT_RETRY = 2; // Default
} // namespace

DnsParamCache::DnsParamCache() : defaultNetId_(0) {}

std::vector<std::string> DnsParamCache::SelectNameservers(const std::vector<std::string> &servers)
{
    std::vector<std::string> res = servers;
    if (res.size() > MAX_SERVER_NUM) {
        res.resize(MAX_SERVER_NUM);
    }
    return res;
}

int32_t DnsParamCache::CreateCacheForNet(uint16_t netId)
{
    NETNATIVE_LOG_D("DnsParamCache::CreateCacheForNet, netid:%{public}d,", netId);
    std::lock_guard<std::mutex> guard(cacheMutex_);
    if (serverConfigMap_.find(netId) != serverConfigMap_.end()) {
        NETNATIVE_LOGE("DnsParamCache::CreateCacheForNet, netid already exist, no need to create");
        return -EEXIST;
    }
    serverConfigMap_[netId].SetNetId(netId);
    return 0;
}

int32_t DnsParamCache::DestroyNetworkCache(uint16_t netId)
{
    NETNATIVE_LOG_D("DnsParamCache::CreateCacheForNet, netid:%{public}d,", netId);
    std::lock_guard<std::mutex> guard(cacheMutex_);
    if (serverConfigMap_.erase(netId)) {
        return 0;
    } else {
        return -EEXIST;
    }
}

int32_t DnsParamCache::SetResolverConfig(uint16_t netId, uint16_t baseTimeoutMsec, uint8_t retryCount,
                                         const std::vector<std::string> &servers,
                                         const std::vector<std::string> &domains)
{
    std::vector<std::string> nameservers = SelectNameservers(servers);
    NETNATIVE_LOG_D("DnsParamCache::SetResolverConfig, netid:%{public}d, numServers:%{public}d,", netId,
                    static_cast<int>(nameservers.size()));

    std::lock_guard<std::mutex> guard(cacheMutex_);

    // select_domains
    if (serverConfigMap_.find(netId) == serverConfigMap_.end()) {
        NETNATIVE_LOGE("DnsParamCache::SetResolverConfig failed, netid is non-existent");
        return -ENOENT;
    }

    auto oldDnsServers = serverConfigMap_[netId].GetServers();
    std::sort(oldDnsServers.begin(), oldDnsServers.end());

    auto newDnsServers = servers;
    std::sort(newDnsServers.begin(), newDnsServers.end());

    if (oldDnsServers != newDnsServers) {
        serverConfigMap_[netId].GetCache().Clear();
    }

    serverConfigMap_[netId].SetNetId(netId);
    serverConfigMap_[netId].SetServers(servers);
    serverConfigMap_[netId].SetDomains(domains);
    if (retryCount == 0) {
        serverConfigMap_[netId].SetRetryCount(RES_DEFAULT_RETRY);
    } else {
        serverConfigMap_[netId].SetRetryCount(retryCount);
    }
    if (baseTimeoutMsec == 0) {
        serverConfigMap_[netId].SetTimeoutMsec(RES_TIMEOUT);
    } else {
        serverConfigMap_[netId].SetTimeoutMsec(baseTimeoutMsec);
    }
    return 0;
}

void DnsParamCache::SetDefaultNetwork(uint16_t netId)
{
    defaultNetId_ = netId;
}

int32_t DnsParamCache::GetResolverConfig(uint16_t netId, std::vector<std::string> &servers,
                                         std::vector<std::string> &domains, uint16_t &baseTimeoutMsec,
                                         uint8_t &retryCount)
{
    if (netId == 0) {
        netId = defaultNetId_;
    }

    std::lock_guard<std::mutex> guard(cacheMutex_);
    if (serverConfigMap_.find(netId) == serverConfigMap_.end()) {
        DNS_CONFIG_PRINT("get Config failed: netid is not have netid:%{public}d,", netId);
        return -ENOENT;
    }

    servers = serverConfigMap_[netId].GetServers();
    domains = serverConfigMap_[netId].GetDomains();
    baseTimeoutMsec = serverConfigMap_[netId].GetTimeoutMsec();
    retryCount = serverConfigMap_[netId].GetRetryCount();

    return 0;
}

int32_t DnsParamCache::GetDefaultNetwork() const
{
    return defaultNetId_;
}

void DnsParamCache::SetDnsCache(uint16_t netId, const std::string &hostName, const AddrInfo &addrInfo)
{
    if (netId == 0) {
        netId = defaultNetId_;
    }
    std::lock_guard<std::mutex> guard(cacheMutex_);
    if (serverConfigMap_.find(netId) == serverConfigMap_.end()) {
        DNS_CONFIG_PRINT("SetDnsCache failed: netid is not have netid:%{public}d,", netId);
        return;
    }

    serverConfigMap_[netId].GetCache().Put(hostName, addrInfo);
}

std::vector<AddrInfo> DnsParamCache::GetDnsCache(uint16_t netId, const std::string &hostName)
{
    if (netId == 0) {
        netId = defaultNetId_;
    }

    std::lock_guard<std::mutex> guard(cacheMutex_);
    if (serverConfigMap_.find(netId) == serverConfigMap_.end()) {
        DNS_CONFIG_PRINT("GetDnsCache failed: netid is not have netid:%{public}d,", netId);
        return {};
    }

    return serverConfigMap_[netId].GetCache().Get(hostName);
}

void DnsParamCache::SetCacheDelayed(uint16_t netId, const std::string &hostName)
{
    if (netId == 0) {
        netId = defaultNetId_;
    }

    std::lock_guard<std::mutex> guard(cacheMutex_);
    if (serverConfigMap_.find(netId) == serverConfigMap_.end()) {
        DNS_CONFIG_PRINT("SetCacheDelayed failed: netid is not have netid:%{public}d,", netId);
        return;
    }

    serverConfigMap_[netId].SetCacheDelayed(hostName);
}

void DnsParamCache::GetDumpInfo(std::string &info)
{
    std::string dnsData;
    static const std::string TAB = "  ";
    std::for_each(serverConfigMap_.begin(), serverConfigMap_.end(), [&dnsData](const auto &serverConfig) {
        dnsData.append(TAB + "NetId: " + std::to_string(serverConfig.second.GetNetId()) + "\n");
        dnsData.append(TAB + "TimeoutMsec: " + std::to_string(serverConfig.second.GetTimeoutMsec()) + "\n");
        dnsData.append(TAB + "RetryCount: " + std::to_string(serverConfig.second.GetRetryCount()) + "\n");
        dnsData.append(TAB + "Servers:");
        GetVectorData(serverConfig.second.GetServers(), dnsData);
        dnsData.append(TAB + "Domains:");
        GetVectorData(serverConfig.second.GetDomains(), dnsData);
    });
    info.append(dnsData);
}
} // namespace OHOS::nmd
