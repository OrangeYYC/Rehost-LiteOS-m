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

#include "networkshare_hisysevent.h"

#include "net_event_report.h"

namespace OHOS {
namespace NetManagerStandard {

NetworkShareHisysEvent &NetworkShareHisysEvent::GetInstance()
{
    static NetworkShareHisysEvent instance;
    return instance;
}

void NetworkShareHisysEvent::SendFaultEvent(const NetworkShareEventOperator &operatorType,
                                            const NetworkShareEventErrorType &errorCode, const std::string &errorMsg,
                                            const NetworkShareEventType &eventType)
{
    EventInfo eventInfo;
    eventInfo.operatorType = static_cast<int32_t>(operatorType);
    eventInfo.errorType = static_cast<int32_t>(errorCode);
    eventInfo.errorMsg = errorMsg;
    eventInfo.sharingType = -1;
    if (eventType == NetworkShareEventType::SETUP_EVENT) {
        NetEventReport::SendSetupFaultEvent(eventInfo);
    } else if (eventType == NetworkShareEventType::CANCEL_EVENT) {
        NetEventReport::SendCancleFaultEvent(eventInfo);
    }
}

void NetworkShareHisysEvent::SendFaultEvent(const SharingIfaceType &sharingType,
                                            const NetworkShareEventOperator &operatorType,
                                            const NetworkShareEventErrorType &errorCode, const std::string &errorMsg,
                                            const NetworkShareEventType &eventType)
{
    EventInfo eventInfo;
    eventInfo.sharingType = static_cast<int32_t>(sharingType);
    eventInfo.operatorType = static_cast<int32_t>(operatorType);
    eventInfo.errorType = static_cast<int32_t>(errorCode);
    eventInfo.errorMsg = errorMsg;
    if (eventType == NetworkShareEventType::SETUP_EVENT) {
        NetEventReport::SendSetupFaultEvent(eventInfo);
    } else if (eventType == NetworkShareEventType::CANCEL_EVENT) {
        NetEventReport::SendCancleFaultEvent(eventInfo);
    }
}

void NetworkShareHisysEvent::SendBehaviorEvent(int32_t sharingCount, const SharingIfaceType &sharingType)
{
    EventInfo eventInfo;
    eventInfo.sharingCount = sharingCount;
    eventInfo.sharingType = static_cast<int32_t>(sharingType);
    NetEventReport::SendTimeBehaviorEvent(eventInfo);
}

} // namespace NetManagerStandard
} // namespace OHOS