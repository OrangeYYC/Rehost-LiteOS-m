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

#ifndef NETMANAGER_BASE_INCLUDE_EVENT_REPORT_H
#define NETMANAGER_BASE_INCLUDE_EVENT_REPORT_H

#include <string>

#include "hisysevent.h"

using HiSysEventType = OHOS::HiviewDFX::HiSysEvent::EventType;
using HiSysEvent = OHOS::HiviewDFX::HiSysEvent;

namespace OHOS {
namespace NetManagerStandard {
enum class NetConnEventType {
    UNKNOW = 0,
    /***********FAULT EVENT**************/
    NET_CONN_SUPPLER_FAULT,
    NET_CONN_REQUEST_FAULT,
    NET_CONN_MONITOR_FAULT,
    /***********BEHAVIOR EVENT***********/
    NET_CONN_SUPPLER_STAT,
    NET_CONN_REQUEST_STAT,
    NET_CONN_MONITOR_STAT,
};

enum HiSysEventType {
    FAULT = 1,     // system fault event
    STATISTIC = 2, // system statistic event
    SECURITY = 3,  // system security event
    BEHAVIOR = 4   // system behavior event
};

enum NetConnSupplerFault {
    FAULT_UPDATE_SUPPLIERINFO_INV_PARAM = (-1),
    FAULT_CREATE_PHYSICAL_NETWORK_FAILED = (-2),
    FAULT_UPDATE_NETLINK_INFO_INV_PARAM = (-3),
    FAULT_UPDATE_NETLINK_INFO_FAILED = (-4),
    FAULT_SET_DEFAULT_NETWORK_FAILED = (-5),
    FAULT_DELETE_PHYSICAL_NETWORK_FAILED = (-6),
    FAULT_CLEAR_DEFAULT_NETWORK_FAILED = (-7)
};

enum NetConnRequestFault { FAULT_INVALID_PARAMETER = (-1) };

enum NetConnMonitorFault {
    FAULT_BIND_SOCKET_FAILED = (-1),
};

struct EventInfo {
    int32_t updateSupplierId = 0;
    std::string supplierInfo;
    std::string netlinkInfo;
    int32_t updateNetlinkId = 0;
    std::string capabilities;
    int32_t socketFd = 0;
    int32_t netId = 0;
    int32_t bearerType = 0;
    std::string ident;
    int32_t supplierId = 0;
    std::string callbackAvailable;
    std::string supplierIdent;
    std::string netcaps;
    int32_t monitorStatus = 0;

    int32_t errorType = 0;
    std::string errorMsg;
};

class EventReport {
public:
    static void SendSupplierFaultEvent(const EventInfo &eventInfo);
    static void SendSupplierBehaviorEvent(const EventInfo &eventInfo);
    static void SendRequestFaultEvent(const EventInfo &eventInfo);
    static void SendRequestBehaviorEvent(const EventInfo &eventInfo);
    static void SendMonitorFaultEvent(const EventInfo &eventInfo);
    static void SendMonitorBehaviorEvent(const EventInfo &eventInfo);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETMANAGER_BASE_INCLUDE_EVENT_REPORT_H
