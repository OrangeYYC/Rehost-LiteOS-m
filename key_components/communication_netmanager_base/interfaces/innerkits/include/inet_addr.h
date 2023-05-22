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

#ifndef INET_ADDR_H
#define INET_ADDR_H

#include <string>

#include "parcel.h"

namespace OHOS {
namespace NetManagerStandard {
struct INetAddr : public Parcelable {
    typedef enum {
        UNKNOWN = 0x00,
        IPV4 = 0x01,
        IPV6 = 0x02,
    } IpType;

    uint8_t type_ = UNKNOWN;
    uint8_t family_ = 0x00;
    uint8_t prefixlen_ = 0;
    std::string address_;
    std::string netMask_;
    std::string hostName_;
    uint8_t port_ = 0;

    bool operator==(const INetAddr& obj) const;

    virtual bool Marshalling(Parcel &parcel) const override;
    static sptr<INetAddr> Unmarshalling(Parcel &parcel);
    static bool Marshalling(Parcel &parcel, const sptr<INetAddr> &object);
    std::string ToString(const std::string &tab) const;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif