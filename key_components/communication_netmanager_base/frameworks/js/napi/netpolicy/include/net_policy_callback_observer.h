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

#ifndef NETMANAGER_BASE_NET_POLICY_CALLBACK_OBSERVER_H
#define NETMANAGER_BASE_NET_POLICY_CALLBACK_OBSERVER_H

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include <napi/native_api.h>
#include <napi/native_node_api.h>
#include <uv.h>

#include "event_manager.h"
#include "napi_utils.h"
#include "net_policy_callback_stub.h"
#include "net_quota_policy.h"

namespace OHOS {
namespace NetManagerStandard {
class NetPolicyCallbackObserver : public NetPolicyCallbackStub {
public:
    int32_t NetUidPolicyChange(uint32_t uid, uint32_t policy) override;
    int32_t NetUidRuleChange(uint32_t uid, uint32_t rule) override;
    int32_t NetQuotaPolicyChange(const std::vector<NetQuotaPolicy> &quotaPolicies) override;
    int32_t NetMeteredIfacesChange(std::vector<std::string> &ifaces) override;
    int32_t NetBackgroundPolicyChange(bool isBackgroundPolicyAllow) override;

private:
    template <napi_value (*MakeJsValue)(napi_env, void *)> static void CallbackTemplate(uv_work_t *work, int status)
    {
        (void)status;
        if (work == nullptr) {
            return;
        }
        auto workWrapper = static_cast<UvWorkWrapper *>(work->data);
        if (workWrapper == nullptr) {
            delete work;
            return;
        }
        napi_env env = workWrapper->env;
        auto closeScope = [env](napi_handle_scope scope) { NapiUtils::CloseScope(env, scope); };
        std::unique_ptr<napi_handle_scope__, decltype(closeScope)> scope(NapiUtils::OpenScope(env), closeScope);
        napi_value obj = MakeJsValue(env, workWrapper->data);
        std::pair<napi_value, napi_value> arg = {NapiUtils::GetUndefined(workWrapper->env), obj};
        workWrapper->manager->Emit(workWrapper->type, arg);
        delete workWrapper;
        delete work;
    }

    static napi_value CreateNetQuotaPolicy(napi_env env, NetQuotaPolicy *data);
    static napi_value CreateNetUidPolicyChangeParam(napi_env env, void *data);
    static napi_value CreateNetUidRuleChangeParam(napi_env env, void *data);
    static napi_value CreateNetQuotaPolicyChangeParam(napi_env env, void *data);
    static napi_value CreateMeteredIfacesChangeParam(napi_env env, void *data);
    static napi_value CreateNetBackgroundPolicyChangeParam(napi_env env, void *data);
    static void NetUidPolicyChangeCallback(uv_work_t *work, int status);
    static void NetUidRuleChangeCallback(uv_work_t *work, int status);
    static void NetQuotaPolicyChangeCallback(uv_work_t *work, int status);
    static void NetMeteredIfacesChangeCallback(uv_work_t *work, int status);
    static void NetBackgroundPolicyChangeCallback(uv_work_t *work, int status);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETMANAGER_BASE_NET_POLICY_CALLBACK_OBSERVER_H
