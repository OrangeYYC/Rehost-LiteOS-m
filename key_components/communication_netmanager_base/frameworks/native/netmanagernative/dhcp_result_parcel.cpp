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
#include "dhcp_result_parcel.h"
#include "netnative_log_wrapper.h"

namespace OHOS {
namespace NetsysNative {
DhcpResultParcel::DhcpResultParcel() {}

bool DhcpResultParcel::Marshalling(Parcel &parcel) const
{
    parcel.WriteString(iface_);
    parcel.WriteString(ipAddr_);
    parcel.WriteString(gateWay_);
    parcel.WriteString(subNet_);
    parcel.WriteString(route1_);
    parcel.WriteString(route2_);
    parcel.WriteString(dns1_);
    parcel.WriteString(dns2_);
    return true;
}

sptr<DhcpResultParcel> DhcpResultParcel::Unmarshalling(Parcel &parcel)
{
    sptr<DhcpResultParcel> ptr = new (std::nothrow) DhcpResultParcel();
    ptr->iface_ = parcel.ReadString();
    ptr->ipAddr_ = parcel.ReadString();
    ptr->gateWay_ = parcel.ReadString();
    ptr->subNet_ = parcel.ReadString();
    ptr->route1_ = parcel.ReadString();
    ptr->route2_ = parcel.ReadString();
    ptr->dns1_ = parcel.ReadString();
    ptr->dns2_ = parcel.ReadString();
    return ptr;
}
} // namespace NetsysNative
} // namespace OHOS
