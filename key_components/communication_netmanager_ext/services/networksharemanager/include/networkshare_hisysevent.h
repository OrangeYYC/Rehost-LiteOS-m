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

#ifndef NETWORKSHARE_HISYSEVENT_H
#define NETWORKSHARE_HISYSEVENT_H

#include <string>

#include "net_event_report.h"
#include "net_manager_ext_constants.h"

namespace OHOS {
namespace NetManagerStandard {
class NetworkShareHisysEvent {
public:
    static NetworkShareHisysEvent &GetInstance();
    ~NetworkShareHisysEvent() = default;

    void SendFaultEvent(const NetworkShareEventOperator &operatorType, const NetworkShareEventErrorType &errorCode,
                        const std::string &errorMsg, const NetworkShareEventType &eventType);
    void SendFaultEvent(const SharingIfaceType &sharingType, const NetworkShareEventOperator &operatorType,
                        const NetworkShareEventErrorType &errorCode, const std::string &errorMsg,
                        const NetworkShareEventType &eventType);
    void SendBehaviorEvent(int32_t sharingCount, const SharingIfaceType &sharingType);

private:
    NetworkShareHisysEvent() = default;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETWORKSHARE_HISYSEVENT_H