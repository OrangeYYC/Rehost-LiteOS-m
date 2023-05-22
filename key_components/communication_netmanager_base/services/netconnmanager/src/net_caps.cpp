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

#include "net_caps.h"

namespace OHOS {
namespace NetManagerStandard {
NetCaps::NetCaps(const std::set<NetCap> &caps)
{
    for (auto cap : caps) {
        InsertNetCap(cap);
    }
}

bool NetCaps::operator==(const NetCaps &netCaps) const
{
    return (caps_ == netCaps.caps_);
}

bool NetCaps::IsValidNetCap(NetCap cap)
{
    return (cap >= NET_CAPABILITY_MMS) && (cap < NET_CAPABILITY_INTERNAL_DEFAULT);
}

void NetCaps::InsertNetCap(NetCap cap)
{
    if (IsValidNetCap(cap)) {
        caps_ |= (1 << cap);
    }
}

void NetCaps::RemoveNetCap(NetCap cap)
{
    if (IsValidNetCap(cap)) {
        caps_ &= ~(1 << cap);
    }
}

bool NetCaps::HasNetCap(NetCap cap) const
{
    return (caps_ >> cap) & 1;
}

bool NetCaps::HasNetCaps(const std::set<NetCap> &caps) const
{
    return std::all_of(caps.cbegin(), caps.cend(), [this] (const NetCap &cap) { return HasNetCap(cap); });
}

std::set<NetCap> NetCaps::ToSet() const
{
    std::set<NetCap> ret;
    for (auto cap = static_cast<NetCap>(0); cap < NET_CAPABILITY_INTERNAL_DEFAULT; cap = static_cast<NetCap>(cap + 1)) {
        if (HasNetCap(cap)) {
            ret.insert(cap);
        }
    }
    return ret;
}
} // namespace NetManagerStandard
} // namespace OHOS