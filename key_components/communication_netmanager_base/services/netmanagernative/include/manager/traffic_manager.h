/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef INCLUDE_TRAFFIC_MANAGER_H
#define INCLUDE_TRAFFIC_MANAGER_H

#include <ostream>
#include <string>
#include <vector>

namespace OHOS {
namespace nmd {
typedef struct TrafficStatsParcel {
    std::string iface;
    unsigned int ifIndex = 0;
    long rxBytes;
    long rxPackets;
    long txBytes;
    long txPackets;

    friend std::ostream &operator<<(std::ostream &os, const TrafficStatsParcel &parcel)
    {
        os << "iface: " << parcel.iface << "ifIndex: " << parcel.ifIndex << "rxBytes: " << parcel.rxBytes
           << "rxPackets: " << parcel.rxPackets << "txBytes: " << parcel.txBytes << "txPackets: " << parcel.txPackets;
        return os;
    }
} TrafficStatsParcel;

class TrafficManager {
public:
    TrafficManager();
    ~TrafficManager();

    static TrafficStatsParcel GetInterfaceTraffic(const std::string &ifName);
    static long GetAllRxTraffic();
    static long GetAllTxTraffic();
    static void TrafficManagerLog();
};
} // namespace nmd
} // namespace OHOS
#endif // INCLUDE_TRAFFIC_MANAGER_H
