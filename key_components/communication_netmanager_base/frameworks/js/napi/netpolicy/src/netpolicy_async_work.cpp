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

#include "netpolicy_async_work.h"

#include "base_async_work.h"
#include "get_net_quota_policies_context.h"
#include "get_policy_by_uid_context.h"
#include "get_uids_by_policy_context.h"
#include "is_uid_net_allowed_context.h"
#include "netpolicy_exec.h"
#include "restore_all_policies_context.h"
#include "set_bg_policy_context.h"
#include "set_device_idle_allow_list_context.h"
#include "set_net_quota_policies_context.h"
#include "set_policy_by_uid_context.h"
#include "update_remind_policy_context.h"

namespace OHOS {
namespace NetManagerStandard {
void NetPolicyAsyncWork::ExecSetPolicyByUid(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<SetPolicyByUidContext, NetPolicyExec::ExecSetPolicyByUid>(env, data);
}

void NetPolicyAsyncWork::ExecGetPolicyByUid(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetPolicyByUidContext, NetPolicyExec::ExecGetPolicyByUid>(env, data);
}

void NetPolicyAsyncWork::ExecGetUidsByPolicy(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetUidsByPolicyContext, NetPolicyExec::ExecGetUidsByPolicy>(env, data);
}

void NetPolicyAsyncWork::ExecSetBackgroundPolicy(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<SetBackgroundPolicyContext, NetPolicyExec::ExecSetBackgroundPolicy>(env, data);
}

void NetPolicyAsyncWork::ExecGetBackgroundPolicy(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetBackgroundPolicyContext, NetPolicyExec::ExecGetBackgroundPolicy>(env, data);
}

void NetPolicyAsyncWork::ExecGetNetQuotaPolicies(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetNetQuotaPoliciesContext, NetPolicyExec::ExecGetNetQuotaPolicies>(env, data);
}

void NetPolicyAsyncWork::ExecSetNetQuotaPolicies(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<SetNetQuotaPoliciesContext, NetPolicyExec::ExecSetNetQuotaPolicies>(env, data);
}

void NetPolicyAsyncWork::ExecRestoreAllPolicies(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<RestoreAllPoliciesContext, NetPolicyExec::ExecRestoreAllPolicies>(env, data);
}

void NetPolicyAsyncWork::ExecIsUidNetAllowed(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<IsUidNetAllowedContext, NetPolicyExec::ExecIsUidNetAllowed>(env, data);
}

void NetPolicyAsyncWork::ExecSetDeviceIdleAllowList(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<SetDeviceIdleAllowListContext, NetPolicyExec::ExecSetDeviceIdleAllowList>(env, data);
}

void NetPolicyAsyncWork::ExecGetDeviceIdleAllowList(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetDeviceIdleAllowListContext, NetPolicyExec::ExecGetDeviceIdleAllowList>(env, data);
}

void NetPolicyAsyncWork::ExecGetBackgroundPolicyByUid(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetBackgroundPolicyByUidContext, NetPolicyExec::ExecGetBackgroundPolicyByUid>(env,
                                                                                                               data);
}

void NetPolicyAsyncWork::ExecResetPolicies(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<ResetPoliciesContext, NetPolicyExec::ExecResetPolicies>(env, data);
}

void NetPolicyAsyncWork::ExecUpdateRemindPolicy(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<UpdateRemindPolicyContext, NetPolicyExec::ExecUpdateRemindPolicy>(env, data);
}

void NetPolicyAsyncWork::SetPolicyByUidCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<SetPolicyByUidContext, NetPolicyExec::SetPolicyByUidCallback>(env, status, data);
}

void NetPolicyAsyncWork::GetPolicyByUidCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetPolicyByUidContext, NetPolicyExec::GetPolicyByUidCallback>(env, status, data);
}

void NetPolicyAsyncWork::GetUidsByPolicyCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetUidsByPolicyContext, NetPolicyExec::GetUidsByPolicyCallback>(env, status, data);
}

void NetPolicyAsyncWork::SetBackgroundPolicyCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<SetBackgroundPolicyContext, NetPolicyExec::SetBackgroundPolicyCallback>(
        env, status, data);
}

void NetPolicyAsyncWork::GetBackgroundPolicyCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetBackgroundPolicyContext, NetPolicyExec::GetBackgroundPolicyCallback>(
        env, status, data);
}

void NetPolicyAsyncWork::GetNetQuotaPoliciesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetNetQuotaPoliciesContext, NetPolicyExec::GetNetQuotaPoliciesCallback>(
        env, status, data);
}
void NetPolicyAsyncWork::SetNetQuotaPoliciesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<SetNetQuotaPoliciesContext, NetPolicyExec::SetNetQuotaPoliciesCallback>(
        env, status, data);
}

void NetPolicyAsyncWork::RestoreAllPoliciesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<RestoreAllPoliciesContext, NetPolicyExec::RestoreAllPoliciesCallback>(env, status,
                                                                                                           data);
}

void NetPolicyAsyncWork::IsUidNetAllowedCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<IsUidNetAllowedContext, NetPolicyExec::IsUidNetAllowedCallback>(env, status, data);
}

void NetPolicyAsyncWork::SetDeviceIdleAllowListCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<SetDeviceIdleAllowListContext, NetPolicyExec::SetDeviceIdleAllowListCallback>(
        env, status, data);
}

void NetPolicyAsyncWork::GetDeviceIdleAllowListCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetDeviceIdleAllowListContext, NetPolicyExec::GetDeviceIdleAllowListCallback>(
        env, status, data);
}

void NetPolicyAsyncWork::GetBackgroundPolicyByUidCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetPolicyByUidContext, NetPolicyExec::GetBackgroundPolicyByUidCallback>(
        env, status, data);
}

void NetPolicyAsyncWork::ResetPoliciesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<ResetPoliciesContext, NetPolicyExec::ResetPoliciesCallback>(env, status, data);
}

void NetPolicyAsyncWork::UpdateRemindPolicyCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<UpdateRemindPolicyContext, NetPolicyExec::UpdateRemindPolicyCallback>(env, status,
                                                                                                           data);
}
} // namespace NetManagerStandard
} // namespace OHOS
