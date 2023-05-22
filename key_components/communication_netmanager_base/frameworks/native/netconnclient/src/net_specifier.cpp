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

#include "net_specifier.h"

#include <functional>

#include "parcel.h"
#include "refbase.h"

#include "net_all_capabilities.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
bool NetSpecifier::SpecifierIsValid() const
{
    if (ident_.empty() && netCapabilities_.CapsIsNull()) {
        return false;
    }
    return netCapabilities_.CapsIsValid();
}

void NetSpecifier::SetCapabilities(const std::set<NetCap> &netCaps)
{
    netCapabilities_.netCaps_ = netCaps;
}

void NetSpecifier::SetCapability(NetCap netCap)
{
    netCapabilities_.netCaps_.clear();
    netCapabilities_.netCaps_.insert(netCap);
}

void NetSpecifier::SetTypes(const std::set<NetBearType> &bearerTypes)
{
    netCapabilities_.bearerTypes_ = bearerTypes;
}

void NetSpecifier::SetType(NetBearType bearerType)
{
    netCapabilities_.bearerTypes_.clear();
    netCapabilities_.bearerTypes_.insert(bearerType);
}

bool NetSpecifier::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(ident_)) {
        return false;
    }
    return netCapabilities_.Marshalling(parcel);
}

sptr<NetSpecifier> NetSpecifier::Unmarshalling(Parcel &parcel)
{
    sptr<NetSpecifier> ptr = new (std::nothrow) NetSpecifier();
    if (ptr == nullptr) {
        NETMGR_LOG_E("make_unique<NetSpecifier>() failed");
        return nullptr;
    }
    if (!parcel.ReadString(ptr->ident_)) {
        return nullptr;
    }
    if (!ptr->netCapabilities_.Unmarshalling(parcel)) {
        return nullptr;
    }
    return ptr;
}

bool NetSpecifier::Marshalling(Parcel &parcel, const sptr<NetSpecifier> &object)
{
    if (object == nullptr) {
        NETMGR_LOG_E("NetSpecifier object ptr is nullptr");
        return false;
    }
    if (!parcel.WriteString(object->ident_)) {
        return false;
    }
    return object->netCapabilities_.Marshalling(parcel);
}

std::string NetSpecifier::ToString(const std::string &tab) const
{
    std::string str;
    str.append(tab);
    str.append("[NetSpecifier]");
    str.append(tab);
    str.append("ident_ = ");
    str.append(ident_);

    str.append(netCapabilities_.ToString(tab));

    return str;
}
} // namespace NetManagerStandard
} // namespace OHOS
