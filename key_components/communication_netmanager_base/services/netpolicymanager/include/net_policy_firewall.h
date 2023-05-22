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

#ifndef NET_POLICY_FIREWALL_H
#define NET_POLICY_FIREWALL_H

#include "firewall_rule.h"
#include "net_policy_base.h"
#include "net_policy_file.h"

namespace OHOS {
namespace NetManagerStandard {
class NetPolicyFirewall : public NetPolicyBase {
public:
    NetPolicyFirewall() : deviceIdleMode_(false) {}
    void Init();

    /**
     * Set the UID into device idle allow list.
     *
     * @param uid The specified UID of application.
     * @param isAllowed The UID is into allow list or not.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t SetDeviceIdleAllowedList(uint32_t uid, bool isAllowed);

    /**
     * Get the allow list of UID in device idle mode.
     *
     * @param uids The list of UIDs
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t GetDeviceIdleAllowedList(std::vector<uint32_t> &uids);

    /**
     * Process network policy in device idle mode.
     *
     * @param enable Device idle mode is open or not.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t UpdateDeviceIdlePolicy(bool enable);

    /**
     * Reset network firewall rules.
     *
     */
    void ResetPolicies();

    /**
     * Handle the event from NetPolicyCore
     *
     * @param eventId The event id
     * @param policyEvent The informations passed from other core
     */
    void HandleEvent(int32_t eventId, const std::shared_ptr<PolicyEvent> &policyEvent);

private:
    void DeleteUid(uint32_t uid);
    std::shared_ptr<FirewallRule> deviceIdleFirewallRule_;
    bool deviceIdleMode_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_POLICY_FIREWALL_H
