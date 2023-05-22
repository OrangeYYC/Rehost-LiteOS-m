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

#ifndef COMMUNICATIONNETMANAGERBASE_POLICY_ASYNC_WORK_H
#define COMMUNICATIONNETMANAGERBASE_POLICY_ASYNC_WORK_H

#include <napi/native_api.h>
#include <napi/native_node_api.h>

namespace OHOS {
namespace NetManagerStandard {
class NetPolicyAsyncWork final {
public:
    NetPolicyAsyncWork() = delete;
    ~NetPolicyAsyncWork() = delete;

    static void ExecSetPolicyByUid(napi_env env, void *data);
    static void ExecGetPolicyByUid(napi_env env, void *data);
    static void ExecGetUidsByPolicy(napi_env env, void *data);
    static void ExecSetBackgroundPolicy(napi_env env, void *data);
    static void ExecGetBackgroundPolicy(napi_env env, void *data);
    static void ExecGetNetQuotaPolicies(napi_env env, void *data);
    static void ExecSetNetQuotaPolicies(napi_env env, void *data);
    static void ExecRestoreAllPolicies(napi_env env, void *data);
    static void ExecIsUidNetAllowed(napi_env env, void *data);
    static void ExecSetDeviceIdleAllowList(napi_env env, void *data);
    static void ExecGetDeviceIdleAllowList(napi_env env, void *data);
    static void ExecGetBackgroundPolicyByUid(napi_env env, void *data);
    static void ExecResetPolicies(napi_env env, void *data);
    static void ExecUpdateRemindPolicy(napi_env env, void *data);

    static void SetPolicyByUidCallback(napi_env env, napi_status status, void *data);
    static void GetPolicyByUidCallback(napi_env env, napi_status status, void *data);
    static void GetUidsByPolicyCallback(napi_env env, napi_status status, void *data);
    static void SetBackgroundPolicyCallback(napi_env env, napi_status status, void *data);
    static void GetBackgroundPolicyCallback(napi_env env, napi_status status, void *data);
    static void GetNetQuotaPoliciesCallback(napi_env env, napi_status status, void *data);
    static void SetNetQuotaPoliciesCallback(napi_env env, napi_status status, void *data);
    static void RestoreAllPoliciesCallback(napi_env env, napi_status status, void *data);
    static void IsUidNetAllowedCallback(napi_env env, napi_status status, void *data);
    static void SetDeviceIdleAllowListCallback(napi_env env, napi_status status, void *data);
    static void GetDeviceIdleAllowListCallback(napi_env env, napi_status status, void *data);
    static void GetBackgroundPolicyByUidCallback(napi_env env, napi_status status, void *data);
    static void ResetPoliciesCallback(napi_env env, napi_status status, void *data);
    static void UpdateRemindPolicyCallback(napi_env env, napi_status status, void *data);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // COMMUNICATIONNETMANAGERBASE_POLICY_ASYNC_WORK_H
