/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef NET_SCORE_H
#define NET_SCORE_H

#include <unordered_map>
#include <vector>

#include "net_supplier.h"

namespace OHOS {
namespace NetManagerStandard {
using NetTypeScore = std::unordered_map<NetBearType, int32_t>;
constexpr int32_t NET_TYPE_SCORE_INTERVAL = 10;
constexpr int32_t NET_VALID_SCORE = 4 * NET_TYPE_SCORE_INTERVAL;
enum class NetTypeScoreValue : int32_t {
    USB_VALUE               = 4 * NET_TYPE_SCORE_INTERVAL,
    BLUETOOTH_VALUE         = 5 * NET_TYPE_SCORE_INTERVAL,
    CELLULAR_VALUE          = 6 * NET_TYPE_SCORE_INTERVAL,
    WIFI_VALUE              = 7 * NET_TYPE_SCORE_INTERVAL,
    ETHERNET_VALUE          = 8 * NET_TYPE_SCORE_INTERVAL,
    VPN_VALUE               = 9 * NET_TYPE_SCORE_INTERVAL,
    WIFI_AWARE_VALUE        = 10 * NET_TYPE_SCORE_INTERVAL
};

class NetScore {
public:
    NetScore() = default;
    ~NetScore() = default;
    bool GetServiceScore(sptr<NetSupplier> &supplier);

private:
    NetTypeScore netTypeScore_ = {
        {BEARER_CELLULAR, static_cast<int32_t>(NetTypeScoreValue::CELLULAR_VALUE)},
        {BEARER_WIFI, static_cast<int32_t>(NetTypeScoreValue::WIFI_VALUE)},
        {BEARER_BLUETOOTH, static_cast<int32_t>(NetTypeScoreValue::BLUETOOTH_VALUE)},
        {BEARER_ETHERNET, static_cast<int32_t>(NetTypeScoreValue::ETHERNET_VALUE)},
        {BEARER_VPN, static_cast<int32_t>(NetTypeScoreValue::VPN_VALUE)},
        {BEARER_WIFI_AWARE, static_cast<int32_t>(NetTypeScoreValue::WIFI_AWARE_VALUE)}};
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_SCORE_H
