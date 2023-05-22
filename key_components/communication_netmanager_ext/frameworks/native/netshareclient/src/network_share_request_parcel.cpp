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

#include "network_share_request_parcel.h"
#include "netmgr_ext_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
bool NetworkShareRequestParcel::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteInt32(networkShareType_)) {
        return false;
    }
    if (!parcel.WriteInt32(connectivityScope_)) {
        return false;
    }

    if (!localIPv4Address_.Marshalling(parcel)) {
        NETMGR_EXT_LOG_E("write localIPv4Address_ to parcel failed");
        return false;
    }
    if (!staticClientAddress_.Marshalling(parcel)) {
        NETMGR_EXT_LOG_E("write staticClientAddress_ to parcel failed");
        return false;
    }

    if (!parcel.WriteBool(exemptFromEntitlementCheck_)) {
        return false;
    }
    if (!parcel.WriteBool(showProvisioningUi_)) {
        return false;
    }

    return true;
}

sptr<NetworkShareRequestParcel> NetworkShareRequestParcel::Unmarshalling(Parcel &parcel)
{
    sptr<NetworkShareRequestParcel> ptr = new NetworkShareRequestParcel();
    if (ptr == nullptr) {
        return nullptr;
    }
    int32_t nsType = 0;
    if (!parcel.ReadInt32(nsType)) {
        return nullptr;
    }
    ptr->networkShareType_ = nsType;

    int32_t conScope = 0;
    if (!parcel.ReadInt32(conScope)) {
        return nullptr;
    }
    ptr->connectivityScope_ = conScope;

    sptr<INetAddr> localIPv4Addr = INetAddr::Unmarshalling(parcel);
    if (localIPv4Addr == nullptr) {
        NETMGR_EXT_LOG_E("localIPv4Addr is null");
        return nullptr;
    }
    ptr->localIPv4Address_ = *localIPv4Addr;

    sptr<INetAddr> staticClientAddr = INetAddr::Unmarshalling(parcel);
    if (staticClientAddr == nullptr) {
        NETMGR_EXT_LOG_E("staticClientAddr is null");
        return nullptr;
    }
    ptr->staticClientAddress_ = *staticClientAddr;

    bool efEntitlementCheck = false;
    if (!parcel.ReadBool(efEntitlementCheck)) {
        return nullptr;
    }
    ptr->exemptFromEntitlementCheck_ = efEntitlementCheck;

    bool showProvUi = false;
    if (!parcel.ReadBool(showProvUi)) {
        return nullptr;
    }
    ptr->showProvisioningUi_ = showProvUi;

    return ptr;
}
} // namespace NetManagerStandard
} // namespace OHOS