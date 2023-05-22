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

//
// Created by root on 22-11-22.
//

#ifndef COMMUNICATION_NET_ADJ_IFACE_INFO_H
#define COMMUNICATION_NET_ADJ_IFACE_INFO_H

#include <list>

#include "parcel.h"
#include "inet_addr.h"
#include "route.h"
#include "net_adj_info.h"

namespace OHOS {
namespace NetManagerStandard {
struct NetAdjIfaceInfo : public Parcelable {
    // Type from {@code NetAdjInfo::NetAdjType}.
    uint32_t type_;

    // Interface name.
    std::string ifaceName_;

    // Network addresses.
    std::list<INetAddr> netAddrList_;

    // Routes.
    std::list<Route> routeList_;
    bool Marshalling(Parcel &parcel) const override;
    sptr<NetAdjIfaceInfo> Unmarshalling(Parcel &parcel);
    bool operator==(const NetAdjIfaceInfo &rhs) const;
    bool operator!=(const NetAdjIfaceInfo &rhs) const;
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif // COMMUNICATION_NET_ADJ_IFACE_INFO_H
