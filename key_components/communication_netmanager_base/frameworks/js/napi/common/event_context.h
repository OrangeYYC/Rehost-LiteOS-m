/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef EVENT_CONTEXT_H
#define EVENT_CONTEXT_H

#include <string>
#include <vector>

#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace NetManagerStandard {
// net policy
constexpr const char *NET_UID_POLICY_CHANGE = "netUidPolicyChange";
constexpr const char *NET_CELLULAR_POLICY_CHANGE = "netCellularPolicyChange";
constexpr const char *NET_STRATEGY_SWITCH_CHANGE = "netStrategySwitchChange";
// net status
constexpr const char *NET_AVAILABLE_CHANGE = "netAvailable";
constexpr const char *NET_BLOCK_STATUS_CHANGE = "netBlockStatusChange";
constexpr const char *NET_CAPABILITIES_CHANGE = "netCapabilitiesChange";
constexpr const char *NET_CONNECTION_CHANGE = "netConnectionPropertiesChange";
constexpr const char *NET_LOST_CHANGE = "netLost";
constexpr const char *NET_UNAVAILABLE_CHANGE = "netUnavailable";
constexpr const char *NET_STATS_CHANGE = "netStatsChange";
constexpr int32_t EVENT_CONTENT_MAX_BYTE = 64;

enum {
    EVENT_NET_UNKNOW_CHANGE = -1,
    EVENT_NET_UID_POLICY_CHANGE = 21,
    EVENT_NET_CELLULAR_POLICY_CHANGE = 22,
    EVENT_NET_STRATEGY_SWITCH_CHANGE = 23,
    EVENT_NET_AVAILABLE_CHANGE = 31,
    EVENT_NET_CAPABILITIES_CHANGE = 32,
    EVENT_NET_CONNECTION_CHANGE = 33,
    EVENT_NET_LOST_CHANGE = 34,
    EVENT_NET_STATS_CHANGE = 35,
    EVENT_NET_BLOCK_STATUS_CHANGE = 36,
    EVENT_NET_UNAVAILABLE_CHANGE = 37,
};

// event listener context
struct EventListener {
    EventListener() {}
    int32_t eventId = 0;
    napi_env env = nullptr;
    napi_ref callbackRef = nullptr;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // EVENT_CONTEXT_H
