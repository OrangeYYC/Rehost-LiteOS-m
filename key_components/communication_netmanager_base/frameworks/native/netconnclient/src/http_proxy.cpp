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

#include "net_mgr_log_wrapper.h"

#include "http_proxy.h"

namespace OHOS {
namespace NetManagerStandard {
static const size_t MAX_EXCLUSION_SIZE = 500;
static const size_t MAX_URL_SIZE = 2048;

HttpProxy::HttpProxy() : port_(0) {}

HttpProxy::HttpProxy(std::string host, uint16_t port, const std::set<std::string> &exclusionList) : port_(0)
{
    if (host.size() <= MAX_URL_SIZE) {
        host_ = std::move(host);
        port_ = port;
        for (const auto &s : exclusionList) {
            if (s.size() <= MAX_URL_SIZE) {
                exclusionList_.insert(s);
            }
            if (exclusionList_.size() >= MAX_EXCLUSION_SIZE) {
                break;
            }
        }
    } else {
        NETMGR_LOG_E("HttpProxy: host length is invalid");
    }
}

std::string HttpProxy::GetHost() const
{
    return host_;
}

uint16_t HttpProxy::GetPort() const
{
    return port_;
}

std::set<std::string> HttpProxy::GetExclusionList() const
{
    return exclusionList_;
}

bool HttpProxy::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(host_)) {
        return false;
    }

    if (!parcel.WriteUint16(port_)) {
        return false;
    }

    if (!parcel.WriteUint32(static_cast<uint32_t>(std::min(MAX_EXCLUSION_SIZE, exclusionList_.size())))) {
        return false;
    }

    uint32_t size = 0;
    for (const auto &s : exclusionList_) {
        if (!parcel.WriteString(s)) {
            return false;
        }
        ++size;
        if (size >= MAX_EXCLUSION_SIZE) {
            return true;
        }
    }

    return true;
}

bool HttpProxy::Unmarshalling(Parcel &parcel, HttpProxy &httpProxy)
{
    std::string host;
    if (!parcel.ReadString(host)) {
        return false;
    }
    if (host.size() > MAX_URL_SIZE) {
        NETMGR_LOG_E("HttpProxy: Unmarshalling: host length is invalid");
        return false;
    }

    uint16_t port = 0;
    if (!parcel.ReadUint16(port)) {
        return false;
    }

    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        return false;
    }

    if (size == 0) {
        httpProxy = {host, port, {}};
        return true;
    }

    if (size > static_cast<uint32_t>(MAX_EXCLUSION_SIZE)) {
        size = MAX_EXCLUSION_SIZE;
    }

    std::set<std::string> exclusionList;
    for (uint32_t i = 0; i < size; ++i) {
        std::string s;
        parcel.ReadString(s);
        if (s.size() <= MAX_URL_SIZE) {
            exclusionList.insert(s);
        }
    }

    httpProxy = {host, port, exclusionList};
    return true;
}

std::string HttpProxy::ToString() const
{
    std::string s;
    std::string tab = "\t";
    s.append(host_);
    s.append(tab);
    s.append(std::to_string(port_));
    s.append(tab);
    for (const auto &e : exclusionList_) {
        s.append(e);
        s.append(",");
    }
    return s;
}
} // namespace NetManagerStandard
} // namespace OHOS
