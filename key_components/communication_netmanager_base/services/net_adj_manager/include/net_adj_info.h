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

#ifndef COMMUNICATION_NET_ADJ_INFO_H
#define COMMUNICATION_NET_ADJ_INFO_H

#include <list>

#include "parcel.h"
#include "inet_addr.h"

namespace OHOS {
namespace NetManagerStandard {
struct NetAdjInfo : public Parcelable {
    enum {
        // Wi-Fi Station and AP.
        TYPE_WIFI = 0,
        // Wi-Fi direct.
        TYPE_WIFI_P2P = 1,
        // Bluetooth.
        TYPE_BLUETOOTH = 2,
        // Bluetooth low energy.
        TYPE_BLE = 3,
        // USB.
        TYPE_USB = 4,
        // Ethernet.
        TYPE_ETHERNET = 5,
        // Spark link.
        TYPE_SPARK_LINK = 6,
        // Remote connection.
        TYPE_REMOTE = 7,
    } NetAdjType;

    // Type from {@code net_adj_info::NetAdjType}.
    uint32_t type_;

    // Identity of network adjacency.
    std::string identity_;

    // The interface where adjacency came from.
    std::string fromIface_;

    // Quality of experience of adjacency.
    uint32_t adjQoeLevel_;

    // The addresses of adjacency.
    std::list<INetAddr> netAddrList_;

    bool Marshalling(Parcel &parcel) const override;
    static sptr<NetAdjInfo> Unmarshalling(Parcel &parcel);
    bool operator==(const NetAdjInfo &rhs) const;
    bool operator!=(const NetAdjInfo &rhs) const;
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif // COMMUNICATION_NET_ADJ_INFO_H
