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

#include "netsys_event_message.h"

#include "netnative_log_wrapper.h"

namespace OHOS {
namespace nmd {
void NetsysEventMessage::PushMessage(NetsysEventMessage::Type type, const std::string &value)
{
    messageMap_[type] = value;
}

const std::string NetsysEventMessage::GetMessage(NetsysEventMessage::Type type)
{
    const std::string empty = "";
    if (messageMap_.find(type) == messageMap_.end()) {
        return empty;
    }
    return messageMap_[type];
}

void NetsysEventMessage::DumpMessage()
{
    NETNATIVE_LOG_D("DumpMessage: Action: %{public}d; SybSys: %{public}d; SeqNum: %{public}d; ", action_, subSys_,
                    seqNum_);
    for (auto &item : messageMap_) {
        NETNATIVE_LOG_D("type: %{public}d", item.first);
    }
}
} // namespace nmd
} // namespace OHOS