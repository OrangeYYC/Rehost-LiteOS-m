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

#include "net_policy_core.h"

#include "net_mgr_log_wrapper.h"
#include "net_policy_base.h"
#include "net_policy_event_handler.h"
#include "netmanager_base_common_utils.h"

namespace OHOS {
namespace NetManagerStandard {
constexpr const char *EVENT_PARAM_DELETED_UID = "DeletedUid";
static constexpr uint32_t CORE_EVENT_PRIORITY = 1;

NetPolicyCore::NetPolicyCore() = default;

NetPolicyCore::~NetPolicyCore()
{
    cores_.clear();
}

void NetPolicyCore::Init(std::shared_ptr<NetPolicyEventHandler> &handler)
{
    handler_ = handler;
    SubscribeCommonEvent();
}

void NetPolicyCore::HandleEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (!event) {
        NETMGR_LOG_E("HandleEvent event is null.");
        return;
    }

    for (const auto &core : cores_) {
        auto eventId = event->GetInnerEventId();
        auto eventData = event->GetSharedObject<PolicyEvent>();
        if (eventData && core && core != eventData->sender) {
            core->HandleEvent(eventId, eventData);
        }
    }
}

void NetPolicyCore::SendEvent(int32_t eventId, std::shared_ptr<PolicyEvent> &eventData, int64_t delayTime)
{
    NETMGR_LOG_D("NetPolicyCore SendEvent: eventId[%{public}d]", eventId);
    auto event = AppExecFwk::InnerEvent::Get(eventId, eventData);
    if (handler_ == nullptr) {
        NETMGR_LOG_E("handler is null");
        return;
    }
    
    handler_->SendEvent(event, delayTime);
}

void NetPolicyCore::SubscribeCommonEvent()
{
    NETMGR_LOG_D("SubscribeCommonEvent");
    EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(COMMON_EVENT_POWER_SAVE_MODE_CHANGED);
    matchingSkills.AddEvent(COMMON_EVENT_DEVICE_IDLE_MODE_CHANGED);
    matchingSkills.AddEvent(COMMON_EVENT_UID_REMOVED);
    matchingSkills.AddEvent(COMMON_EVENT_NET_QUOTA_WARNING);
    EventFwk::CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(CORE_EVENT_PRIORITY);
    EventFwk::CommonEventManager::SubscribeCommonEvent(shared_from_this());
}

void NetPolicyCore::OnReceiveEvent(const EventFwk::CommonEventData &eventData)
{
    const auto &action = eventData.GetWant().GetAction();
    const auto &data = eventData.GetData();
    const auto &code = eventData.GetCode();
    NETMGR_LOG_I("OnReceiveEvent action:[%{public}s], data:[%{public}s], code:[%{public}d]", action.c_str(),
                 data.c_str(), code);
    if (action == COMMON_EVENT_POWER_SAVE_MODE_CHANGED) {
        bool isPowerSave = (code == SAVE_MODE || code == LOWPOWER_MODE);
        auto policyEvent = std::make_shared<PolicyEvent>();
        policyEvent->powerSaveMode = isPowerSave;
        SendEvent(NetPolicyEventHandler::MSG_POWER_SAVE_MODE_CHANGED, policyEvent);
        return;
    }

    if (action == COMMON_EVENT_DEVICE_IDLE_MODE_CHANGED) {
        bool isDeviceIdle = (code == EXTREME_MODE);
        auto policyEvent = std::make_shared<PolicyEvent>();
        policyEvent->deviceIdleMode = isDeviceIdle;
        SendEvent(NetPolicyEventHandler::MSG_DEVICE_IDLE_MODE_CHANGED, policyEvent);
        return;
    }

    if (action == COMMON_EVENT_UID_REMOVED) {
        uint32_t deletedUid = CommonUtils::StrToUint(eventData.GetWant().GetStringParam(EVENT_PARAM_DELETED_UID));
        auto policyEvent = std::make_shared<PolicyEvent>();
        policyEvent->deletedUid = deletedUid;
        SendEvent(NetPolicyEventHandler::MSG_UID_REMOVED, policyEvent);
        return;
    }
}
} // namespace NetManagerStandard
} // namespace OHOS
