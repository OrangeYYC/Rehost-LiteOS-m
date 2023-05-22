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

#ifndef NET_POLICY_CALLBACK_PROXY_H
#define NET_POLICY_CALLBACK_PROXY_H

#include "iremote_proxy.h"

#include "i_net_policy_callback.h"
#include "net_policy_constants.h"
#include "net_quota_policy.h"

namespace OHOS {
namespace NetManagerStandard {
class NetPolicyCallbackProxy : public IRemoteProxy<INetPolicyCallback> {
public:
    explicit NetPolicyCallbackProxy(const sptr<IRemoteObject> &impl);
    virtual ~NetPolicyCallbackProxy();

public:
    int32_t NetUidPolicyChange(uint32_t uid, uint32_t policy) override;
    int32_t NetUidRuleChange(uint32_t uid, uint32_t rule) override;
    int32_t NetQuotaPolicyChange(const std::vector<NetQuotaPolicy> &quotaPolicies) override;
    int32_t NetStrategySwitch(const std::string &iccid, bool enable) override;
    int32_t NetMeteredIfacesChange(std::vector<std::string> &ifaces) override;
    int32_t NetBackgroundPolicyChange(bool isBackgroundPolicyAllow) override;

private:
    bool WriteInterfaceToken(MessageParcel &data);

private:
    static inline BrokerDelegator<NetPolicyCallbackProxy> delegator_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_POLICY_CALLBACK_PROXY_H