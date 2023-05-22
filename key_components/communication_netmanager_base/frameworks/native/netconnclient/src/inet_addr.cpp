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

#include "inet_addr.h"

#include "parcel.h"
#include "refbase.h"

#include "net_mgr_log_wrapper.h"
#include "netmanager_base_common_utils.h"

namespace OHOS {
namespace NetManagerStandard {
bool INetAddr::operator==(const INetAddr &obj) const
{
    bool out = true;
    out = out && (type_ == obj.type_);
    out = out && (family_ == obj.family_);
    out = out && (prefixlen_ == obj.prefixlen_);
    out = out && (address_ == obj.address_);
    out = out && (netMask_ == obj.netMask_);
    out = out && (hostName_ == obj.hostName_);
    out = out && (port_ == obj.port_);
    return out;
}

bool INetAddr::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteUint8(type_)) {
        return false;
    }
    if (!parcel.WriteUint8(family_)) {
        return false;
    }
    if (!parcel.WriteUint8(prefixlen_)) {
        return false;
    }
    if (!parcel.WriteString(address_)) {
        return false;
    }
    if (!parcel.WriteString(netMask_)) {
        return false;
    }
    if (!parcel.WriteString(hostName_)) {
        return false;
    }
    if (!parcel.WriteUint8(port_)) {
        return false;
    }
    return true;
}

sptr<INetAddr> INetAddr::Unmarshalling(Parcel &parcel)
{
    sptr<INetAddr> ptr = new (std::nothrow) INetAddr();
    if (ptr == nullptr) {
        NETMGR_LOG_E("create INetAddr failed");
        return nullptr;
    }
    if (!parcel.ReadUint8(ptr->type_)) {
        return nullptr;
    }
    if (!parcel.ReadUint8(ptr->family_)) {
        return nullptr;
    }
    if (!parcel.ReadUint8(ptr->prefixlen_)) {
        return nullptr;
    }
    if (!parcel.ReadString(ptr->address_)) {
        return nullptr;
    }
    if (!parcel.ReadString(ptr->netMask_)) {
        return nullptr;
    }
    if (!parcel.ReadString(ptr->hostName_)) {
        return nullptr;
    }
    if (!parcel.ReadUint8(ptr->port_)) {
        return nullptr;
    }
    return ptr;
}

bool INetAddr::Marshalling(Parcel &parcel, const sptr<INetAddr> &object)
{
    if (object == nullptr) {
        NETMGR_LOG_E("INetAddr object ptr is nullptr");
        return false;
    }
    if (!parcel.WriteUint8(object->type_)) {
        return false;
    }

    if (!parcel.WriteUint8(object->family_)) {
        return false;
    }

    if (!parcel.WriteUint8(object->prefixlen_)) {
        return false;
    }

    if (!parcel.WriteString(object->address_)) {
        return false;
    }

    if (!parcel.WriteString(object->netMask_)) {
        return false;
    }

    if (!parcel.WriteString(object->hostName_)) {
        return false;
    }

    if (!parcel.WriteUint8(object->port_)) {
        return false;
    }
    return true;
}

std::string INetAddr::ToString(const std::string &tab) const
{
    std::string str;  // print the member values of the INetAddr class
    str.append(tab);
    str.append("[INetAddr]");

    str.append(tab);
    str.append("type = ");
    str.append(std::to_string(type_));

    str.append(tab);
    str.append("family = ");
    str.append(std::to_string(family_));

    str.append(tab);
    str.append("prefixLength = ");
    str.append(std::to_string(prefixlen_));

    str.append(tab);
    str.append("address = ");
    str.append(CommonUtils::ToAnonymousIp(address_));

    str.append(tab);
    str.append("netMask = ");
    str.append(netMask_);

    str.append(tab);
    str.append("hostName = ");
    str.append(hostName_);

    str.append(tab);
    str.append("port = ");
    str.append(std::to_string(port_));
    return str;
}
} // namespace NetManagerStandard
} // namespace OHOS
