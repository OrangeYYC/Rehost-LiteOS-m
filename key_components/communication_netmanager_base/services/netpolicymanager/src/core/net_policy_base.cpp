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

#include "net_policy_base.h"

#include "net_policy_event_handler.h"

namespace OHOS {
namespace NetManagerStandard {
NetPolicyBase::NetPolicyBase() = default;

NetPolicyBase::~NetPolicyBase() = default;

std::shared_ptr<NetPolicyCallback> NetPolicyBase::GetCbInst()
{
    return DelayedSingleton<NetPolicyCallback>::GetInstance();
}

std::shared_ptr<NetPolicyFile> NetPolicyBase::GetFileInst()
{
    return DelayedSingleton<NetPolicyFile>::GetInstance();
}

std::shared_ptr<NetsysPolicyWrapper> NetPolicyBase::GetNetsysInst()
{
    return DelayedSingleton<NetsysPolicyWrapper>::GetInstance();
}

NetManagerCenter &NetPolicyBase::GetNetCenterInst()
{
    return NetManagerCenter::GetInstance();
}

void NetPolicyBase::SendEvent(int32_t eventId, int64_t delayTime)
{
    std::shared_ptr<PolicyEvent> policyEvent = std::make_shared<PolicyEvent>();
    SendEvent(eventId, policyEvent, delayTime);
}

void NetPolicyBase::SendEvent(int32_t eventId, std::shared_ptr<PolicyEvent> &policyEvent, int64_t delayTime)
{
    policyEvent->eventId = eventId;
    policyEvent->sender = shared_from_this();
    DelayedSingleton<NetPolicyCore>::GetInstance()->SendEvent(eventId, policyEvent, delayTime);
}
} // namespace NetManagerStandard
} // namespace OHOS
