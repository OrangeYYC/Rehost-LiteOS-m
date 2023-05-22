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

#ifndef NET_POLICY_QUOTA_POLICY_H
#define NET_POLICY_QUOTA_POLICY_H

#include <string>

#include "parcel.h"

#include "net_all_capabilities.h"
#include "net_policy_constants.h"

namespace OHOS {
namespace NetManagerStandard {
static constexpr const char *PERIOD_DAY = "D";
static constexpr const char *PERIOD_MONTH = "M";
static constexpr const char *PERIOD_YEAR = "Y";
static constexpr int32_t PERIOD_START = 1;
static constexpr int32_t DAY_MAX = 31;
static constexpr int32_t MONTH_MAX = 12;
static constexpr int32_t YEAR_MAX = 366;
static constexpr int32_t PERIOD_DURATION_SIZE = 2;
static constexpr int32_t QUOTA_POLICY_MAX_SIZE = 100;

struct NetQuotaPolicy : public Parcelable {
    /* See {@link NetBearType} */
    int32_t netType = BEARER_DEFAULT;
    /* The ID of the target card, valid when netType is BEARER_CELLULAR */
    std::string iccid;
    /* To specify the identity of network, such as different WLAN */
    std::string ident;
    // @deprecated
    int64_t periodStartTime = -1;
    /* The period and the start time for quota policy, default: "M1" */
    std::string periodDuration = (PERIOD_MONTH + std::to_string(PERIOD_START));
    // @deprecated
    std::string title;
    // @deprecated
    std::string summary;
    /* The warning threshold of traffic, default:  DATA_USAGE_UNKNOWN */
    int64_t warningBytes = DATA_USAGE_UNKNOWN;
    /* The limit threshold of traffic, default: DATA_USAGE_UNKNOWN */
    int64_t limitBytes = DATA_USAGE_UNKNOWN;
    /* The updated wall time that last warning remind, default: REMIND_NEVER */
    int64_t lastWarningRemind = REMIND_NEVER;
    /* The updated wall time that last limit remind, default: REMIND_NEVER */
    int64_t lastLimitRemind = REMIND_NEVER;
    /* Is metered network or not */
    bool metered = false;
    // @deprecated
    int32_t source = -1;
    /* The action while the used bytes reach the limit, see {@link LimitAction} */
    int32_t limitAction = LimitAction::LIMIT_ACTION_NONE;
    // @deprecated
    int64_t usedBytes = -1;
    // @deprecated
    int64_t usedTimeDuration = -1;
    // @deprecated
    std::string possessor;

    virtual bool Marshalling(Parcel &parcel) const override;
    static bool Marshalling(Parcel &parcel, const NetQuotaPolicy &quotaPolicy);
    static bool Marshalling(Parcel &parcel, const std::vector<NetQuotaPolicy> &quotaPolicies);
    static bool Unmarshalling(Parcel &parcel, NetQuotaPolicy &quotaPolicy);
    static bool Unmarshalling(Parcel &parcel, std::vector<NetQuotaPolicy> &quotaPolicies);
    /**
     * Get the period start, transform the periodDuration to wall time.
     *
     * @return int64_t The wall time.of the period start.
     */
    int64_t GetPeriodStart();

    /**
     * To judge the quota is over the warning threshold.
     *
     * @param totalQuota The total quota used.
     * @return true Over the warning threshold.
     * @return false
     */
    bool IsOverWarning(int64_t totalQuota) const;

    /**
     * To judge the quota is over the limit threshold.
     *
     * @param totalQuota The total quota used.
     * @return true Over the limit threshold.
     * @return false Not over.
     */
    bool IsOverLimit(int64_t totalQuota) const;

    /**
     * Reset the quota policy to default.
     *
     */
    void Reset();
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_POLICY_QUOTA_POLICY_H
