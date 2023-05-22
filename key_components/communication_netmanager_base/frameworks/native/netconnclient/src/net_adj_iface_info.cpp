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

#include "net_adj_iface_info.h"

namespace OHOS {
namespace NetManagerStandard {
static constexpr uint32_t MAX_ADDR_SIZE = 16;
static constexpr uint32_t MAX_ROUTE_SIZE = 32;
bool NetAdjIfaceInfo::Marshalling(OHOS::Parcel &parcel) const
{
    // Write type_
    if (!parcel.WriteUint32(type_)) {
        return false;
    }
    // Write ifaceName_
    if (!parcel.WriteString(ifaceName_)) {
        return false;
    }
    // Write netAddrList_
    if (!parcel.WriteUint32(netAddrList_.size())) {
        return false;
    }
    for (const auto &addr : netAddrList_) {
        if (!addr.Marshalling(parcel)) {
            return false;
        }
    }
    // Write routeList_
    if (!parcel.WriteUint32(routeList_.size())) {
        return false;
    }
    for (const auto &route : routeList_) {
        if (!route.Marshalling(parcel)) {
            return false;
        }
    }
    return true;
}

sptr<NetAdjIfaceInfo> NetAdjIfaceInfo::Unmarshalling(OHOS::Parcel &parcel)
{
    sptr<NetAdjIfaceInfo> info = new (std::nothrow) NetAdjIfaceInfo();
    if (info != nullptr) {
        uint32_t size = 0;
        if (!parcel.ReadUint32(info->type_) || !parcel.ReadString(info->ifaceName_) || !parcel.ReadUint32(size)) {
            return nullptr;
        }
        // Read netAddrList_
        size = size > MAX_ADDR_SIZE ? MAX_ADDR_SIZE : size;
        sptr<INetAddr> netAddr;
        for (uint32_t i = 0; i < size; i++) {
            netAddr = INetAddr::Unmarshalling(parcel);
            if (netAddr == nullptr) {
                return nullptr;
            }
            info->netAddrList_.push_back(*netAddr);
        }
        // Read routeList_
        if (!parcel.ReadUint32(size)) {
            return nullptr;
        }
        size = size > MAX_ROUTE_SIZE ? MAX_ROUTE_SIZE : size;
        sptr<Route> route;
        for (uint32_t i = 0; i < size; i++) {
            route = Route::Unmarshalling(parcel);
            if (route == nullptr) {
                return nullptr;
            }
            info->routeList_.push_back(*route);
        }
    }
    return info;
}

bool NetAdjIfaceInfo::operator==(const NetAdjIfaceInfo &rhs) const
{
    if (this == &rhs) {
        return true;
    }
    return type_ == rhs.type_ && ifaceName_ == rhs.ifaceName_ && netAddrList_ == rhs.netAddrList_ &&
           routeList_ == rhs.routeList_;
}

bool NetAdjIfaceInfo::operator!=(const NetAdjIfaceInfo &rhs) const
{
    return !(rhs == *this);
}
} // namespace NetManagerStandard
} // namespace OHOS
