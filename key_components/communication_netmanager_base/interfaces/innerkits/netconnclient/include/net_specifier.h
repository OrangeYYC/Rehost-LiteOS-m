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

#ifndef NET_SPECIFIER_H
#define NET_SPECIFIER_H

#include <string>

#include "parcel.h"

#include "net_all_capabilities.h"

namespace OHOS {
namespace NetManagerStandard {
struct NetSpecifier : public Parcelable {
    std::string ident_;
    NetAllCapabilities netCapabilities_;

    bool SpecifierIsValid() const;
    void SetCapabilities(const std::set<NetCap> &netCaps);
    void SetCapability(NetCap netCap);
    void SetTypes(const std::set<NetBearType> &bearerTypes);
    void SetType(NetBearType bearerType);

    virtual bool Marshalling(Parcel &parcel) const override;
    static sptr<NetSpecifier> Unmarshalling(Parcel &parcel);
    static bool Marshalling(Parcel &parcel, const sptr<NetSpecifier> &object);
    std::string ToString(const std::string &tab) const;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_SPECIFIER_H