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

#ifndef COMMUNICATIONNETMANAGERBASE_POLICY_CONSTANT_H
#define COMMUNICATIONNETMANAGERBASE_POLICY_CONSTANT_H

namespace OHOS {
namespace NetManagerStandard {
constexpr const char *EVENT_POLICY_UID_POLICY = "netUidPolicyChange";
constexpr const char *EVENT_POLICY_UID_RULE = "netUidRuleChange";
constexpr const char *EVENT_POLICY_METERED_IFACES = "netMeteredIfacesChange";
constexpr const char *EVENT_POLICY_QUOTA_POLICY = "netQuotaPolicyChange";
constexpr const char *EVENT_POLICY_BACKGROUND_POLICY = "netBackgroundPolicyChange";
constexpr const char *EVENT_POLICY_STRATEGY_SWITCH = "netStrategySwitch";

constexpr const char *KEY_POLICY = "policy";
constexpr const char *KEY_UID = "uid";
constexpr const char *KEY_RULE = "rule";
constexpr const char *KEY_ICCID = "iccid";
constexpr const char *KEY_ENABLE = "enable";

constexpr int ARRAY_LIMIT = 64;

} // namespace NetManagerStandard
} // namespace OHOS
#endif // COMMUNICATIONNETMANAGERBASE_POLICY_CONSTANT_H
