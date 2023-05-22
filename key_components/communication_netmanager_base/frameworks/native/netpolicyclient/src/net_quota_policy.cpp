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

#include "net_quota_policy.h"

#include <ctime>

#include "parcel.h"

#include "net_mgr_log_wrapper.h"
#include "netmanager_base_common_utils.h"

namespace OHOS {
namespace NetManagerStandard {
static constexpr uint32_t MAX_POLICY_SIZE = 100;

bool NetQuotaPolicy::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteInt32(netType)) {
        return false;
    }
    if (!parcel.WriteString(iccid)) {
        return false;
    }
    if (!parcel.WriteInt64(periodStartTime)) {
        return false;
    }
    if (!parcel.WriteString(periodDuration)) {
        return false;
    }
    if (!parcel.WriteInt64(warningBytes)) {
        return false;
    }
    if (!parcel.WriteInt64(limitBytes)) {
        return false;
    }
    if (!parcel.WriteInt64(lastLimitRemind)) {
        return false;
    }
    if (!parcel.WriteBool(metered)) {
        return false;
    }
    if (!parcel.WriteInt32(source)) {
        return false;
    }
    if (!parcel.WriteString(ident)) {
        return false;
    }

    return true;
}

bool NetQuotaPolicy::Marshalling(Parcel &parcel, const NetQuotaPolicy &quotaPolicy)
{
    quotaPolicy.Marshalling(parcel);
    return true;
}

bool NetQuotaPolicy::Marshalling(Parcel &parcel, const std::vector<NetQuotaPolicy> &quotaPolicies)
{
    uint32_t vsize = static_cast<uint32_t>(quotaPolicies.size());
    if (!parcel.WriteUint32(vsize)) {
        return false;
    }

    for (uint32_t i = 0; i < vsize; ++i) {
        quotaPolicies[i].Marshalling(parcel);
    }

    return true;
}

bool NetQuotaPolicy::Unmarshalling(Parcel &parcel, NetQuotaPolicy &quotaPolicy)
{
    if (!parcel.ReadInt32(quotaPolicy.netType)) {
        return false;
    }
    if (!parcel.ReadString(quotaPolicy.iccid)) {
        return false;
    }
    if (!parcel.ReadInt64(quotaPolicy.periodStartTime)) {
        return false;
    }
    if (!parcel.ReadString(quotaPolicy.periodDuration)) {
        return false;
    }
    if (!parcel.ReadInt64(quotaPolicy.warningBytes)) {
        return false;
    }
    if (!parcel.ReadInt64(quotaPolicy.limitBytes)) {
        return false;
    }
    if (!parcel.ReadInt64(quotaPolicy.lastLimitRemind)) {
        return false;
    }
    if (!parcel.ReadBool(quotaPolicy.metered)) {
        return false;
    }
    if (!parcel.ReadInt32(quotaPolicy.source)) {
        return false;
    }
    if (!parcel.ReadString(quotaPolicy.ident)) {
        return false;
    }

    return true;
}

bool NetQuotaPolicy::Unmarshalling(Parcel &parcel, std::vector<NetQuotaPolicy> &quotaPolicies)
{
    uint32_t vSize = 0;
    if (!parcel.ReadUint32(vSize)) {
        return false;
    }
    vSize = vSize > MAX_POLICY_SIZE ? MAX_POLICY_SIZE : vSize;

    NetQuotaPolicy quotaPolicyTmp;
    for (uint32_t i = 0; i < vSize; i++) {
        if (!parcel.ReadInt32(quotaPolicyTmp.netType)) {
            return false;
        }
        if (!parcel.ReadString(quotaPolicyTmp.iccid)) {
            return false;
        }
        if (!parcel.ReadInt64(quotaPolicyTmp.periodStartTime)) {
            return false;
        }
        if (!parcel.ReadString(quotaPolicyTmp.periodDuration)) {
            return false;
        }
        if (!parcel.ReadInt64(quotaPolicyTmp.warningBytes)) {
            return false;
        }
        if (!parcel.ReadInt64(quotaPolicyTmp.limitBytes)) {
            return false;
        }
        if (!parcel.ReadInt64(quotaPolicyTmp.lastLimitRemind)) {
            return false;
        }
        if (!parcel.ReadBool(quotaPolicyTmp.metered)) {
            return false;
        }
        if (!parcel.ReadInt32(quotaPolicyTmp.source)) {
            return false;
        }
        if (!parcel.ReadString(quotaPolicyTmp.ident)) {
            return false;
        }
        quotaPolicies.push_back(quotaPolicyTmp);
    }

    return true;
}

bool NetQuotaPolicy::IsOverWarning(int64_t totalQuota) const
{
    return totalQuota > warningBytes;
}

bool NetQuotaPolicy::IsOverLimit(int64_t totalQuota) const
{
    return totalQuota > limitBytes;
}

int64_t NetQuotaPolicy::GetPeriodStart()
{
    if (periodDuration.size() < PERIOD_DURATION_SIZE) {
        periodDuration = PERIOD_MONTH;
    }
    time_t timeNow;
    time(&timeNow);
    struct tm tm;
    localtime_r(&timeNow, &tm);
    std::string cycle = periodDuration.substr(0, 1);
    int32_t start = CommonUtils::StrToInt(periodDuration.substr(1, periodDuration.size()));

    if (cycle == PERIOD_DAY) {
        tm.tm_hour = start;
        tm.tm_min = 0;
        tm.tm_sec = 0;
    } else if (cycle == PERIOD_YEAR) {
        tm.tm_hour = 0;
        tm.tm_min = 0;
        tm.tm_sec = 0;
        tm.tm_yday = start - 1;
    } else {
        tm.tm_hour = 0;
        tm.tm_min = 0;
        tm.tm_sec = 0;
        tm.tm_mday = start;
    }
    time_t start_time = mktime(&tm);
    return start_time;
}

void NetQuotaPolicy::Reset()
{
    periodDuration = PERIOD_MONTH + std::to_string(PERIOD_START);
    warningBytes = DATA_USAGE_UNKNOWN;
    limitBytes = DATA_USAGE_UNKNOWN;
    lastWarningRemind = REMIND_NEVER;
    lastLimitRemind = REMIND_NEVER;
    metered = false;
    limitAction = LimitAction::LIMIT_ACTION_NONE;
}
} // namespace NetManagerStandard
} // namespace OHOS
