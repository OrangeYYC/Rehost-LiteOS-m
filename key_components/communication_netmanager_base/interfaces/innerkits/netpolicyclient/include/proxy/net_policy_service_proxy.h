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

#ifndef NET_POLICY_SERVICE_PROXY_H
#define NET_POLICY_SERVICE_PROXY_H

#include "iremote_proxy.h"

#include "i_net_policy_service.h"
#include "net_policy_constants.h"
#include "net_quota_policy.h"

namespace OHOS {
namespace NetManagerStandard {
class NetPolicyServiceProxy : public IRemoteProxy<INetPolicyService> {
public:
    explicit NetPolicyServiceProxy(const sptr<IRemoteObject> &impl);
    virtual ~NetPolicyServiceProxy();
    int32_t SetPolicyByUid(uint32_t uid, uint32_t policy) override;
    int32_t GetPolicyByUid(uint32_t uid, uint32_t &policy) override;
    int32_t GetUidsByPolicy(uint32_t policy, std::vector<uint32_t> &uids) override;
    int32_t IsUidNetAllowed(uint32_t uid, bool metered, bool &isAllowed) override;
    int32_t IsUidNetAllowed(uint32_t uid, const std::string &ifaceName, bool &isAllowed) override;
    int32_t RegisterNetPolicyCallback(const sptr<INetPolicyCallback> &callback) override;
    int32_t UnregisterNetPolicyCallback(const sptr<INetPolicyCallback> &callback) override;
    int32_t SetNetQuotaPolicies(const std::vector<NetQuotaPolicy> &quotaPolicies) override;
    int32_t GetNetQuotaPolicies(std::vector<NetQuotaPolicy> &quotaPolicies) override;
    int32_t ResetPolicies(const std::string &iccid) override;
    int32_t SetBackgroundPolicy(bool allowBackground) override;
    int32_t GetBackgroundPolicy(bool &backgroundPolicy) override;
    int32_t GetBackgroundPolicyByUid(uint32_t uid, uint32_t &backgroundPolicyOfUid) override;
    int32_t UpdateRemindPolicy(int32_t netType, const std::string &iccid, uint32_t remindType) override;
    int32_t SetDeviceIdleAllowedList(uint32_t uid, bool isAllowed) override;
    int32_t GetDeviceIdleAllowedList(std::vector<uint32_t> &uids) override;
    int32_t SetDeviceIdlePolicy(bool enable) override;

private:
    bool WriteInterfaceToken(MessageParcel &data);

private:
    static inline BrokerDelegator<NetPolicyServiceProxy> delegator_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_POLICY_SERVICE_PROXY_H
