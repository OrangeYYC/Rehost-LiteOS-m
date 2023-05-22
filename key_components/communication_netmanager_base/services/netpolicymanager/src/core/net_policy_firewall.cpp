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

#include "net_policy_firewall.h"

#include "ipc_skeleton.h"

#include "firewall_rule.h"
#include "net_policy_core.h"
#include "net_policy_event_handler.h"
#include "net_settings.h"

namespace OHOS {
namespace NetManagerStandard {
void NetPolicyFirewall::Init()
{
    deviceIdleFirewallRule_ = FirewallRule::CreateFirewallRule(FIREWALL_CHAIN_DEVICE_IDLE);
}

int32_t NetPolicyFirewall::SetDeviceIdleAllowedList(uint32_t uid, bool isAllowed)
{
    deviceIdleFirewallRule_->SetAllowedList(uid, isAllowed ? FIREWALL_RULE_ALLOW : FIREWALL_RULE_DENY);

    std::shared_ptr<PolicyEvent> eventData = std::make_shared<PolicyEvent>();
    eventData->eventId = NetPolicyEventHandler::MSG_DEVICE_IDLE_LIST_UPDATED;
    eventData->deviceIdleList = deviceIdleFirewallRule_->GetAllowedList();
    SendEvent(NetPolicyEventHandler::MSG_DEVICE_IDLE_LIST_UPDATED, eventData);
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyFirewall::GetDeviceIdleAllowedList(std::vector<uint32_t> &uids)
{
    uids = deviceIdleFirewallRule_->GetAllowedList();
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyFirewall::UpdateDeviceIdlePolicy(bool enable)
{
    if (deviceIdleMode_ == enable) {
        NETMGR_LOG_W("Same device idle policy.");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    if (enable) {
        deviceIdleFirewallRule_->SetAllowedList();
    }
    NetmanagerHiTrace::NetmanagerStartSyncTrace("Update firewall status start");
    deviceIdleFirewallRule_->EnableFirewall(enable);
    NetmanagerHiTrace::NetmanagerFinishSyncTrace("Update firewall status end");
    deviceIdleMode_ = enable;
    // notify to other core.
    auto policyEvent = std::make_shared<PolicyEvent>();
    policyEvent->deviceIdleMode = enable;
    NetmanagerHiTrace::NetmanagerStartSyncTrace("Notify other policy class status start");
    SendEvent(NetPolicyEventHandler::MSG_DEVICE_IDLE_MODE_CHANGED, policyEvent);
    NetmanagerHiTrace::NetmanagerFinishSyncTrace("Notify other policy class status end");
    return NETMANAGER_SUCCESS;
}

void NetPolicyFirewall::ResetPolicies()
{
    deviceIdleFirewallRule_->ClearAllowedList();
    deviceIdleFirewallRule_->ClearDeniedList();
    UpdateDeviceIdlePolicy(false);
}

void NetPolicyFirewall::DeleteUid(uint32_t uid)
{
    deviceIdleFirewallRule_->RemoveFromAllowedList(uid);
}

void NetPolicyFirewall::HandleEvent(int32_t eventId, const std::shared_ptr<PolicyEvent> &policyEvent)
{
    switch (eventId) {
        case NetPolicyEventHandler::MSG_UID_REMOVED:
            DeleteUid(policyEvent->deletedUid);
            break;
        default:
            break;
    }
}
} // namespace NetManagerStandard
} // namespace OHOS
