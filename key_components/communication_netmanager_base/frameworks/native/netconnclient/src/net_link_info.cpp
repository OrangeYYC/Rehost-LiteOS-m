/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "net_link_info.h"

#include "parcel.h"
#include "refbase.h"
#include "route.h"

#include "inet_addr.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
static constexpr uint32_t MAX_ADDR_SIZE = 16;
static constexpr uint32_t MAX_ROUTE_SIZE = 32;

NetLinkInfo::NetLinkInfo(const NetLinkInfo &linkInfo)
{
    ifaceName_ = linkInfo.ifaceName_;
    domain_ = linkInfo.domain_;
    netAddrList_.assign(linkInfo.netAddrList_.begin(), linkInfo.netAddrList_.end());
    dnsList_.assign(linkInfo.dnsList_.begin(), linkInfo.dnsList_.end());
    routeList_.assign(linkInfo.routeList_.begin(), linkInfo.routeList_.end());
    mtu_ = linkInfo.mtu_;
    tcpBufferSizes_ = linkInfo.tcpBufferSizes_;
}

NetLinkInfo &NetLinkInfo::operator=(const NetLinkInfo &linkInfo)
{
    ifaceName_ = linkInfo.ifaceName_;
    domain_ = linkInfo.domain_;
    netAddrList_.assign(linkInfo.netAddrList_.begin(), linkInfo.netAddrList_.end());
    dnsList_.assign(linkInfo.dnsList_.begin(), linkInfo.dnsList_.end());
    routeList_.assign(linkInfo.routeList_.begin(), linkInfo.routeList_.end());
    mtu_ = linkInfo.mtu_;
    tcpBufferSizes_ = linkInfo.tcpBufferSizes_;
    return *this;
}

bool NetLinkInfo::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(ifaceName_)) {
        return false;
    }
    if (!parcel.WriteString(domain_)) {
        return false;
    }
    if (!parcel.WriteUint32(netAddrList_.size())) {
        return false;
    }
    for (auto it = netAddrList_.begin(); it != netAddrList_.end(); it++) {
        if (!it->Marshalling(parcel)) {
            NETMGR_LOG_E("write net address to parcel failed");
            return false;
        }
    }
    if (!parcel.WriteUint32(dnsList_.size())) {
        return false;
    }
    for (auto it = dnsList_.begin(); it != dnsList_.end(); it++) {
        if (!it->Marshalling(parcel)) {
            NETMGR_LOG_E("write dns to parcel failed");
            return false;
        }
    }
    if (!parcel.WriteUint32(routeList_.size())) {
        return false;
    }
    for (auto it = routeList_.begin(); it != routeList_.end(); it++) {
        if (!it->Marshalling(parcel)) {
            NETMGR_LOG_E("write route to parcel failed");
            return false;
        }
    }
    if (!parcel.WriteUint16(mtu_)) {
        return false;
    }
    if (!parcel.WriteString(tcpBufferSizes_)) {
        return false;
    }
    return true;
}

sptr<NetLinkInfo> NetLinkInfo::Unmarshalling(Parcel &parcel)
{
    sptr<NetLinkInfo> ptr = new (std::nothrow) NetLinkInfo();
    if (ptr == nullptr) {
        return nullptr;
    }
    uint32_t size = 0;
    if (!parcel.ReadString(ptr->ifaceName_) || !parcel.ReadString(ptr->domain_) || !parcel.ReadUint32(size)) {
        return nullptr;
    }
    size = size > MAX_ADDR_SIZE ? MAX_ADDR_SIZE : size;
    sptr<INetAddr> netAddr;
    for (uint32_t i = 0; i < size; i++) {
        netAddr = INetAddr::Unmarshalling(parcel);
        if (netAddr == nullptr) {
            NETMGR_LOG_E("INetAddr::Unmarshalling(parcel) is null");
            return nullptr;
        }
        ptr->netAddrList_.push_back(*netAddr);
    }
    if (!parcel.ReadUint32(size)) {
        return nullptr;
    }
    size = size > MAX_ADDR_SIZE ? MAX_ADDR_SIZE : size;
    for (uint32_t i = 0; i < size; i++) {
        netAddr = INetAddr::Unmarshalling(parcel);
        if (netAddr == nullptr) {
            NETMGR_LOG_E("INetAddr::Unmarshalling(parcel) is null");
            return nullptr;
        }
        ptr->dnsList_.push_back(*netAddr);
    }
    if (!parcel.ReadUint32(size)) {
        return nullptr;
    }
    size = size > MAX_ROUTE_SIZE ? MAX_ROUTE_SIZE : size;
    sptr<Route> route;
    for (uint32_t i = 0; i < size; i++) {
        route = Route::Unmarshalling(parcel);
        if (route == nullptr) {
            NETMGR_LOG_E("Route::Unmarshalling(parcel) is null");
            return nullptr;
        }
        ptr->routeList_.push_back(*route);
    }
    if (!parcel.ReadUint16(ptr->mtu_) || !parcel.ReadString(ptr->tcpBufferSizes_)) {
        return nullptr;
    }
    return ptr;
}

bool NetLinkInfo::Marshalling(Parcel &parcel, const sptr<NetLinkInfo> &object)
{
    if (object == nullptr) {
        NETMGR_LOG_E("NetLinkInfo object ptr is nullptr");
        return false;
    }
    if (!parcel.WriteString(object->ifaceName_)) {
        return false;
    }
    if (!parcel.WriteString(object->domain_)) {
        return false;
    }
    if (!parcel.WriteUint32(object->netAddrList_.size())) {
        return false;
    }
    for (auto it = object->netAddrList_.begin(); it != object->netAddrList_.end(); it++) {
        if (!it->Marshalling(parcel)) {
            NETMGR_LOG_E("write objects net address to parcel failed");
            return false;
        }
    }
    if (!parcel.WriteUint32(object->dnsList_.size())) {
        return false;
    }
    for (auto it = object->dnsList_.begin(); it != object->dnsList_.end(); it++) {
        if (!it->Marshalling(parcel)) {
            NETMGR_LOG_E("write objects dns to parcel failed");
            return false;
        }
    }
    if (!parcel.WriteUint32(object->routeList_.size())) {
        return false;
    }
    for (auto it = object->routeList_.begin(); it != object->routeList_.end(); it++) {
        if (!it->Marshalling(parcel)) {
            NETMGR_LOG_E("write objects route to parcel failed");
            return false;
        }
    }
    if (!parcel.WriteUint16(object->mtu_)) {
        return false;
    }
    if (!parcel.WriteString(object->tcpBufferSizes_)) {
        return false;
    }
    return true;
}

void NetLinkInfo::Initialize()
{
    ifaceName_ = "";
    domain_ = "";
    std::list<INetAddr>().swap(netAddrList_);
    std::list<INetAddr>().swap(dnsList_);
    std::list<Route>().swap(routeList_);
    mtu_ = 0;
    tcpBufferSizes_ = "";
}

std::string NetLinkInfo::ToString(const std::string &tab) const
{
    std::string str;
    str.append(tab);
    str.append("[NetLinkInfo]");

    str.append(tab);
    str.append("ifaceName_ = ");
    str.append(ifaceName_);

    str.append(tab);
    str.append("domain_ = ");
    str.append(domain_);

    str.append(tab);
    str.append(ToStringAddr(tab));

    str.append(tab);
    str.append(ToStringDns(tab));

    str.append(tab);
    str.append(ToStringRoute(tab));
    str.append("routeList_ = ");

    str.append(tab);
    str.append("mtu_ = ");
    str.append(std::to_string(mtu_));

    str.append(tab);
    str.append("tcpBufferSizes_ = ");
    str.append(tcpBufferSizes_);
    return str;
}

std::string NetLinkInfo::ToStringAddr(const std::string &tab) const
{
    std::string str;
    str.append(tab);
    str.append("netAddrList_ = ");
    if (netAddrList_.empty()) {
        str.append("null");
        str.append(tab);
    } else {
        for (const auto &it : netAddrList_) {
            str.append(it.ToString(tab));
        }
    }
    return str;
}

std::string NetLinkInfo::ToStringDns(const std::string &tab) const
{
    std::string str;
    str.append(tab);
    str.append("dnsList_ = ");
    if (dnsList_.empty()) {
        str.append("null");
        str.append(tab);
    } else {
        for (const auto &it : dnsList_) {
            str.append(it.ToString(tab));
        }
    }
    return str;
}

std::string NetLinkInfo::ToStringRoute(const std::string &tab) const
{
    std::string str;
    str.append(tab);
    str.append("routeList_ = ");
    if (routeList_.empty()) {
        str.append("null");
        str.append(tab);
    } else {
        for (const auto &it : routeList_) {
            str.append(it.ToString(tab));
        }
    }
    return str;
}
} // namespace NetManagerStandard
} // namespace OHOS
