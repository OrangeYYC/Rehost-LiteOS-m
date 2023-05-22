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

#ifndef ROUTE_H
#define ROUTE_H

#include <iosfwd>
#include <cstdint>
#include <string>

#include "parcel.h"
#include "refbase.h"

#include "inet_addr.h"

namespace OHOS {
namespace NetManagerStandard {
enum {
    RTN_UNICAST = 1,
    RTN_UNREACHABLE = 7,
    RTN_THROW = 9
};

struct Route : public Parcelable {
    std::string iface_;
    INetAddr destination_;
    INetAddr gateway_;
    int32_t rtnType_ = RTN_UNICAST;
    int32_t mtu_ = 0;
    bool isHost_ = false;
    bool hasGateway_ = true;
    bool isDefaultRoute_ = false;

    bool operator==(const Route& obj) const;

    virtual bool Marshalling(Parcel &parcel) const override;
    static sptr<Route> Unmarshalling(Parcel &parcel);
    static bool Marshalling(Parcel &parcel, const sptr<Route> &object);
    std::string ToString(const std::string &tab) const;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif