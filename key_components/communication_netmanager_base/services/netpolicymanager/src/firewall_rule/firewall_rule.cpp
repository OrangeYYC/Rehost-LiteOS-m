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

#include "firewall_rule.h"

#include "device_idle_firewall_rule.h"
#include "net_policy_inner_define.h"

namespace OHOS {
namespace NetManagerStandard {
std::shared_ptr<FirewallRule> FirewallRule::CreateFirewallRule(uint32_t chain)
{
    switch (chain) {
        case FIREWALL_CHAIN_DEVICE_IDLE:
            return DelayedSingleton<DeviceIdleFirewallRule>::GetInstance();
        default:
            break;
    }
    return nullptr;
}

FirewallRule::FirewallRule(uint32_t chainType)
{
    chainType_ = chainType;
    netsys_ = DelayedSingleton<NetsysPolicyWrapper>::GetInstance();
}

FirewallRule::~FirewallRule() = default;

const std::vector<uint32_t> &FirewallRule::GetAllowedList() const
{
    return allowedList_;
}

void FirewallRule::SetAllowedList(uint32_t uid, uint32_t rule)
{
    if (rule == FIREWALL_RULE_ALLOW) {
        allowedList_.emplace_back(uid);
    } else {
        for (auto iter = allowedList_.begin(); iter != allowedList_.end();) {
            if (uid == *iter) {
                allowedList_.erase(iter);
                break;
            } else {
                iter++;
            }
        }
    }
    netsys_->FirewallSetUidRule(chainType_, uid, rule);
}

void FirewallRule::SetAllowedList(const std::vector<uint32_t> &uids)
{
    for (const auto &it : uids) {
        if (std::find(allowedList_.begin(), allowedList_.end(), it) == allowedList_.end()) {
            allowedList_.push_back(it);
        }
    }

    SetAllowedList();
}

void FirewallRule::SetAllowedList()
{
    NetmanagerHiTrace::NetmanagerStartSyncTrace("Set allowed list start");
    netsys_->FirewallSetUidsAllowedListChain(chainType_, allowedList_);
    NetmanagerHiTrace::NetmanagerFinishSyncTrace("Set allowed list end");
}

void FirewallRule::ClearAllowedList()
{
    allowedList_.clear();
    netsys_->FirewallSetUidsAllowedListChain(chainType_, allowedList_);
}

const std::vector<uint32_t> &FirewallRule::GetDeniedList() const
{
    return deniedList_;
}

void FirewallRule::SetDeniedList(uint32_t uid, uint32_t rule)
{
    if (rule == FIREWALL_RULE_DENY) {
        deniedList_.emplace_back(uid);
    } else {
        for (auto iter = deniedList_.begin(); iter != deniedList_.end();) {
            if (uid == *iter) {
                iter = deniedList_.erase(iter);
            } else {
                iter++;
            }
        }
    }
    netsys_->FirewallSetUidRule(chainType_, uid, rule);
}

void FirewallRule::SetDeniedList(const std::vector<uint32_t> &uids)
{
    for (const auto &it : uids) {
        if (std::find(deniedList_.begin(), deniedList_.end(), it) == deniedList_.end()) {
            deniedList_.push_back(it);
        }
    }

    SetDeniedList();
}

void FirewallRule::SetDeniedList()
{
    netsys_->FirewallSetUidsDeniedListChain(chainType_, deniedList_);
}

void FirewallRule::ClearDeniedList()
{
    deniedList_.clear();
    netsys_->FirewallSetUidsAllowedListChain(chainType_, deniedList_);
}

void FirewallRule::SetUidFirewallRule(uint uid, bool isAllowed)
{
    netsys_->FirewallSetUidRule(chainType_, uid, isAllowed ? FIREWALL_RULE_ALLOW : FIREWALL_RULE_DENY);
}

void FirewallRule::EnableFirewall(bool enable)
{
    netsys_->FirewallEnableChain(chainType_, enable);
}

void FirewallRule::RemoveFromAllowedList(uint32_t uid)
{
    for (auto iter = allowedList_.begin(); iter != allowedList_.end(); ++iter) {
        if (*iter == uid) {
            allowedList_.erase(iter);
            break;
        }
    }
    netsys_->FirewallSetUidsAllowedListChain(chainType_, allowedList_);
}

void FirewallRule::RemoveFromDeniedList(uint32_t uid)
{
    for (auto iter = deniedList_.begin(); iter != deniedList_.end(); ++iter) {
        if (*iter == uid) {
            deniedList_.erase(iter);
            break;
        }
    }
    netsys_->FirewallSetUidsDeniedListChain(chainType_, deniedList_);
}
} // namespace NetManagerStandard
} // namespace OHOS
