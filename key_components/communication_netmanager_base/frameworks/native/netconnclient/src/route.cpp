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

#include "route.h"

#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
bool Route::operator==(const Route &obj) const
{
    bool out = true;
    out = out && (iface_ == obj.iface_);
    out = out && (destination_ == obj.destination_);
    out = out && (gateway_ == obj.gateway_);
    return out;
}

bool Route::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(iface_)) {
        return false;
    }
    if (!destination_.Marshalling(parcel)) {
        NETMGR_LOG_E("write destination_ to parcel failed");
        return false;
    }
    if (!gateway_.Marshalling(parcel)) {
        NETMGR_LOG_E("write gateway_ to parcel failed");
        return false;
    }
    if (!parcel.WriteInt32(rtnType_)) {
        return false;
    }
    if (!parcel.WriteInt32(mtu_)) {
        return false;
    }
    if (!parcel.WriteBool(isHost_)) {
        return false;
    }
    if (!parcel.WriteBool(hasGateway_)) {
        return false;
    }
    if (!parcel.WriteBool(isDefaultRoute_)) {
        return false;
    }
    return true;
}

sptr<Route> Route::Unmarshalling(Parcel &parcel)
{
    sptr<Route> ptr = new (std::nothrow) Route();
    if (ptr == nullptr) {
        NETMGR_LOG_E("make_unique<Route>() failed");
        return nullptr;
    }
    if (!parcel.ReadString(ptr->iface_)) {
        return nullptr;
    }
    sptr<INetAddr> destination = INetAddr::Unmarshalling(parcel);
    if (destination == nullptr) {
        NETMGR_LOG_E("read destination from parcel failed");
        return nullptr;
    }
    ptr->destination_ = *destination;
    sptr<INetAddr> gateway = INetAddr::Unmarshalling(parcel);
    if (gateway == nullptr) {
        NETMGR_LOG_E("read gateway from parcel failed");
        return nullptr;
    }
    ptr->gateway_ = *gateway;
    if (!parcel.ReadInt32(ptr->rtnType_)) {
        return nullptr;
    }
    if (!parcel.ReadInt32(ptr->mtu_)) {
        return nullptr;
    }
    if (!parcel.ReadBool(ptr->isHost_)) {
        return nullptr;
    }
    if (!parcel.ReadBool(ptr->hasGateway_)) {
        return nullptr;
    }
    if (!parcel.ReadBool(ptr->isDefaultRoute_)) {
        return nullptr;
    }
    return ptr;
}

bool Route::Marshalling(Parcel &parcel, const sptr<Route> &object)
{
    if (object == nullptr) {
        NETMGR_LOG_E("Route object ptr is nullptr");
        return false;
    }
    if (!parcel.WriteString(object->iface_)) {
        return false;
    }
    if (!object->destination_.Marshalling(parcel)) {
        NETMGR_LOG_E("write object->destination_ to parcel failed");
        return false;
    }
    if (!object->gateway_.Marshalling(parcel)) {
        NETMGR_LOG_E("write object->gateway_ to parcel failed");
        return false;
    }
    if (!parcel.WriteInt32(object->rtnType_)) {
        return false;
    }
    if (!parcel.WriteInt32(object->mtu_)) {
        return false;
    }
    if (!parcel.WriteBool(object->isHost_)) {
        return false;
    }
    if (!parcel.WriteBool(object->hasGateway_)) {
        return false;
    }
    if (!parcel.WriteBool(object->isDefaultRoute_)) {
        return false;
    }
    return true;
}

std::string Route::ToString(const std::string &tab) const
{
    std::string str;
    str.append(tab);
    str.append("[Route]");

    str.append(tab);
    str.append("iface_ = ");
    str.append(iface_);

    str.append(tab);
    str.append("destination_ = ");
    str.append(destination_.ToString(tab));

    str.append("\n");
    str.append(tab);
    str.append("gateway_ = ");
    str.append(gateway_.ToString(tab));

    str.append("\n");
    str.append(tab);
    str.append("rtnType_ = ");
    str.append(std::to_string(rtnType_));

    str.append("\n");
    str.append(tab);
    str.append("mtu_ = ");
    str.append(std::to_string(mtu_));

    str.append("\n");
    str.append(tab);
    str.append("isHost_ = ");
    str.append(isHost_ ? "true" : "false");

    str.append("\n");
    str.append(tab);
    str.append("hasGateway_ = ");
    str.append(hasGateway_ ? "true" : "false");

    str.append("\n");
    str.append(tab);
    str.append("isDefaultRoute = ");
    str.append(isDefaultRoute_ ? "true" : "false");

    return str;
}
} // namespace NetManagerStandard
} // namespace OHOS
