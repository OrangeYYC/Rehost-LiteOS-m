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

#include "netsys_policy_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
NetsysPolicyWrapper::NetsysPolicyWrapper() = default;

NetsysPolicyWrapper::~NetsysPolicyWrapper() = default;

int32_t NetsysPolicyWrapper::RegisterNetsysCallback(sptr<NetsysControllerCallback> callback)
{
    auto netsysReturnValue = NetsysController::GetInstance().RegisterCallback(callback);
    NETMGR_LOG_D("NetsysPolicyWrapper:RegisterNetsysObserver netsys return[%{public}d]", netsysReturnValue);
    return netsysReturnValue;
}

int32_t NetsysPolicyWrapper::BandwidthEnableDataSaver(bool enable)
{
    auto netsysReturnValue = NetsysController::GetInstance().BandwidthEnableDataSaver(enable);
    NETMGR_LOG_D("NetsysPolicyWrapper:EnableDataSaver enable[%{public}d] netsys return[%{public}d]", enable,
                 netsysReturnValue);
    return netsysReturnValue;
}

int32_t NetsysPolicyWrapper::BandwidthSetIfaceQuota(const std::string &iface, int64_t bytes)
{
    auto netsysReturnValue = NetsysController::GetInstance().BandwidthSetIfaceQuota(iface, bytes);
    NETMGR_LOG_D("NetsysPolicyWrapper:SetIfaceQuota iface[%{public}s] bytes[%{public}s] netsys return[%{public}d]",
                 iface.c_str(), std::to_string(bytes).c_str(), netsysReturnValue);
    return netsysReturnValue;
}

int32_t NetsysPolicyWrapper::BandwidthRemoveIfaceQuota(const std::string &iface)
{
    auto netsysReturnValue = NetsysController::GetInstance().BandwidthRemoveIfaceQuota(iface);
    NETMGR_LOG_D("NetsysPolicyWrapper:RemoveIfaceQuota iface[%{public}s] netsys return[%{public}d]", iface.c_str(),
                 netsysReturnValue);
    return netsysReturnValue;
}

int32_t NetsysPolicyWrapper::BandwidthAddDeniedList(uint32_t uid)
{
    auto netsysReturnValue = NetsysController::GetInstance().BandwidthAddDeniedList(uid);
    NETMGR_LOG_D("NetsysPolicyWrapper:AddDeniedList uid[%{public}u] netsys return[%{public}d]", uid, netsysReturnValue);
    return netsysReturnValue;
}

int32_t NetsysPolicyWrapper::BandwidthRemoveDeniedList(uint32_t uid)
{
    auto netsysReturnValue = NetsysController::GetInstance().BandwidthRemoveDeniedList(uid);
    NETMGR_LOG_D("NetsysPolicyWrapper:RemoveDeniedList uid[%{public}u] netsys return[%{public}d]", uid,
                 netsysReturnValue);
    return netsysReturnValue;
}

int32_t NetsysPolicyWrapper::BandwidthAddAllowedList(uint32_t uid)
{
    auto netsysReturnValue = NetsysController::GetInstance().BandwidthAddAllowedList(uid);
    NETMGR_LOG_D("NetsysPolicyWrapper:AddAllowedList uid[%{public}u] netsys return[%{public}d]", uid,
                 netsysReturnValue);
    return netsysReturnValue;
}

int32_t NetsysPolicyWrapper::BandwidthRemoveAllowedList(uint32_t uid)
{
    auto netsysReturnValue = NetsysController::GetInstance().BandwidthRemoveAllowedList(uid);
    NETMGR_LOG_D("NetsysPolicyWrapper:RemoveAllowedLists uid[%{public}u] netsys return[%{public}d]", uid,
                 netsysReturnValue);
    return netsysReturnValue;
}

int32_t NetsysPolicyWrapper::FirewallSetUidsAllowedListChain(uint32_t chain, const std::vector<uint32_t> &uids)
{
    auto netsysReturnValue = NetsysController::GetInstance().FirewallSetUidsAllowedListChain(chain, uids);
    NETMGR_LOG_D("SetUidsAllowedListChain chain[%{public}u] uids size[%{public}zu] netsys return[%{public}d]", chain,
                 uids.size(), netsysReturnValue);
    return netsysReturnValue;
}

int32_t NetsysPolicyWrapper::FirewallSetUidsDeniedListChain(uint32_t chain, const std::vector<uint32_t> &uids)
{
    auto netsysReturnValue = NetsysController::GetInstance().FirewallSetUidsDeniedListChain(chain, uids);
    NETMGR_LOG_D("SetUidsDeniedListChain chain[%{public}u] uids size[%{public}zu] netsys return[%{public}d]", chain,
                 uids.size(), netsysReturnValue);
    return netsysReturnValue;
}

int32_t NetsysPolicyWrapper::FirewallSetUidRule(uint32_t chain, uint32_t uid, uint32_t firewallRule)
{
    auto netsysReturnValue = NetsysController::GetInstance().FirewallSetUidRule(chain, uid, firewallRule);
    NETMGR_LOG_D(
        "NetsysPolicyWrapper:FirewallSetUidRule chain[%{public}u] uid[%{public}u] firewallRule[%{public}u] netsys "
        "return[%{public}d]",
        chain, uid, firewallRule, netsysReturnValue);
    return netsysReturnValue;
}

int32_t NetsysPolicyWrapper::FirewallEnableChain(uint32_t chain, bool enable)
{
    auto netsysReturnValue = NetsysController::GetInstance().FirewallEnableChain(chain, enable);
    NETMGR_LOG_D("FirewallEnableChain chain[%{public}u] enable[%{public}d] netsys return[%{public}d]", chain, enable,
                 netsysReturnValue);
    return netsysReturnValue;
}
} // namespace NetManagerStandard
} // namespace OHOS
