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

#include "net_policy_event_handler.h"

#include "net_mgr_log_wrapper.h"
#include "net_policy_core.h"

namespace OHOS {
namespace NetManagerStandard {
NetPolicyEventHandler::NetPolicyEventHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner,
                                             const std::shared_ptr<NetPolicyCore> &core)
    : EventHandler(runner), core_(core)
{
}

NetPolicyEventHandler::~NetPolicyEventHandler() = default;

void NetPolicyEventHandler::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        NETMGR_LOG_E("NetPolicyEventHandler::ProcessEvent::parameter error");
        return;
    }

    if (core_ == nullptr) {
        NETMGR_LOG_E("Net policy core is null.");
        return;
    }
    core_->HandleEvent(event);
}
} // namespace NetManagerStandard
} // namespace OHOS
