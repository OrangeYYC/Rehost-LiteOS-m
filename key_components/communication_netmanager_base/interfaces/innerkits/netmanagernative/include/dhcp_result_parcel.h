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
#ifndef  DHCP_RESULT_PARCEL_H
#define  DHCP_RESULT_PARCEL_H

#include <string>
#include "parcel.h"

namespace OHOS {
namespace NetsysNative {
struct DhcpResultParcel : public Parcelable {
    DhcpResultParcel();
    ~DhcpResultParcel() {}

    std::string iface_;
    std::string ipAddr_;
    std::string gateWay_;
    std::string subNet_;
    std::string route1_;
    std::string route2_;
    std::string dns1_;
    std::string dns2_;
    bool Marshalling(Parcel &parcel) const override;
    static sptr<DhcpResultParcel> Unmarshalling(Parcel &parcel);
};
} // namespace NetsysNative
} // namespace OHOS
#endif // DHCP_RESULT_PARCEL_H