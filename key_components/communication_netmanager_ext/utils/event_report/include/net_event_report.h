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

#ifndef NETMGR_EXT_INCLUDE_EVENT_REPORT_H
#define NETMGR_EXT_INCLUDE_EVENT_REPORT_H

#include <string>

#include "hisysevent.h"

using HiSysEventType = OHOS::HiviewDFX::HiSysEvent::EventType;
using HiSysEvent = OHOS::HiviewDFX::HiSysEvent;

namespace OHOS {
namespace NetManagerStandard {
struct EventInfo {
    int32_t sharingType = 0;
    int32_t operatorType = 0;
    int32_t sharingCount = 0;

    int32_t errorType = 0;
    std::string errorMsg;
};

enum class NetworkShareEventOperator {
    OPERATION_START_SA = 0,
    OPERATION_ENABLE_IFACE,
    OPERATION_TURNON_IP_FORWARD,
    OPERATION_GET_UPSTREAM,
    OPERATION_CONFIG_FORWARD,
    OPERATION_CHECK_SA = 10,
    OPERATION_DISABLE_IFACE,
    OPERATION_TURNOFF_IP_FORWARD,
    OPERATION_CANCEL_FORWARD,
};

enum class NetworkShareEventErrorType {
    ERROR_START_SA = -100,
    ERROR_CHECK_SA = -101,
    ERROR_ENABLE_IFACE = -110,
    ERROR_DISABLE_IFACE = -111,
    ERROR_TURNON_IP_FORWARD = -120,
    ERROR_TURNOFF_IP_FORWARD = -121,
    ERROR_CONFIG_FORWARD = -130,
    ERROR_CANCEL_FORWARD = -131,
    ERROR_GET_UPSTREAM = -140,
};

enum class NetworkShareEventType {
    SETUP_EVENT = 0,
    CANCEL_EVENT = 1,
    BEHAVIOR_EVENT = 2,
};

class NetEventReport {
public:
    static void SendSetupFaultEvent(const EventInfo &eventInfo);
    static void SendCancleFaultEvent(const EventInfo &eventInfo);
    static void SendTimeBehaviorEvent(const EventInfo &eventInfo);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETMGR_EXT_INCLUDE_EVENT_REPORT_H
