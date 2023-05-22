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

#ifndef NET_POLICY_CALLBACK_H
#define NET_POLICY_CALLBACK_H

#include <string>
#include <vector>

#include "singleton.h"

#include "i_net_policy_callback.h"
#include "net_quota_policy.h"

namespace OHOS {
namespace NetManagerStandard {
class NetPolicyCallback : public std::enable_shared_from_this<NetPolicyCallback> {
    DECLARE_DELAYED_SINGLETON(NetPolicyCallback);

public:
    /**
     * Register net policy callback.
     * @param callback Interface type pointer.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t RegisterNetPolicyCallback(const sptr<INetPolicyCallback> &callback);

    /**
     * Unregister net policy callback.
     * @param callback Interface type pointer.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t UnregisterNetPolicyCallback(const sptr<INetPolicyCallback> &callback);

    /**
     * Notify this uid's policy is changed.
     * @param uid The UID of application.
     * @param policy See {@link NetUidPolicy}.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t NotifyNetUidPolicyChange(uint32_t uid, uint32_t policy);

    /**
     * Notify this uid's rule is changed.
     * @param uid The UID of application.
     * @param rule See {@link NetUidRule}.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t NotifyNetUidRuleChange(uint32_t uid, uint32_t rule);

    /**
     * Notify the quota policy is changed.
     * @param quotaPolicies The struct vector of quotaPolicies.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t NotifyNetQuotaPolicyChange(const std::vector<NetQuotaPolicy> &quotaPolicies);

    /**
     * Notify when metered ifaces is changed.
     * @param ifaces The string vector of ifaces.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t NotifyNetMeteredIfacesChange(std::vector<std::string> &ifaces);

    /**
     * Notify when background policy is changed.
     * @param isAllow When isAllow is true,it means background policy is true,
     * when isAllow is false,it means background policy is false.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t NotifyNetBackgroundPolicyChange(bool isAllowed);

private:
    std::vector<sptr<INetPolicyCallback>> callbacks_;
    std::mutex callbackMutex_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_POLICY_CALLBACK_H