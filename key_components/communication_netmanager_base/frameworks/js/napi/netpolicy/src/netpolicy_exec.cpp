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

#include "netpolicy_exec.h"

#include "net_policy_client.h"
#include "net_policy_constants.h"
#include "netmanager_base_log.h"
#include "napi_utils.h"

namespace OHOS {
namespace NetManagerStandard {
bool NetPolicyExec::ExecSetPolicyByUid(SetPolicyByUidContext *context)
{
    int32_t result = DelayedSingleton<NetPolicyClient>::GetInstance()->SetPolicyByUid(context->uid_, context->policy_);
    if (result != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecSetPolicyByUid error, uid = %{public}s, policy = %{public}d, result = %{public}d",
                             std::to_string(context->uid_).c_str(), context->policy_, result);
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecGetPolicyByUid(GetPolicyByUidContext *context)
{
    int32_t result = DelayedSingleton<NetPolicyClient>::GetInstance()->GetPolicyByUid(context->uid_, context->policy_);
    if (result != NETMANAGER_SUCCESS) {
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecGetUidsByPolicy(GetUidsByPolicyContext *context)
{
    int32_t result =
        DelayedSingleton<NetPolicyClient>::GetInstance()->GetUidsByPolicy(context->policy_, context->uidTogether_);
    if (result != NETMANAGER_SUCCESS) {
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecSetBackgroundPolicy(SetBackgroundPolicyContext *context)
{
    int32_t result = DelayedSingleton<NetPolicyClient>::GetInstance()->SetBackgroundPolicy(context->isAllowed_);
    if (result != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecSetBackgroundPolicy error, isAllowed = %{public}d, result = %{public}d",
                             context->isAllowed_, result);
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecGetBackgroundPolicy(GetBackgroundPolicyContext *context)
{
    int32_t result = DelayedSingleton<NetPolicyClient>::GetInstance()->GetBackgroundPolicy(context->backgroundPolicy_);
    if (result != NETMANAGER_SUCCESS) {
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecGetNetQuotaPolicies(GetNetQuotaPoliciesContext *context)
{
    int32_t result = DelayedSingleton<NetPolicyClient>::GetInstance()->GetNetQuotaPolicies(context->quotaPolicys_);
    if (result != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecGetNetQuotaPolicies error, result = %{public}d", result);
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecSetNetQuotaPolicies(SetNetQuotaPoliciesContext *context)
{
    int32_t result = DelayedSingleton<NetPolicyClient>::GetInstance()->SetNetQuotaPolicies(context->quotaPolicys_);
    if (result != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecSetNetQuotaPolicies error, result = %{public}d, arr size = %{public}zu", result,
                             context->quotaPolicys_.size());
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecRestoreAllPolicies(RestoreAllPoliciesContext *context)
{
    int32_t result = DelayedSingleton<NetPolicyClient>::GetInstance()->ResetPolicies(context->iccid_);
    if (result != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecRestoreAllPolicies error, result = %{public}d", result);
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecIsUidNetAllowed(IsUidNetAllowedContext *context)
{
    int32_t result = NETMANAGER_SUCCESS;
    if (context->isBoolean_) {
        result = DelayedSingleton<NetPolicyClient>::GetInstance()->IsUidNetAllowed(context->uid_, context->isMetered_,
                                                                                   context->isUidNet_);
    } else {
        result = DelayedSingleton<NetPolicyClient>::GetInstance()->IsUidNetAllowed(context->uid_, context->iface_,
                                                                                   context->isUidNet_);
    }

    if (result != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecIsUidNetAllowed error, result = %{public}d", result);
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecSetDeviceIdleAllowList(SetDeviceIdleAllowListContext *context)
{
    int32_t result =
        DelayedSingleton<NetPolicyClient>::GetInstance()->SetDeviceIdleAllowedList(context->uid_, context->isAllow_);
    if (result != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecSetDeviceIdleAllowList error: uid = %{public}s, result = %{public}d",
                             std::to_string(context->uid_).c_str(), result);
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecGetDeviceIdleAllowList(GetDeviceIdleAllowListContext *context)
{
    int32_t result = DelayedSingleton<NetPolicyClient>::GetInstance()->GetDeviceIdleAllowedList(context->uids_);
    if (result != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecGetDeviceIdleAllowList error: result = %{public}d, arr size = %{public}zu", result,
                             context->uids_.size());
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecGetBackgroundPolicyByUid(GetBackgroundPolicyByUidContext *context)
{
    int32_t result = DelayedSingleton<NetPolicyClient>::GetInstance()->GetBackgroundPolicyByUid(
        context->uid_, context->backgroundPolicyOfUid_);
    if (result != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecGetBackgroundPolicyByUid error: result = %{public}d", result);
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecResetPolicies(ResetPoliciesContext *context)
{
    int32_t result = DelayedSingleton<NetPolicyClient>::GetInstance()->ResetPolicies(context->iccid_);
    if (result != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecResetPolicies error: result = %{public}d", result);
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

bool NetPolicyExec::ExecUpdateRemindPolicy(UpdateRemindPolicyContext *context)
{
    int32_t result = DelayedSingleton<NetPolicyClient>::GetInstance()->UpdateRemindPolicy(
        context->netType_, context->iccid_, context->remindType_);
    if (result != NETMANAGER_SUCCESS) {
        NETMANAGER_BASE_LOGE("ExecUpdateRemindPolicy error: result = %{public}d", result);
        context->SetErrorCode(result);
        return false;
    }
    context->SetErrorCode(result);
    return true;
}

napi_value NetPolicyExec::SetPolicyByUidCallback(SetPolicyByUidContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value NetPolicyExec::GetPolicyByUidCallback(GetPolicyByUidContext *context)
{
    return NapiUtils::CreateInt32(context->GetEnv(), context->policy_);
}

napi_value NetPolicyExec::GetUidsByPolicyCallback(GetUidsByPolicyContext *context)
{
    napi_value uids = NapiUtils::CreateArray(context->GetEnv(), context->uidTogether_.size());
    uint32_t index = 0;
    for (const auto &uid : context->uidTogether_) {
        napi_value element = NapiUtils::CreateInt32(context->GetEnv(), uid);
        NapiUtils::SetArrayElement(context->GetEnv(), uids, index++, element);
    }
    return uids;
}

napi_value NetPolicyExec::SetBackgroundPolicyCallback(SetBackgroundPolicyContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value NetPolicyExec::GetBackgroundPolicyCallback(GetBackgroundPolicyContext *context)
{
    return NapiUtils::GetBoolean(context->GetEnv(), context->backgroundPolicy_);
}

napi_value NetPolicyExec::GetNetQuotaPoliciesCallback(GetNetQuotaPoliciesContext *context)
{
    napi_value callbackValue = NapiUtils::CreateArray(context->GetEnv(), context->quotaPolicys_.size());
    uint32_t index = 0;
    for (const auto &quotaPolicy : context->quotaPolicys_) {
        napi_value element = NetPolicyExec::CreateNetQuotaPolicy(context->GetEnv(), quotaPolicy);
        NapiUtils::SetArrayElement(context->GetEnv(), callbackValue, index++, element);
    }
    return callbackValue;
}

napi_value NetPolicyExec::CreateNetQuotaPolicy(napi_env env, const NetQuotaPolicy &item)
{
    napi_value elementObject = NapiUtils::CreateObject(env);
    NapiUtils::SetInt32Property(env, elementObject, "netType", item.netType);
    NapiUtils::SetStringPropertyUtf8(env, elementObject, "iccid", item.iccid);
    NapiUtils::SetStringPropertyUtf8(env, elementObject, "ident", item.ident);
    NapiUtils::SetStringPropertyUtf8(env, elementObject, "periodDuration", item.periodDuration);
    NapiUtils::SetInt64Property(env, elementObject, "warningBytes", item.warningBytes);
    NapiUtils::SetInt64Property(env, elementObject, "limitBytes", item.limitBytes);
    NapiUtils::SetInt64Property(env, elementObject, "lastWarningRemind", item.lastWarningRemind);
    NapiUtils::SetInt64Property(env, elementObject, "lastLimitRemind", item.lastLimitRemind);
    NapiUtils::SetBooleanProperty(env, elementObject, "metered", item.metered);
    NapiUtils::SetInt32Property(env, elementObject, "limitAction", item.limitAction);
    return elementObject;
}

napi_value NetPolicyExec::SetNetQuotaPoliciesCallback(SetNetQuotaPoliciesContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value NetPolicyExec::RestoreAllPoliciesCallback(RestoreAllPoliciesContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value NetPolicyExec::IsUidNetAllowedCallback(IsUidNetAllowedContext *context)
{
    return NapiUtils::GetBoolean(context->GetEnv(), context->isUidNet_);
}

napi_value NetPolicyExec::SetDeviceIdleAllowListCallback(SetDeviceIdleAllowListContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value NetPolicyExec::GetDeviceIdleAllowListCallback(GetDeviceIdleAllowListContext *context)
{
    napi_value list = NapiUtils::CreateArray(context->GetEnv(), context->uids_.size());
    uint32_t index = 0;
    for (const auto &uid : context->uids_) {
        napi_value element = NapiUtils::CreateUint32(context->GetEnv(), uid);
        NapiUtils::SetArrayElement(context->GetEnv(), list, index++, element);
    }
    return list;
}

napi_value NetPolicyExec::GetBackgroundPolicyByUidCallback(GetBackgroundPolicyByUidContext *context)
{
    return NapiUtils::CreateInt32(context->GetEnv(), static_cast<int32_t>(context->backgroundPolicyOfUid_));
}

napi_value NetPolicyExec::ResetPoliciesCallback(ResetPoliciesContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}

napi_value NetPolicyExec::UpdateRemindPolicyCallback(UpdateRemindPolicyContext *context)
{
    return NapiUtils::GetUndefined(context->GetEnv());
}
} // namespace NetManagerStandard
} // namespace OHOS
