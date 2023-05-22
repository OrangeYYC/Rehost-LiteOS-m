/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "static_configuration.h"

#include "inet_addr.h"
#include "netmgr_ext_log_wrapper.h"
#include "parcel.h"
#include "refbase.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr uint32_t MAX_SIZE = 50;
}

bool StaticConfiguration::Marshalling(Parcel &parcel) const
{
    if (!ipAddr_.Marshalling(parcel)) {
        NETMGR_EXT_LOG_E("write ipAddr_ to parcel failed");
        return false;
    }
    if (!route_.Marshalling(parcel)) {
        NETMGR_EXT_LOG_E("write route_ to parcel failed");
        return false;
    }
    if (!gateway_.Marshalling(parcel)) {
        NETMGR_EXT_LOG_E("write gateway_ to parcel failed");
        return false;
    }
    if (!netMask_.Marshalling(parcel)) {
        NETMGR_EXT_LOG_E("write netMask_ to parcel failed");
        return false;
    }
    if (!parcel.WriteUint32(dnsServers_.size())) {
        NETMGR_EXT_LOG_E("write dnsServers_ size to parcel failed");
        return false;
    }
    for (auto dnsServer : dnsServers_) {
        if (!dnsServer.Marshalling(parcel)) {
            NETMGR_EXT_LOG_E("write dnsServers_ to parcel failed");
            return false;
        }
    }
    if (!parcel.WriteString(domain_)) {
        NETMGR_EXT_LOG_E("write domain_ to parcel failed");
        return false;
    }
    return true;
}

sptr<StaticConfiguration> StaticConfiguration::Unmarshalling(Parcel &parcel)
{
    sptr<StaticConfiguration> ptr = new (std::nothrow) StaticConfiguration();
    if (ptr == nullptr) {
        NETMGR_EXT_LOG_E("ptr new failed");
        return nullptr;
    }
    sptr<INetAddr> ipAddr = INetAddr::Unmarshalling(parcel);
    if (ipAddr == nullptr) {
        NETMGR_EXT_LOG_E("ipAddr is null");
        return nullptr;
    }
    ptr->ipAddr_ = *ipAddr;
    sptr<INetAddr> route = INetAddr::Unmarshalling(parcel);
    if (route == nullptr) {
        NETMGR_EXT_LOG_E("route is null");
        return nullptr;
    }
    ptr->route_ = *route;
    sptr<INetAddr> gateway = INetAddr::Unmarshalling(parcel);
    if (gateway == nullptr) {
        NETMGR_EXT_LOG_E("gateway is null");
        return nullptr;
    }
    ptr->gateway_ = *gateway;
    sptr<INetAddr> netMask = INetAddr::Unmarshalling(parcel);
    if (netMask == nullptr) {
        NETMGR_EXT_LOG_E("netMask is null");
        return nullptr;
    }
    ptr->netMask_ = *netMask;
    uint32_t size = 0;
    if (!parcel.ReadUint32(size)) {
        return nullptr;
    }
    if (size > MAX_SIZE) {
        NETMGR_EXT_LOG_E("size=[%{public}d] is too large", size);
        return nullptr;
    }
    for (uint32_t i = 0; i < size; i++) {
        sptr<INetAddr> netAddr = INetAddr::Unmarshalling(parcel);
        if (netAddr == nullptr) {
            NETMGR_EXT_LOG_E("netAddr is null");
            return nullptr;
        }
        ptr->dnsServers_.push_back(*netAddr);
    }
    if (!parcel.ReadString(ptr->domain_)) {
        return nullptr;
    }
    return ptr;
}
} // namespace NetManagerStandard
} // namespace OHOS