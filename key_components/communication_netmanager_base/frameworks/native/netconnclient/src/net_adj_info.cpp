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

#include "net_adj_info.h"

namespace OHOS {
namespace NetManagerStandard {
static constexpr uint32_t MAX_ADDR_SIZE = 16;
bool NetAdjInfo::Marshalling(Parcel &parcel) const
{
    // Write type_
    if (!parcel.WriteUint32(type_)) {
        return false;
    }

    // WRite identity_
    if (!parcel.WriteString(identity_)) {
        return false;
    }

    // Write fromIface_
    if (!parcel.WriteString(fromIface_)) {
        return false;
    }

    // Write adjQoeLevel_
    if (!parcel.WriteUint32(adjQoeLevel_)) {
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

    return true;
}

sptr<NetAdjInfo> NetAdjInfo::Unmarshalling(Parcel &parcel)
{
    sptr<NetAdjInfo> info = new (std::nothrow) NetAdjInfo();
    if (info != nullptr) {
        uint32_t size = 0;
        if (!parcel.ReadUint32(info->type_) || !parcel.ReadString(info->identity_) ||
            !parcel.ReadString(info->fromIface_) || !parcel.ReadUint32(info->adjQoeLevel_)) {
            return nullptr;
        }
        // Read netAddrList_
        if (!parcel.ReadUint32(size)) {
            return nullptr;
        }
        size = size > MAX_ADDR_SIZE ? MAX_ADDR_SIZE : size;
        sptr<INetAddr> netAddr;
        for (uint32_t i = 0; i < size; i++) {
            netAddr = INetAddr::Unmarshalling(parcel);
            if (netAddr == nullptr) {
                return nullptr;
            }
            info->netAddrList_.push_back(*netAddr);
        }
    }
    return info;
}

bool NetAdjInfo::operator==(const NetAdjInfo &rhs) const
{
    if (this == &rhs) {
        return true;
    }
    return type_ == rhs.type_ && identity_ == rhs.identity_ && fromIface_ == rhs.fromIface_ &&
           adjQoeLevel_ == rhs.adjQoeLevel_ && netAddrList_ == rhs.netAddrList_;
}

bool NetAdjInfo::operator!=(const NetAdjInfo &rhs) const
{
    return !(rhs == *this);
}
} // namespace NetManagerStandard
} // namespace OHOS