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

#ifndef NET_POLICY_TRAFFIC_H
#define NET_POLICY_TRAFFIC_H

#include "singleton.h"
#include "system_ability.h"

#include "net_policy_base.h"
#include "net_policy_callback.h"
#include "net_policy_service_stub.h"
#include "netsys_controller_callback.h"

namespace OHOS {
namespace NetManagerStandard {
constexpr int16_t NET_POLICY_LEAP_YEAR_ONE = 1;
constexpr int16_t NET_POLICY_LEAP_YEAR_FOUR = 4;
constexpr int16_t NET_POLICY_LEAP_YEAR_ONEHUNDRED = 100;
constexpr int16_t NET_POLICY_LEAP_YEAR_FOURHUNDRED = 400;
constexpr int16_t NET_POLICY_FEBRUARY = 1;
constexpr int32_t NET_POLICY_ONEDAYTIME = 86400;
constexpr int16_t MONTH_TWENTY_EIGHT = 28;
constexpr int16_t MONTH_THIRTY = 30;
constexpr int16_t MONTH_THIRTY_ONE = 31;
constexpr int32_t NINETY_PERCENTAGE = 90;
constexpr int32_t HUNDRED_PERCENTAGE = 100;
class NetPolicyTraffic : public NetPolicyBase {
public:
    void Init();

    /**
     * Update quota policies.
     *
     * @param quotaPolicies The updated quota policies
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}
     */
    int32_t UpdateQuotaPolicies(const std::vector<NetQuotaPolicy> &quotaPolicies);

    /**
     * Get network policies.
     *
     * @param quotaPolicies The list of network quota policy, {@link NetQuotaPolicy}.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t GetNetQuotaPolicies(std::vector<NetQuotaPolicy> &quotaPolicies);

    /**
     * Update the limit or warning remind time of quota policy.
     *
     * @param netType {@link NetBearType}.
     * @param iccid Specify the matched iccid of quota policy when netType is cellular.
     * @param remindType {@link RemindType}.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t UpdateRemindPolicy(int32_t netType, const std::string &iccid, uint32_t remindType);

    /**
     * Handle the event from NetPolicyCore
     *
     * @param eventId The event id
     * @param policyEvent The informations passed from other core
     */
    void HandleEvent(int32_t eventId, const std::shared_ptr<PolicyEvent> &policyEvent);

    /**
     * Get the metered ifaces.
     *
     * @return const std::vector<std::string>& The vector of metered ifaces
     */
    const std::vector<std::string> &GetMeteredIfaces();

    /**
     * Reset network policies\rules\quota policies\firewall rules.
     *
     * @param iccid Specify the matched iccid of quota policy.
     */
    int32_t ResetPolicies(const std::string &iccid);

    void ReachedLimit(const std::string &iface);
    void GetDumpMessage(std::string &message);

private:
    class NetsysControllerCallbackImpl : public NetsysControllerCallback {
    public:
        NetsysControllerCallbackImpl(std::shared_ptr<NetPolicyTraffic> traffic)
        {
            traffic_ = traffic;
        }
        virtual int32_t OnInterfaceAddressUpdated(const std::string &, const std::string &, int, int)
        {
            return 0;
        }
        virtual int32_t OnInterfaceAddressRemoved(const std::string &, const std::string &, int, int)
        {
            return 0;
        }
        virtual int32_t OnInterfaceAdded(const std::string &)
        {
            return 0;
        }
        virtual int32_t OnInterfaceRemoved(const std::string &)
        {
            return 0;
        }
        virtual int32_t OnInterfaceChanged(const std::string &, bool)
        {
            return 0;
        }
        virtual int32_t OnInterfaceLinkStateChanged(const std::string &, bool)
        {
            return 0;
        }
        virtual int32_t OnRouteChanged(bool, const std::string &, const std::string &, const std::string &)
        {
            return 0;
        }
        virtual int32_t OnDhcpSuccess(NetsysControllerCallback::DhcpResult &dhcpResult)
        {
            return 0;
        }
        virtual int32_t OnBandwidthReachedLimit(const std::string &limitName, const std::string &iface)
        {
            traffic_->ReachedLimit(iface);
            return 0;
        }

    private:
        std::shared_ptr<NetPolicyTraffic> traffic_ = nullptr;
    };

    int32_t UpdateQuotaPoliciesInner();
    int64_t GetQuotaRemain(NetQuotaPolicy &quotaPolicy);
    void UpdateQuotaNotify();
    void UpdateMeteredIfaces(std::vector<std::string> &newMeteredIfaces);
    void UpdateNetEnableStatus(const NetQuotaPolicy &quotaPolicy);
    void FormalizeQuotaPolicies(const std::vector<NetQuotaPolicy> &quotaPolicies);
    const std::vector<std::string> UpdateMeteredIfacesQuota();

    bool IsValidQuotaPolicy(const NetQuotaPolicy &quotaPolicy);
    int64_t GetTotalQuota(NetQuotaPolicy &quotaPolicy);
    void SetNetworkEnableStatus(const NetQuotaPolicy &quotaPolicy, bool enable);
    void NotifyQuotaWarning(int64_t totalQuota);
    void NotifyQuotaLimit(int64_t totalQuota);
    void NotifyQuotaLimitReminded(int64_t totalQuota);
    void PublishQuotaEvent(const std::string &action, const std::string &describe, int64_t quota);
    int32_t ReadQuotaPolicies();
    bool WriteQuotaPolicies();
    const std::string GetMatchIfaces(const NetQuotaPolicy &quotaPolicy);

    bool IsValidNetType(int32_t netType);
    bool IsValidPeriodDuration(const std::string &periodDuration);
    bool IsQuotaPolicyExist(int32_t netType, const std::string &iccid);
    bool IsValidNetRemindType(uint32_t remindType);

private:
    std::vector<uint32_t> idleAllowedList_;
    std::vector<NetQuotaPolicy> quotaPolicies_;
    std::vector<std::string> meteredIfaces_;
    sptr<NetsysControllerCallback> netsysCallback_ = nullptr;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_POLICY_TRAFFIC_H
