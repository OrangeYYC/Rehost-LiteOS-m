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

#ifndef NET_POLICY_RULE_H
#define NET_POLICY_RULE_H

#include <map>

#include "net_policy_base.h"
#include "netmanager_base_common_utils.h"

namespace OHOS {
namespace NetManagerStandard {
constexpr uint32_t POLICY_TRANS_CONDITION_MASK = 0b11111111110000000000;
constexpr uint32_t POLICY_TRANS_RULE_MASK = 0b00000000001111111000;
constexpr uint32_t POLICY_TRANS_NET_CTRL_MASK = 0b00000000000000000111;
constexpr u_int8_t CONDITION_START_BIT = 10;
constexpr u_int8_t RULE_START_BIT = 3;

enum PolicyTransCtrl {
    POLICY_TRANS_CTRL_NONE = 0b000,
    POLICY_TRANS_CTRL_ADD_DENIEDLIST = 0b010,
    POLICY_TRANS_CTRL_ADD_ALLOWEDLIST = 0b100,
    POLICY_TRANS_CTRL_REMOVE_ALL = 0b001,
};

struct UidPolicyRule {
    uint32_t policy_ = 0;
    uint32_t rule_ = 1 << 7;
    uint32_t netsys_ = 7;
};

class NetPolicyRule : public NetPolicyBase {
public:
    NetPolicyRule();
    void Init();
    void HandleEvent(int32_t eventId, const std::shared_ptr<PolicyEvent> &policyEvent);

    /**
     * Transform policy to rule and netsys-control.
     *
     * @param uid The UID of application.
     * @param policy See {@link NetUidPolicy}.
     * @return Returns 0 success. Otherwise fail.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t TransPolicyToRule(uint32_t uid, uint32_t policy);

    /**
     * Get the status whether the specified uid app can access the metered network or non-metered network.
     *
     * @param uid The specified UID of application.
     * @param metered Indicates meterd network or non-metered network.
     * @param isAllowed Return true means it's allowed to access the network.
     *      Return false means it's not allowed to access the network.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t IsUidNetAllowed(uint32_t uid, bool metered, bool &isAllowed);

    /**
     * Get the network policy of the specified UID.
     *
     * @param uid The specified UID of app.
     * @param policy Return this uid's policy.
     *      For details, see {@link NetUidPolicy}.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetPolicyByUid(uint32_t uid, uint32_t &policy);

    /**
     * Get the application UIDs of the specified policy.
     *
     * @param policy the network policy of the current UID of application.
     * @param uids The list of UIDs.
     *      For details, see {@link NetUidPolicy}.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetUidsByPolicy(uint32_t policy, std::vector<uint32_t> &uids);

    /**
     * Reset network policies and rules.
     *
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t ResetPolicies();

    /**
     * Control if apps can use data on background.
     *
     * @param allow Allow apps to use data on background.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t SetBackgroundPolicy(bool allow);

    /**
     * Get the background network restriction policy for the specified uid.
     *
     * @param uid The specified UID of application.
     * @param backgroundPolicyOfUid The specified UID of backgroundPolicy.
     *      For details, see {@link NetBackgroundPolicy}.
     * @return uint32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetBackgroundPolicyByUid(uint32_t uid, uint32_t &backgroundPolicyOfUid);

    /**
     * Get the status if apps can use data on background.
     * @param backgroundPolicy True is allowed to use data on background.
     *      False is not allowed to use data on background.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     * @permission ohos.permission.CONNECTIVITY_INTERNAL
     * @systemapi Hide this for inner system use.
     */
    int32_t GetBackgroundPolicy(bool &backgroundPolicy);

    /**
     * Get the Dump Message object.
     */
    void GetDumpMessage(std::string &message);

private:
    void NetsysCtrl(uint32_t uid, uint32_t netsysCtrl);
    void TransConditionToRuleAndNetsys(uint32_t policyCondition, uint32_t uid, uint32_t policy);
    uint32_t MoveToConditionBit(uint32_t value);
    uint32_t MoveToRuleBit(uint32_t value);
    uint32_t ChangePolicyToPolicyTransitionCondition(uint32_t policy);
    uint32_t BuildTransCondition(uint32_t uid, uint32_t policy);
    uint32_t GetMatchTransCondition(uint32_t policyCondition);

    // When system's mode status is changed, do this function.
    void TransPolicyToRule();
    // When a uid add into some forbidden list, do this function.
    void TransPolicyToRule(uint32_t uid);
    bool IsIdleMode();
    bool InIdleAllowedList(uint32_t uid);
    bool IsLimitByAdmin();
    bool IsForeground(uint32_t uid);
    bool IsPowerSave();
    bool InPowerSaveAllowedList(uint32_t uid);
    bool IsLimitedBackground();
    void DeleteUid(uint32_t uid);
    bool IsValidNetPolicy(uint32_t policy);

private:
    std::map<uint32_t, UidPolicyRule> uidPolicyRules_;
    bool backgroundAllow_ = true;
    bool deviceIdleMode_ = false;
    std::vector<uint32_t> deviceIdleAllowedList_;

private:
    /**
     * The map for transforming conditions to net rule and netsys control.
     *
     * Example:
     *  bit 11~20       bit 4~10      bit 1~3
     *  0000001100      0010000       110
     *  Condition       Rule          Netsys control
     *
     * Condition: see {@link enum PolicyTransCondition}
     * Rule: see {@link enum NetUidRule}
     * Netsys control: see {@link enum NetsysOperation}
     *
     * Transform Flow:
     *      1. According to the status of system(such as device idle mode or power save mode)
     *          and the net policy of uid, construct the Condition by bit operations.
     *      2. Find the matched Condition in this map.
     *      3. Get the rule bits and netsys-control bits from the matched Condition.
     *      4. Process the corresponding operations.
     */
    static inline const std::vector<uint32_t> POLICY_TRANS_MAP = {
        0b00011000000100000000, 0b10000000000000100010, 0b00010000001000000000, 0b00000010100000010100,
        0b00000011000000001100, 0b00000010010000001100, 0b01000000001000000000, 0b00000100100000010100,
        0b00000100010000100010, 0b00000000010000000001, 0b00000001000000100010, 0b00000000000000000001,
    };
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_POLICY_RULE_H
