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

#ifndef NET_POLICY_CONSTANTS_H
#define NET_POLICY_CONSTANTS_H

#include <climits>

#include "net_manager_constants.h"

namespace OHOS {
namespace NetManagerStandard {
static constexpr int64_t DATA_USAGE_UNKNOWN = -1;
static constexpr int64_t DATA_USAGE_UNLIMITED = LONG_MAX;
static constexpr int64_t REMIND_NEVER = -1;
constexpr const char *QUOTA_POLICY_NO_PERIOD = "--";

enum NetPolicyResultCode {
    POLICY_ERR_INVALID_UID = 2104002,
    POLICY_ERR_INVALID_POLICY = 2104003,
    POLICY_ERR_INVALID_QUOTA_POLICY = 2104004,
    POLICY_ERR_QUOTA_POLICY_NOT_EXIST = 2104005,
};

enum NetUidPolicy {
    /* Default net policy. */
    NET_POLICY_NONE = 0,
    /* Reject on metered networks when app in background. */
    NET_POLICY_ALLOW_METERED_BACKGROUND = 1 << 0,
    /* Allow on metered networks when app in background. */
    NET_POLICY_REJECT_METERED_BACKGROUND = 1 << 1,
};

enum NetUidRule {
    /* Default uid rule */
    NET_RULE_NONE = 0,
    /* Allow traffic on metered networks while app is foreground. */
    NET_RULE_ALLOW_METERED_FOREGROUND = 1 << 0,
    /* Allow traffic on metered network. */
    NET_RULE_ALLOW_METERED = 1 << 1,
    /* Reject traffic on metered network. */
    NET_RULE_REJECT_METERED = 1 << 2,
    /* Allow traffic on all network (metered or non-metered). */
    NET_RULE_ALLOW_ALL = 1 << 5,
    /* Reject traffic on all network. */
    NET_RULE_REJECT_ALL = 1 << 6,
};

enum NetBackgroundPolicy {
    /* Default value. */
    NET_BACKGROUND_POLICY_NONE = 0,
    /* Apps can use metered networks on background. */
    NET_BACKGROUND_POLICY_ENABLE = 1,
    /* Apps can't use metered networks on background. */
    NET_BACKGROUND_POLICY_DISABLE = 2,
    /* Only apps in allowedlist can use metered networks on background. */
    NET_BACKGROUND_POLICY_ALLOWEDLIST = 3,
};

enum PolicyTransCondition {
    POLICY_TRANS_CONDITION_UID_POLICY_NONE = 1 << 0,
    POLICY_TRANS_CONDITION_ALLOW_METERED_BACKGROUND = 1 << 1,
    POLICY_TRANS_CONDITION_REJECT_METERED_BACKGROUND = 1 << 2,
    POLICY_TRANS_CONDITION_FOREGROUND = 1 << 3,
    POLICY_TRANS_CONDITION_BACKGROUND_RESTRICT = 1 << 4,
    POLICY_TRANS_CONDITION_IDLE_ALLOWEDLIST = 1 << 5,
    POLICY_TRANS_CONDITION_IDLE_MODE = 1 << 6,
    POLICY_TRANS_CONDITION_POWERSAVE_ALLOWEDLIST = 1 << 7,
    POLICY_TRANS_CONDITION_POWERSAVE_MODE = 1 << 8,
    POLICY_TRANS_CONDITION_ADMIN_RESTRICT = 1 << 9,
};

enum LimitAction {
    /* Default action, do nothing. */
    LIMIT_ACTION_NONE = -1,
    /* Access is disabled, when quota policy hit the limit */
    LIMIT_ACTION_DISABLE = 0,
    /* The user is billed automatically, when quota policy hit the limit */
    LIMIT_ACTION_AUTO_BILL = 1,
};

enum RemindType {
    /* Warning remind. */
    REMIND_TYPE_WARNING = 1,
    /* Limit remind. */
    REMIND_TYPE_LIMIT = 2,
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_POLICY_CONSTANTS_H
