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

#ifndef NETWORKSHARE_STATE_COMMMON_H
#define NETWORKSHARE_STATE_COMMMON_H

#include "net_all_capabilities.h"
#include "net_handle.h"
#include "net_link_info.h"

namespace OHOS {
namespace NetManagerStandard {
constexpr int CMD_NETSHARE_REQUESTED = 1;
constexpr int CMD_NETSHARE_UNREQUESTED = 2;
constexpr int CMD_INTERFACE_DOWN = 3;
constexpr int CMD_IP_FORWARDING_ENABLE_ERROR = 4;
constexpr int CMD_IP_FORWARDING_DISABLE_ERROR = 5;
constexpr int CMD_START_SHARING_ERROR = 6;
constexpr int CMD_STOP_SHARING_ERROR = 7;
constexpr int CMD_SET_DNS_FORWARDERS_ERROR = 8;
constexpr int CMD_NOTIFY_PREFIX_CONFLICT = 9;
constexpr int CMD_NEW_PREFIX_REQUEST = 10;
constexpr int CMD_NETSHARE_CONNECTION_CHANGED = 11;
constexpr int NETWORKSHARING_SHARING_NO_ERROR = 0;

// interface sub statemachine has requested
constexpr int EVENT_IFACE_SM_STATE_ACTIVE = 20;
// interface sub statemachine has unrequested
constexpr int EVENT_IFACE_SM_STATE_INACTIVE = 21;
constexpr int EVENT_IFACE_UPDATE_LINKPROPERTIES = 22;
constexpr int EVENT_UPSTREAM_CALLBACK = 23;
// upstream connection change
constexpr int CMD_UPSTREAM_CHANGED = 24;
// has no valid upstream conn, check again after a delay
constexpr int CMD_RETRY_UPSTREAM = 25;
// after occur the error and want to clear it
constexpr int CMD_CLEAR_ERROR = 26;

constexpr int SUB_SM_STATE_AVAILABLE = 0;
constexpr int SUB_SM_STATE_UNAVAILABLE = 1;
constexpr int SUB_SM_STATE_SHARED = 2;

constexpr int EVENT_UPSTREAM_CALLBACK_ON_CAPABILITIES = 0;
constexpr int EVENT_UPSTREAM_CALLBACK_ON_LINKPROPERTIES = 1;
constexpr int EVENT_UPSTREAM_CALLBACK_ON_LOST = 2;
constexpr int EVENT_UPSTREAM_CALLBACK_DEFAULT_SWITCHED = 3;
constexpr int EVENT_UPSTREAM_CALLBACK_DEFAULT_SWITCHED_BY_LOST = 4;
constexpr int EVENT_UPSTREAM_CALLBACK_LOCAL_PREFIXES = 5;

constexpr int32_t CALLBACK_DEFAULT_INTERNET_NETWORK = 1;
constexpr int32_t CALLBACK_LISTEN_ALL_NETWORK = 2;
constexpr int32_t CALLBACK_MOBILE_REQUEST_NETWORK = 3;

// delayTime
constexpr int32_t HANDLER_DELAY_TIME_MS = 1000;    // 1 second
constexpr int32_t UPSTREAM_HANDLE_TIME_MS = 10000; // 10 second

constexpr int32_t PREFIX_LENGTH_16 = 16;
constexpr int32_t PREFIX_LENGTH_24 = 24;

constexpr int32_t SET_IFACE_UP_NULL = -1;
constexpr int32_t SET_IFACE_UP_TRUE = 1;
constexpr int32_t SET_IFACE_UP_FALSE = 0;

constexpr int32_t SET_WIFI_DEFAULT_CHANNEL = 6;
constexpr int32_t SET_WIFI_MAX_CONNECT = 10;

constexpr int NO_NEXT_STATE = -1;
enum { MAINSTATE_INIT = 1, MAINSTATE_ALIVE, MAINSTATE_ERROR, MAINSTATE_MAX } MainState;

enum { SUBSTATE_INIT = 11, SUBSTATE_SHARED, SUBSTATE_RESTART, SUBSTATE_UNAVAILABLE, SUBSTATE_MAX } SubState;

enum class TrafficType { TRAFFIC_RX = 1, TRAFFIC_TX = 2, TRAFFIC_ALL = 3 };

struct UpstreamNetworkInfo {
    sptr<NetHandle> netHandle_;
    sptr<NetAllCapabilities> netAllCap_;
    sptr<NetLinkInfo> netLinkPro_; // net link property
    UpstreamNetworkInfo(sptr<NetHandle> &netHandle, sptr<NetAllCapabilities> &netcap, sptr<NetLinkInfo> &netlinkinfo)
        : netHandle_(netHandle), netAllCap_(netcap), netLinkPro_(netlinkinfo)
    {
    }
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETWORKSHARE_STATE_COMMMON_H
