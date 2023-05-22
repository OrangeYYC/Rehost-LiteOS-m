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

#ifndef I_NET_POLICY_SERVICE_H
#define I_NET_POLICY_SERVICE_H

#include "iremote_broker.h"

#include "i_net_policy_callback.h"
#include "net_policy_constants.h"
#include "net_quota_policy.h"

namespace OHOS {
namespace NetManagerStandard {
class INetPolicyService : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.NetManagerStandard.INetPolicyService");
    enum {
        CMD_NPS_START = 0,
        CMD_NPS_SET_POLICY_BY_UID,
        CMD_NPS_GET_POLICY_BY_UID,
        CMD_NPS_GET_UIDS_BY_POLICY,
        CMD_NPS_IS_NET_ALLOWED_BY_METERED,
        CMD_NPS_IS_NET_ALLOWED_BY_IFACE,
        CMD_NPS_REGISTER_NET_POLICY_CALLBACK,
        CMD_NPS_UNREGISTER_NET_POLICY_CALLBACK,
        CMD_NPS_SET_NET_QUOTA_POLICIES,
        CMD_NPS_GET_NET_QUOTA_POLICIES,
        CMD_NPS_UPDATE_REMIND_POLICY,
        CMD_NPS_SET_IDLE_ALLOWED_LIST,
        CMD_NPS_GET_IDLE_ALLOWED_LIST,
        CMD_NPS_SET_DEVICE_IDLE_POLICY,
        CMD_NPS_RESET_POLICIES,
        CMD_NPS_SET_BACKGROUND_POLICY,
        CMD_NPS_GET_BACKGROUND_POLICY,
        CMD_NPS_GET_BACKGROUND_POLICY_BY_UID,
        CMD_NPS_END = 100,
    };

public:
    /**
     * Set the network policy for the specified UID.
     *
     * @param uid The specified UID of app.
     * @param policy The network policy for application.
     *      For details, see {@link NetUidPolicy}.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t SetPolicyByUid(uint32_t uid, uint32_t policy) = 0;

    /**
     * Get the network policy of the specified UID.
     *
     * @param uid The specified UID of app.
     * @param policy Return this uid's policy.
     *      For details, see {@link NetUidPolicy}.
     * int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t GetPolicyByUid(uint32_t uid, uint32_t &policy) = 0;

    /**
     * Get the application UIDs of the specified policy.
     *
     * @param policy the network policy of the current UID of application.
     *      For details, see {@link NetUidPolicy}.
     * @param uids The list of UIDs
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t GetUidsByPolicy(uint32_t policy, std::vector<uint32_t> &uids) = 0;

    /**
     * Get the status whether the specified uid app can access the metered network or non-metered network.
     *
     * @param uid The specified UID of application.
     * @param metered Indicates metered network or non-metered network.
     * @param isAllowed True means it's allowed to access the network.
     *      False means it's not allowed to access the network.
     * @return Returns it's allowed or not to access the network.
     */
    virtual int32_t IsUidNetAllowed(uint32_t uid, bool metered, bool &isAllowed) = 0;

    /**
     * Get the status whether the specified uid app can access the specified iface network.
     *
     * @param uid The specified UID of application.
     * @param ifaceName Iface name.
     * @param isAllowed True means it's allowed to access the network.
     *      False means it's not allowed to access the network.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t IsUidNetAllowed(uint32_t uid, const std::string &ifaceName, bool &isAllowed) = 0;

    /**
     * Register network policy change callback.
     *
     * @param callback The callback of INetPolicyCallback interface.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t RegisterNetPolicyCallback(const sptr<INetPolicyCallback> &callback) = 0;

    /**
     * Unregister network policy change callback.
     *
     * @param callback The callback of INetPolicyCallback interface.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t UnregisterNetPolicyCallback(const sptr<INetPolicyCallback> &callback) = 0;

    /**
     * Set network policies.
     *
     * @param quotaPolicies The list of network quota policy, {@link NetQuotaPolicy}.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t SetNetQuotaPolicies(const std::vector<NetQuotaPolicy> &quotaPolicies) = 0;

    /**
     * Get network policies.
     *
     * @param quotaPolicies The list of network quota policy, {@link NetQuotaPolicy}.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t GetNetQuotaPolicies(std::vector<NetQuotaPolicy> &quotaPolicies) = 0;

    /**
     * Update the limit or warning remind time of quota policy.
     *
     * @param netType {@link NetBearType}.
     * @param iccid Specify the matched iccid of quota policy when netType is cellular.
     * @param remindType {@link RemindType}.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t UpdateRemindPolicy(int32_t netType, const std::string &iccid, uint32_t remindType) = 0;

    /**
     * Set the UID into device idle allow list.
     *
     * @param uid The specified UID of application.
     * @param isAllowed The UID is into allow list or not.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t SetDeviceIdleAllowedList(uint32_t uid, bool isAllowed) = 0;

    /**
     * Get the allow list of UID in device idle mode.
     *
     * @param uids The list of UIDs
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t GetDeviceIdleAllowedList(std::vector<uint32_t> &uids) = 0;

    /**
     * Process network policy in device idle mode.
     *
     * @param enable Device idle mode is open or not.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t SetDeviceIdlePolicy(bool enable) = 0;

    /**
     * Reset network policies\rules\quota policies\firewall rules.
     *
     * @param iccid Specify the matched iccid of quota policy.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t ResetPolicies(const std::string &iccid) = 0;

    /**
     * Control if apps can use data on background.
     *
     * @param isAllowed Allow apps to use data on background.
     * @return Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t SetBackgroundPolicy(bool isAllowed) = 0;

    /**
     * Get the status if apps can use data on background.
     *
     * @param backgroundPolicy True is allowed to use data on background.
     *      False is not allowed to use data on background.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t GetBackgroundPolicy(bool &backgroundPolicy) = 0;

    /**
     * Get the background network restriction policy for the specified uid.
     *
     * @param uid The specified UID of application.
     * @param backgroundPolicyOfUid The specified UID of backgroundPolicy.
     *      For details, see {@link NetBackgroundPolicy}.
     * @return uint32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    virtual int32_t GetBackgroundPolicyByUid(uint32_t uid, uint32_t &backgroundPolicyOfUid) = 0;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // I_NET_POLICY_SERVICE_H
