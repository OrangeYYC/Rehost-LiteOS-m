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

#ifndef NET_POLICY_SERVICE_STUB_H
#define NET_POLICY_SERVICE_STUB_H

#include <map>

#include "iremote_stub.h"

#include "i_net_policy_service.h"
#include "net_policy_event_handler.h"

namespace OHOS {
namespace NetManagerStandard {
constexpr const char *NET_POLICY_WORK_THREAD = "NET_POLICY_WORK_THREAD";

class NetPolicyServiceStub : public IRemoteStub<INetPolicyService> {
public:
    NetPolicyServiceStub();
    ~NetPolicyServiceStub();

    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

protected:
    bool CheckPermission(const std::string &permission, uint32_t funcCode);
    int32_t CheckPolicyPermission(uint32_t funcCode);
    std::shared_ptr<AppExecFwk::EventRunner> runner_;
    std::shared_ptr<NetPolicyEventHandler> handler_;

private:
    using NetPolicyServiceFunc = int32_t (NetPolicyServiceStub::*)(MessageParcel &, MessageParcel &);

private:
    void InitEventHandler();
    int32_t OnSetPolicyByUid(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetPolicyByUid(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetUidsByPolicy(MessageParcel &data, MessageParcel &reply);
    int32_t OnIsUidNetAllowedMetered(MessageParcel &data, MessageParcel &reply);
    int32_t OnIsUidNetAllowedIfaceName(MessageParcel &data, MessageParcel &reply);
    int32_t OnRegisterNetPolicyCallback(MessageParcel &data, MessageParcel &reply);
    int32_t OnUnregisterNetPolicyCallback(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetNetQuotaPolicies(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetNetQuotaPolicies(MessageParcel &data, MessageParcel &reply);
    int32_t OnResetPolicies(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetBackgroundPolicy(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetBackgroundPolicy(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetBackgroundPolicyByUid(MessageParcel &data, MessageParcel &reply);
    int32_t OnSnoozePolicy(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetDeviceIdleAllowedList(MessageParcel &data, MessageParcel &reply);
    int32_t OnGetDeviceIdleAllowedList(MessageParcel &data, MessageParcel &reply);
    int32_t OnSetDeviceIdlePolicy(MessageParcel &data, MessageParcel &reply);

private:
    std::map<uint32_t, NetPolicyServiceFunc> memberFuncMap_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_POLICY_SERVICE_STUB_H
