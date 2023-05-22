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

#ifndef FIREWALL_RULE_H
#define FIREWALL_RULE_H

#include <string>
#include <vector>

#include "refbase.h"

#include "netmanager_hitrace.h"
#include "netsys_policy_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
class FirewallRule : public std::enable_shared_from_this<FirewallRule> {
public:
    /**
     * Creare firewall rule
     *
     * @param chain The chain type
     * @return std::shared_ptr<FirewallRule> The firewall rule, such as DeviceIdleFirewallRule
     */
    static std::shared_ptr<FirewallRule> CreateFirewallRule(uint32_t chain);

    /**
     * Get the firewall allow list.
     *
     * @return const std::vector<uint32_t>& The firewall allow list
     */
    virtual const std::vector<uint32_t> &GetAllowedList() const;

    /**
     * Set the firewall allow list
     *
     * @param uid The UID of application
     * @param rule The firewall rull, see {@link FIREWALL_RULE_ALLOW and FIREWALL_RULE_DENY}
     */
    virtual void SetAllowedList(uint32_t uid, uint32_t rule);

    /**
     * Set the firewall allow list
     *
     * @param uids The vector of UID
     */
    virtual void SetAllowedList(const std::vector<uint32_t> &uids);

    /**
     * Set the firewall allow list
     *
     */
    virtual void SetAllowedList();

    /**
     * Get the firewall reject list
     *
     * @return const std::vector<uint32_t>& The firewall reject list
     */
    virtual const std::vector<uint32_t> &GetDeniedList() const;

    /**
     * Set the firewall reject list
     *
     * @param uid The UID of application
     * @param rule The firewall rull, see {@link FIREWALL_RULE_ALLOW} and {@link FIREWALL_RULE_DENY}
     */
    virtual void SetDeniedList(uint32_t uid, uint32_t rule);

    /**
     * Set the firewall reject list
     *
     * @param uids The vector of UID
     */
    virtual void SetDeniedList(const std::vector<uint32_t> &uids);

    /**
     * Set the firewall reject list
     *
     */
    virtual void SetDeniedList();

    /**
     * Clear the firewall allow list
     *
     */
    void ClearAllowedList();

    /**
     * Clear the firewall reject list
     *
     */
    void ClearDeniedList();

    /**
     * Set the firewall rule for the specified UID
     *
     * @param uid The UID of application
     * @param isAllow allow the firewall rule or not
     */
    virtual void SetUidFirewallRule(uint32_t uid, bool isAllow);

    /**
     * Enable the firewall rule
     *
     * @param enable true: enable the firewall rule; false: disable the firewall rule
     */
    virtual void EnableFirewall(bool enable);

    /**
     * Remove the UID from the firewall allow list
     *
     * @param uid The UID of application
     */
    virtual void RemoveFromAllowedList(uint32_t uid);

    /**
     * Remove the UID from the firewall reject list
     *
     * @param uid The UID of application
     */
    virtual void RemoveFromDeniedList(uint32_t uid);

protected:
    explicit FirewallRule(uint32_t chainType);
    virtual ~FirewallRule();

protected:
    uint32_t chainType_ = 0;
    std::string chainName_;
    std::vector<uint32_t> allowedList_;
    std::vector<uint32_t> deniedList_;
    bool modeEnable_ = false;

private:
    std::shared_ptr<NetsysPolicyWrapper> netsys_ = nullptr;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // FIREWALL_RULE_H
