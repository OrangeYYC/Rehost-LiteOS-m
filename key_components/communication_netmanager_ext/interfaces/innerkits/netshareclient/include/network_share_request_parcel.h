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

#ifndef NETSHARE_STARTED_PARCEL_H
#define NETSHARE_STARTED_PARCEL_H

#include "parcel.h"
#include "inet_addr.h"

namespace OHOS {
namespace NetManagerStandard {
struct NetworkShareRequestParcel : public Parcelable {
    int32_t networkShareType_;
    int32_t connectivityScope_;
    INetAddr localIPv4Address_;
    INetAddr staticClientAddress_;
    bool exemptFromEntitlementCheck_;
    bool showProvisioningUi_;

    virtual bool Marshalling(Parcel &parcel) const override;
    static sptr<NetworkShareRequestParcel> Unmarshalling(Parcel &parcel);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETSHARE_STARTED_PARCEL_H