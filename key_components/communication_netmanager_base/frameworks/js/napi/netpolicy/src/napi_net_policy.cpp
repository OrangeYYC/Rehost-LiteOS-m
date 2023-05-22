/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "napi_net_policy.h"
#include <memory>
#include <cinttypes>
#include <charconv>
#include "net_mgr_log_wrapper.h"
#include "i_net_policy_service.h"
#include "net_policy_client.h"
#include "napi_common.h"
#include "base_context.h"
#include "net_policy_event_listener_context.h"
#include "net_all_capabilities.h"

namespace OHOS {
namespace NetManagerStandard {
const int32_t DEFAULT_REF_COUNT = 1;
const int32_t DECIMAL = 10;
static constexpr uint32_t MAX_POLICY_LEN = 100;

void NapiNetPolicy::ExecSetPolicyByUid(napi_env env, void *data)
{
    NetPolicyAsyncContext *context = static_cast<NetPolicyAsyncContext *>(data);
    if (context == nullptr) {
        NETMGR_LOG_E("context == nullptr");
        return;
    }
    NetUidPolicy policy = static_cast<NetUidPolicy>(context->policy);
    context->policyResult =
        static_cast<int32_t>(DelayedSingleton<NetPolicyClient>::GetInstance()->SetPolicyByUid(context->uid, policy));
    NETMGR_LOG_I("ExecSetPolicyByUid, policy = [%{public}d], policyResult = [%{public}d]", context->policy,
                 context->policyResult);
}

void NapiNetPolicy::ExecGetUidsByPolicy(napi_env env, void *data)
{
    NetPolicyAsyncContext *context = static_cast<NetPolicyAsyncContext *>(data);
    if (context == nullptr) {
        NETMGR_LOG_E("context == nullptr");
        return;
    }
    NetUidPolicy policy = static_cast<NetUidPolicy>(context->policy);
    context->uidTogether = DelayedSingleton<NetPolicyClient>::GetInstance()->GetUidsByPolicy(policy);
    NETMGR_LOG_I("ExecGetUidsByPolicy, policy = [%{public}d], res.length = [%{public}d]", context->policy,
                 static_cast<int32_t>(context->uidTogether.size()));
}

void NapiNetPolicy::ExecGetPolicyByUid(napi_env env, void *data)
{
    NetPolicyAsyncContext *context = static_cast<NetPolicyAsyncContext *>(data);
    if (context == nullptr) {
        NETMGR_LOG_E("context == nullptr");
        return;
    }
    context->policyResult =
        static_cast<int32_t>(DelayedSingleton<NetPolicyClient>::GetInstance()->GetPolicyByUid(context->uid));
    NETMGR_LOG_D("ExecGetPolicyByUid, uid = [%{public}d], policyResult = [%{public}d]", context->uid,
                 context->policyResult);
}

void NapiNetPolicy::ExecSetNetQuotaPolicies(napi_env env, void *data)
{
    NetPolicyAsyncContext *context = static_cast<NetPolicyAsyncContext *>(data);
    if (context == nullptr) {
        NETMGR_LOG_E("context == nullptr");
        return;
    }
    context->resultCode = DelayedSingleton<NetPolicyClient>::GetInstance()->SetNetQuotaPolicies(context->quotaPolicys);
}

void NapiNetPolicy::ExecSetSnoozePolicy(napi_env env, void *data)
{
    NetPolicyAsyncContext *context = static_cast<NetPolicyAsyncContext *>(data);
    if (context == nullptr) {
        NETMGR_LOG_E("context == nullptr");
        return;
    }
    context->resultCode = DelayedSingleton<NetPolicyClient>::GetInstance()->SetSnoozePolicy(
        context->netType, std::to_string(context->simId));
}

void NapiNetPolicy::ExecOn(napi_env env, void *data)
{
    NETMGR_LOG_I("ExecOn");
    NetPolicyAsyncContext *context = static_cast<NetPolicyAsyncContext *>(data);
    if (context == nullptr) {
        NETMGR_LOG_E("context == nullptr");
        return;
    }
    EventListener listen;
    listen.callbackRef = context->callbackRef;
    listen.env = env;
    listen.eventId = context->eventPolicyId;
    context->policyResult = NetPolicyEventListenerContext::GetInstance().AddEventListener(listen);
}

void NapiNetPolicy::ExecOff(napi_env env, void *data)
{
    NETMGR_LOG_I("ExecOff");
    NetPolicyAsyncContext *context = static_cast<NetPolicyAsyncContext *>(data);
    if (context == nullptr) {
        NETMGR_LOG_E("context == nullptr");
        return;
    }
    EventListener listen;
    listen.callbackRef = context->callbackRef;
    listen.env = env;
    listen.eventId = context->eventPolicyId;
    context->policyResult = NetPolicyEventListenerContext::GetInstance().RemoveEventListener(listen);
}

void NapiNetPolicy::CompleteSetPolicyByUid(napi_env env, napi_status status, void *data)
{
    NetPolicyAsyncContext *context = static_cast<NetPolicyAsyncContext *>(data);
    if (context == nullptr) {
        NETMGR_LOG_E("context == nullptr");
        return;
    }
    napi_value info = nullptr;
    if (context->policyResult != NETMANAGER_SUCCESS) {
        napi_create_int32(env, context->policyResult, &info);
    } else {
        info = NapiCommon::CreateUndefined(env);
    }
    if (context->callbackRef == nullptr) {
        // promiss return
        if (context->policyResult != NETMANAGER_SUCCESS) {
            NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, context->deferred, info));
        } else {
            NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, context->deferred, info));
        }
    } else {
        // call back
        napi_value callbackValues[CALLBACK_ARGV_CNT] = {nullptr, nullptr};
        napi_value recv = nullptr;
        napi_value result = nullptr;
        napi_value callbackFunc = nullptr;
        napi_get_undefined(env, &recv);
        napi_get_reference_value(env, context->callbackRef, &callbackFunc);
        if (context->policyResult != NETMANAGER_SUCCESS) {
            callbackValues[CALLBACK_ARGV_INDEX_0] = info;
        } else {
            callbackValues[CALLBACK_ARGV_INDEX_1] = info;
        }
        napi_call_function(env, recv, callbackFunc, std::size(callbackValues), callbackValues, &result);
        napi_delete_reference(env, context->callbackRef);
    }
    napi_delete_async_work(env, context->work);
    delete context;
    context = nullptr;
}

void NapiNetPolicy::CompleteGetUidsByPolicy(napi_env env, napi_status status, void *data)
{
    NetPolicyAsyncContext *context = static_cast<NetPolicyAsyncContext *>(data);
    if (context == nullptr) {
        NETMGR_LOG_E("context == nullptr");
        return;
    }
    napi_value info = nullptr;
    std::vector<uint32_t> &res = context->uidTogether;
    napi_create_array(env, &info);
    for (unsigned int i = 0; i < res.size(); i++) {
        napi_value num;
        napi_create_uint32(env, res[i], &num);
        napi_set_element(env, info, i, num);
    }
    if (!context->callbackRef) {
        NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, context->deferred, info));
    } else {
        napi_value callbackValues[CALLBACK_ARGV_CNT] = {nullptr, nullptr};
        napi_value recv = nullptr;
        napi_value result = nullptr;
        napi_value callbackFunc = nullptr;
        napi_get_undefined(env, &recv);
        napi_get_reference_value(env, context->callbackRef, &callbackFunc);
        callbackValues[CALLBACK_ARGV_INDEX_1] = info;
        napi_call_function(env, recv, callbackFunc, std::size(callbackValues), callbackValues, &result);
        napi_delete_reference(env, context->callbackRef);
    }
    napi_delete_async_work(env, context->work);
    delete context;
    context = nullptr;
}

void NapiNetPolicy::CompleteGetPolicyByUid(napi_env env, napi_status status, void *data)
{
    NetPolicyAsyncContext *context = static_cast<NetPolicyAsyncContext *>(data);
    if (context == nullptr) {
        NETMGR_LOG_E("context == nullptr");
        return;
    }
    napi_value info = nullptr;
    napi_create_int32(env, context->policyResult, &info);
    if (!context->callbackRef) {
        NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, context->deferred, info));
    } else {
        napi_value callbackValues[CALLBACK_ARGV_CNT] = {nullptr, nullptr};
        napi_value recv = nullptr;
        napi_value result = nullptr;
        napi_value callbackFunc = nullptr;
        napi_get_undefined(env, &recv);
        napi_get_reference_value(env, context->callbackRef, &callbackFunc);
        callbackValues[CALLBACK_ARGV_INDEX_1] = info;
        napi_call_function(env, recv, callbackFunc, std::size(callbackValues), callbackValues, &result);
        napi_delete_reference(env, context->callbackRef);
    }
    napi_delete_async_work(env, context->work);
    delete context;
    context = nullptr;
}

void NapiNetPolicy::CompleteSetNetQuotaPolicies(napi_env env, napi_status status, void *data)
{
    CompleteNetPolicyResultCode(env, status, data);
}

void NapiNetPolicy::CompleteSetSnoozePolicy(napi_env env, napi_status status, void *data)
{
    CompleteNetPolicyResultCode(env, status, data);
}

void NapiNetPolicy::CompleteNetPolicyResultCode(napi_env env, napi_status status, void *data)
{
    NetPolicyAsyncContext *context = static_cast<NetPolicyAsyncContext *>(data);
    if (context == nullptr) {
        NETMGR_LOG_E("context == nullptr");
        return;
    }
    napi_value info = nullptr;
    if (context->resultCode != NETMANAGER_SUCCESS) {
        info = NapiCommon::CreateCodeMessage(env, "fail", static_cast<int32_t>(context->resultCode));
    } else {
        info = NapiCommon::CreateUndefined(env);
    }
    if (!context->callbackRef) { // promiss return
        if (context->resultCode != NETMANAGER_SUCCESS) {
            NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, context->deferred, info));
        } else {
            NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, context->deferred, info));
        }
    } else { // call back
        napi_value callbackValues[CALLBACK_ARGV_CNT] = {nullptr, nullptr};
        napi_value recv = nullptr;
        napi_value result = nullptr;
        napi_value callbackFunc = nullptr;
        napi_get_undefined(env, &recv);
        napi_get_reference_value(env, context->callbackRef, &callbackFunc);
        if (context->resultCode != NETMANAGER_SUCCESS) {
            callbackValues[CALLBACK_ARGV_INDEX_0] = info;
        } else {
            callbackValues[CALLBACK_ARGV_INDEX_1] = info;
        }
        napi_call_function(env, recv, callbackFunc, std::size(callbackValues), callbackValues, &result);
        napi_delete_reference(env, context->callbackRef);
    }
    napi_delete_async_work(env, context->work);
    delete context;
    context = nullptr;
}

void NapiNetPolicy::CompleteOn(napi_env env, napi_status status, void *data)
{
    NETMGR_LOG_I("CompleteOn");
    NetPolicyAsyncContext *context = static_cast<NetPolicyAsyncContext *>(data);
    if (context == nullptr) {
        NETMGR_LOG_E("context == nullptr");
        return;
    }
    napi_value info = nullptr;
    if (context->callbackRef != nullptr) {
        if (context->policyResult != 0) {
            napi_value callbackValues[CALLBACK_ARGV_CNT] = {nullptr, nullptr};
            napi_value recv = nullptr;
            napi_value result = nullptr;
            napi_value callbackFunc = nullptr;
            napi_get_undefined(env, &recv);
            napi_get_reference_value(env, context->callbackRef, &callbackFunc);
            napi_create_int32(env, context->policyResult, &info);
            callbackValues[CALLBACK_ARGV_INDEX_0] = info;
            napi_call_function(env, recv, callbackFunc, std::size(callbackValues), callbackValues, &result);
            napi_delete_reference(env, context->callbackRef);
        }
    }
    napi_delete_async_work(env, context->work);
    delete context;
    context = nullptr;
}

void NapiNetPolicy::CompleteOff(napi_env env, napi_status status, void *data)
{
    NETMGR_LOG_I("CompleteOff");
    NetPolicyAsyncContext *context = static_cast<NetPolicyAsyncContext *>(data);
    if (context == nullptr) {
        NETMGR_LOG_E("context == nullptr");
        return;
    }
    napi_value info = nullptr;
    if (context->callbackRef != nullptr) {
        if (context->policyResult != 0) {
            napi_value callbackValues[CALLBACK_ARGV_CNT] = {nullptr, nullptr};
            napi_value recv = nullptr;
            napi_value result = nullptr;
            napi_value callbackFunc = nullptr;
            napi_get_undefined(env, &recv);
            napi_get_reference_value(env, context->callbackRef, &callbackFunc);
            napi_create_int32(env, context->policyResult, &info);
            callbackValues[CALLBACK_ARGV_INDEX_0] = info;
            napi_call_function(env, recv, callbackFunc, std::size(callbackValues), callbackValues, &result);
            napi_delete_reference(env, context->callbackRef);
        }
    }
    napi_delete_async_work(env, context->work);
    delete context;
    context = nullptr;
}

NetPolicyQuotaPolicy NapiNetPolicy::ReadQuotaPolicy(napi_env env, napi_value value)
{
    NetPolicyQuotaPolicy data;
    data.netType_ = static_cast<int8_t>(NapiCommon::GetNapiInt32Value(env, value, "netType"));
    data.simId_ = std::to_string(NapiCommon::GetNapiInt64Value(env, value, "simId"));
    data.periodStartTime_ = NapiCommon::GetNapiInt64Value(env, value, "periodStartTime");
    data.periodDuration_ = NapiCommon::GetNapiStringValue(env, value, "periodDuration");
    data.warningBytes_ = NapiCommon::GetNapiInt64Value(env, value, "warningBytes");
    data.limitBytes_ = NapiCommon::GetNapiInt64Value(env, value, "limitBytes");
    data.lastLimitSnooze_ = NapiCommon::GetNapiInt64Value(env, value, "lastLimitSnooze");
    data.metered_ = static_cast<int8_t>(NapiCommon::GetNapiInt32Value(env, value, "metered"));
    return data;
}

bool MatchGetNetQuotaPoliciesInputParam(napi_env env, const napi_value parameters[], size_t parameterCount)
{
    switch (parameterCount) {
        case ARGV_NUM_0:
            return true;
        case ARGV_NUM_1:
            return NapiCommon::MatchValueType(env, parameters[0], napi_function);
        default:
            return false;
    }
}

void NativeGetNetQuotaPolicies(napi_env env, void *data)
{
    auto context = static_cast<GetNetPolicyContext *>(data);
    if (context == nullptr) {
        NETMGR_LOG_E("context == nullptr");
        return;
    }
    NetPolicyResultCode result = DelayedSingleton<NetPolicyClient>::GetInstance()->GetNetQuotaPolicies(context->result);
    context->resolved = result == NETMANAGER_SUCCESS;
    context->errorCode = static_cast<int32_t>(result);
}

void GetNetQuotaPoliciesCallback(napi_env env, napi_status status, void *data)
{
    auto context = static_cast<GetNetPolicyContext *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_create_array(env, &callbackValue);
            int32_t arraySize = static_cast<int32_t>(context->result.size());
            for (int32_t i = 0; i < arraySize; i++) {
                napi_value elementObject = nullptr;
                napi_create_object(env, &elementObject);
                NetPolicyQuotaPolicy item = context->result[i];
                NapiCommon::SetPropertyInt32(env, elementObject, "netType", item.netType_);
                int64_t simIdValue = 0;
                std::from_chars(&(*item.simId_.begin()), &(*item.simId_.end()), simIdValue, DECIMAL);
                NapiCommon::SetPropertyInt64(env, elementObject, "simId", simIdValue);
                NapiCommon::SetPropertyInt64(env, elementObject, "periodStartTime", item.periodStartTime_);
                NapiCommon::SetPropertyString(env, elementObject, "periodDuration", item.periodDuration_);
                NapiCommon::SetPropertyInt64(env, elementObject, "warningBytes", item.warningBytes_);
                NapiCommon::SetPropertyInt64(env, elementObject, "limitBytes", item.limitBytes_);
                NapiCommon::SetPropertyInt64(env, elementObject, "lastLimitSnooze", item.lastLimitSnooze_);
                NapiCommon::SetPropertyInt32(env, elementObject, "metered", static_cast<int32_t>(item.metered_));
                napi_set_element(env, callbackValue, i, elementObject);
            }
        } else {
            callbackValue = NapiCommon::CreateCodeMessage(env, "Failed to GetNetQuotaPolicies",
                                                          static_cast<int32_t>(context->errorCode));
        }
    } else {
        callbackValue =
            NapiCommon::CreateErrorMessage(env, "GetNetQuotaPolicies error,napi_status = " + std::to_string(status));
    }
    NapiCommon::Handle2ValueCallback(env, context, callbackValue);
}

napi_value NapiNetPolicy::GetNetQuotaPolicies(napi_env env, napi_callback_info info)
{
    size_t paramsCount = ARGV_NUM_1;
    napi_value params[ARGV_NUM_1] = {0};
    napi_value arg = nullptr;
    void *data = nullptr;
    napi_get_cb_info(env, info, &paramsCount, params, &arg, &data);

    NAPI_ASSERT(env, MatchGetNetQuotaPoliciesInputParam(env, params, paramsCount),
                "GetNetQuotaPolicies input param type mismatch");

    auto context = std::make_unique<GetNetPolicyContext>().release();
    if (paramsCount == ARGV_NUM_1) {
        napi_create_reference(env, params[0], DEFAULT_REF_COUNT, &context->callbackRef);
    }
    napi_value result = NapiCommon::HandleAsyncWork(env, context, "GetNetQuotaPolicies", NativeGetNetQuotaPolicies,
                                                    GetNetQuotaPoliciesCallback);
    return result;
}

bool MatchRestoreAllPoliciesParameters(napi_env env, napi_value argv[], size_t argc)
{
    switch (argc) {
        case ARGV_INDEX_1: {
            return NapiCommon::MatchParameters(env, argv, {napi_number});
        }
        case ARGV_INDEX_2: {
            return NapiCommon::MatchParameters(env, argv, {napi_number, napi_function});
        }
        default: {
            return false;
        }
    }
}

void NapiNetPolicy::ExecRestoreAllPolicies(napi_env env, void *data)
{
    auto context = static_cast<RestoreAllPoliciesContext *>(data);
    DelayedSingleton<NetPolicyClient>::GetInstance()->SetFactoryPolicy(std::to_string(context->simId));
    context->resolved = true;
}

void NapiNetPolicy::CompleteRestoreAllPolicies(napi_env env, napi_status status, void *data)
{
    NETMGR_LOG_I("CompleteRestoreAllPolicies start");
    auto context = static_cast<RestoreAllPoliciesContext *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_get_undefined(env, &callbackValue);
        } else {
            callbackValue = NapiCommon::CreateErrorMessage(env, "policy reset factory error");
        }
    } else {
        callbackValue =
            NapiCommon::CreateErrorMessage(env, "reset factory error cause napi_status = " + std::to_string(status));
    }
    NapiCommon::Handle1ValueCallback(env, context, callbackValue);
}

napi_value NapiNetPolicy::RestoreAllPolicies(napi_env env, napi_callback_info info)
{
    size_t argc = ARGV_NUM_2;
    napi_value argv[] = {nullptr, nullptr};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    NAPI_ASSERT(env, MatchRestoreAllPoliciesParameters(env, argv, argc), "type mismatch");
    auto context = std::make_unique<RestoreAllPoliciesContext>().release();
    NETMGR_LOG_I("napi_policy RestoreAllPolicies start.");
    napi_get_value_int32(env, argv[0], &context->simId);
    if (argc == ARGV_NUM_2) {
        napi_create_reference(env, argv[1], CALLBACK_REF_CNT, &context->callbackRef);
    }
    napi_value result = NapiCommon::HandleAsyncWork(env, context, "RestoreAllPolicies", ExecRestoreAllPolicies,
                                                    CompleteRestoreAllPolicies);
    return result;
}

bool MatchGetBackgroundPolicyParameters(napi_env env, napi_value argv[], size_t argc)
{
    switch (argc) {
        case ARGV_INDEX_0:
            return true;
        case ARGV_INDEX_1:
            return NapiCommon::MatchParameters(env, argv, {napi_function});
        default:
            return false;
    }
}

void NapiNetPolicy::ExecSetBackgroundPolicy(napi_env env, void *data)
{
    NetPolicyAsyncContext *context = static_cast<NetPolicyAsyncContext *>(data);
    if (context == nullptr) {
        NETMGR_LOG_E("context == nullptr");
        return;
    }
    context->resultCode = DelayedSingleton<NetPolicyClient>::GetInstance()->SetBackgroundPolicy(context->allow);
}

void NapiNetPolicy::ExecGetBackgroundPolicy(napi_env env, void *data)
{
    auto context = static_cast<GetBackgroundPolicyContext *>(data);
    context->backgroundPolicy = DelayedSingleton<NetPolicyClient>::GetInstance()->GetCurrentBackgroundPolicy();
    context->resolved = true;
}

void NapiNetPolicy::CompleteSetBackgroundPolicy(napi_env env, napi_status status, void *data)
{
    NetPolicyAsyncContext *context = static_cast<NetPolicyAsyncContext *>(data);
    if (context == nullptr) {
        NETMGR_LOG_E("context == nullptr");
        return;
    }
    napi_value info = nullptr;
    if (context->resultCode != NETMANAGER_SUCCESS) {
        info = NapiCommon::CreateCodeMessage(env, "fail", static_cast<int32_t>(context->resultCode));
    } else {
        info = NapiCommon::CreateUndefined(env);
    }
    if (!context->callbackRef) { // promiss return
        if (context->resultCode != NETMANAGER_SUCCESS) {
            NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, context->deferred, info));
        } else {
            NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, context->deferred, info));
        }
    } else { // call back
        napi_value callbackValues[CALLBACK_ARGV_CNT] = {nullptr, nullptr};
        napi_value recv = nullptr;
        napi_value result = nullptr;
        napi_value callbackFunc = nullptr;
        napi_get_undefined(env, &recv);
        napi_get_reference_value(env, context->callbackRef, &callbackFunc);
        if (context->resultCode != NETMANAGER_SUCCESS) {
            callbackValues[CALLBACK_ARGV_INDEX_0] = info;
        } else {
            callbackValues[CALLBACK_ARGV_INDEX_1] = info;
        }
        napi_call_function(env, recv, callbackFunc, std::size(callbackValues), callbackValues, &result);
        napi_delete_reference(env, context->callbackRef);
    }
    napi_delete_async_work(env, context->work);
    delete context;
    context = nullptr;
}

void NapiNetPolicy::CompleteGetBackgroundPolicy(napi_env env, napi_status status, void *data)
{
    NETMGR_LOG_I("CompleteSetBackgroundPolicy start");
    auto context = static_cast<GetBackgroundPolicyContext *>(data);
    napi_value callbackValue = nullptr;
    if (status == napi_ok) {
        if (context->resolved) {
            napi_create_int32(env, static_cast<int32_t>(context->backgroundPolicy), &callbackValue);
        } else {
            callbackValue = NapiCommon::CreateErrorMessage(env, "set background error");
        }
    } else {
        callbackValue =
            NapiCommon::CreateErrorMessage(env, "set background error cause napi_status = " + std::to_string(status));
    }
    NapiCommon::Handle2ValueCallback(env, context, callbackValue);
}

napi_value NapiNetPolicy::SetBackgroundPolicy(napi_env env, napi_callback_info info)
{
    size_t argc = ARGV_NUM_2;
    napi_value argv[] = {nullptr, nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    NetPolicyAsyncContext *context = std::make_unique<NetPolicyAsyncContext>().release();
    NAPI_CALL(env, napi_get_value_bool(env, argv[ARGV_INDEX_0], &context->allow));
    NETMGR_LOG_I("JS agvc count = [%{public}d], argv[ARGV_INDEX_0] = [%{public}d], ", static_cast<int>(argc),
                 static_cast<int>(context->allow));
    napi_value result = nullptr;
    if (argc == ARGV_NUM_1) {
        if (context->callbackRef == nullptr) {
            NAPI_CALL(env, napi_create_promise(env, &context->deferred, &result));
        } else {
            NAPI_CALL(env, napi_get_undefined(env, &result));
        }
    } else if (argc == ARGV_NUM_2) {
        NAPI_CALL(env, napi_create_reference(env, argv[ARGV_INDEX_1], CALLBACK_REF_CNT, &context->callbackRef));
    } else {
        NETMGR_LOG_E("SetBackgroundPolicy exception");
    }
    // creat async work
    napi_value resource = nullptr;
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &resource));
    NAPI_CALL(env, napi_create_string_utf8(env, "SetBackgroundPolicy", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env, resource, resourceName, ExecSetBackgroundPolicy,
                                          CompleteSetBackgroundPolicy, context, &context->work));
    NAPI_CALL(env, napi_queue_async_work(env, context->work));
    return result;
}

napi_value NapiNetPolicy::GetBackgroundPolicy(napi_env env, napi_callback_info info)
{
    size_t argc = ARGV_NUM_1;
    napi_value argv[] = {nullptr};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    NAPI_ASSERT(env, MatchGetBackgroundPolicyParameters(env, argv, argc), "type mismatch");
    auto context = std::make_unique<GetBackgroundPolicyContext>().release();
    if (argc == 1) {
        napi_create_reference(env, argv[0], CALLBACK_REF_CNT, &context->callbackRef);
    }
    napi_value result = NapiCommon::HandleAsyncWork(env, context, "GetBackgroundPolicy", ExecGetBackgroundPolicy,
                                                    CompleteGetBackgroundPolicy);
    return result;
}

napi_value NapiNetPolicy::DeclareNapiNetPolicyInterface(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("setPolicyByUid", SetPolicyByUid),
        DECLARE_NAPI_FUNCTION("getPolicyByUid", GetPolicyByUid),
        DECLARE_NAPI_FUNCTION("getUidsByPolicy", GetUidsByPolicy),
        DECLARE_NAPI_FUNCTION("setNetQuotaPolicies", SetNetQuotaPolicies),
        DECLARE_NAPI_FUNCTION("getNetQuotaPolicies", GetNetQuotaPolicies),
        DECLARE_NAPI_FUNCTION("restoreAllPolicies", RestoreAllPolicies),
        DECLARE_NAPI_FUNCTION("setSnoozePolicy", SetSnoozePolicy),
        DECLARE_NAPI_FUNCTION("setBackgroundPolicy", SetBackgroundPolicy),
        DECLARE_NAPI_FUNCTION("getBackgroundPolicy", GetBackgroundPolicy),
        DECLARE_NAPI_FUNCTION("on", On),
        DECLARE_NAPI_FUNCTION("off", Off),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    return exports;
}

napi_value NapiNetPolicy::SetPolicyByUid(napi_env env, napi_callback_info info)
{
    size_t argc = ARGV_NUM_3;
    napi_value argv[] = {nullptr, nullptr, nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    NetPolicyAsyncContext *context = std::make_unique<NetPolicyAsyncContext>().release();
    NAPI_CALL(env, napi_get_value_uint32(env, argv[ARGV_INDEX_0], &context->uid));
    NAPI_CALL(env, napi_get_value_uint32(env, argv[ARGV_INDEX_1], &context->policy));
    NETMGR_LOG_D(
        "JS agvc count = [%{public}d], argv[ARGV_INDEX_0] = [%{public}d], "
        "argv[ARGV_INDEX_1] = [%{public}d]",
        static_cast<int>(argc), context->uid, static_cast<int>(context->policy));
    napi_value result = nullptr;
    if (argc == ARGV_NUM_2) {
        if (context->callbackRef == nullptr) {
            NAPI_CALL(env, napi_create_promise(env, &context->deferred, &result));
        } else {
            NAPI_CALL(env, napi_get_undefined(env, &result));
        }
    } else if (argc == ARGV_NUM_3) {
        NAPI_CALL(env, napi_create_reference(env, argv[ARGV_INDEX_2], CALLBACK_REF_CNT, &context->callbackRef));
    } else {
        NETMGR_LOG_E("SetPolicyByUid exception");
    }
    // creat async work
    napi_value resource = nullptr;
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &resource));
    NAPI_CALL(env, napi_create_string_utf8(env, "SetPolicyByUid", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env, resource, resourceName, ExecSetPolicyByUid, CompleteSetPolicyByUid,
                                          context, &context->work));
    NAPI_CALL(env, napi_queue_async_work(env, context->work));
    return result;
}

napi_value NapiNetPolicy::GetPolicyByUid(napi_env env, napi_callback_info info)
{
    size_t argc = ARGV_NUM_2;
    napi_value argv[] = {nullptr, nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    NetPolicyAsyncContext *context = std::make_unique<NetPolicyAsyncContext>().release();
    NAPI_CALL(env, napi_get_value_uint32(env, argv[ARGV_INDEX_0], &context->uid));
    NETMGR_LOG_D("JS agvc count = [%{public}d], argv[0] = [%{public}d]", static_cast<int32_t>(argc), context->uid);
    // Get and verify parameter[js]
    napi_value result = nullptr;
    if (argc == ARGV_NUM_1) {
        if (!context->callbackRef) {
            NAPI_CALL(env, napi_create_promise(env, &context->deferred, &result));
        } else {
            NAPI_CALL(env, napi_get_undefined(env, &result));
        }
    } else if (argc == ARGV_NUM_2) {
        NAPI_CALL(env, napi_create_reference(env, argv[ARGV_INDEX_1], CALLBACK_REF_CNT, &context->callbackRef));
    } else {
        NETMGR_LOG_E("GetPolicyByUid exception");
    }
    // creat async work
    napi_value resource = nullptr;
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &resource));
    NAPI_CALL(env, napi_create_string_utf8(env, "GetUidsByPolicy", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env, resource, resourceName, ExecGetPolicyByUid, CompleteGetPolicyByUid,
                                          context, &context->work));
    NAPI_CALL(env, napi_queue_async_work(env, context->work));
    return result;
}

napi_value NapiNetPolicy::GetUidsByPolicy(napi_env env, napi_callback_info info)
{
    size_t argc = ARGV_NUM_2;
    napi_value argv[] = {nullptr, nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    NetPolicyAsyncContext *context = std::make_unique<NetPolicyAsyncContext>().release();
    NAPI_CALL(env, napi_get_value_uint32(env, argv[ARGV_INDEX_0], &context->policy));
    NETMGR_LOG_E("JS agvc count = [%{public}d], argv[0] = [%{public}d]", static_cast<int32_t>(argc), context->policy);
    // Get and verify parameter[js]
    napi_value result = nullptr;
    if (argc == ARGV_NUM_1) {
        if (!context->callbackRef) {
            NAPI_CALL(env, napi_create_promise(env, &context->deferred, &result));
        } else {
            NAPI_CALL(env, napi_get_undefined(env, &result));
        }
    } else if (argc == ARGV_NUM_2) {
        NAPI_CALL(env, napi_create_reference(env, argv[ARGV_INDEX_1], CALLBACK_REF_CNT, &context->callbackRef));
    } else {
        NETMGR_LOG_E("GetUidsByPolicy exception");
    }
    // creat async work
    napi_value resource = nullptr;
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &resource));
    NAPI_CALL(env, napi_create_string_utf8(env, "GetUidsByPolicy", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env, resource, resourceName, ExecGetUidsByPolicy, CompleteGetUidsByPolicy,
                                          context, &context->work));
    NAPI_CALL(env, napi_queue_async_work(env, context->work));
    return result;
}

napi_value NapiNetPolicy::SetNetQuotaPolicies(napi_env env, napi_callback_info info)
{
    NETMGR_LOG_D("SetNetQuotaPolicies");
    size_t argc = ARGV_NUM_2;
    napi_value argv[] = {nullptr, nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    NetPolicyAsyncContext *context = std::make_unique<NetPolicyAsyncContext>().release();

    bool isArray = false;
    NAPI_CALL(env, napi_is_array(env, argv[0], &isArray));
    if (!isArray) {
        NETMGR_LOG_D("An array parameter is required.");
        return nullptr;
    }
    uint32_t arrayLength = 0;
    napi_get_array_length(env, argv[0], &arrayLength);
    arrayLength = arrayLength > MAX_POLICY_LEN ? MAX_POLICY_LEN : arrayLength;
    napi_value elementValue = nullptr;
    for (uint32_t i = 0; i < arrayLength; i++) {
        napi_get_element(env, argv[0], i, &elementValue);
        NetPolicyQuotaPolicy data = ReadQuotaPolicy(env, elementValue);
        context->quotaPolicys.push_back(data);
    }

    napi_value result = nullptr;
    if (argc == ARGV_NUM_1) { // promise call
        if (!context->callbackRef) {
            // promise call, other callref call
            NAPI_CALL(env, napi_create_promise(env, &context->deferred, &result));
        } else {
            NAPI_CALL(env, napi_get_undefined(env, &result));
        }
    } else if (argc == ARGV_NUM_2) { // callback
        NAPI_CALL(env, napi_create_reference(env, argv[ARGV_INDEX_1], CALLBACK_REF_CNT, &context->callbackRef));
    } else {
        // exception
    }
    // creat async work
    napi_value resource = nullptr;
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &resource));
    NAPI_CALL(env, napi_create_string_utf8(env, "SetNetQuotaPolicies", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env, resource, resourceName, ExecSetNetQuotaPolicies,
                                          CompleteSetNetQuotaPolicies, context, &context->work));
    NAPI_CALL(env, napi_queue_async_work(env, context->work));
    return result;
}

napi_value NapiNetPolicy::SetSnoozePolicy(napi_env env, napi_callback_info info)
{
    NETMGR_LOG_D("SetSnoozePolicy");
    size_t argc = ARGV_NUM_3;
    napi_value argv[] = {nullptr, nullptr, nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    NetPolicyAsyncContext *context = std::make_unique<NetPolicyAsyncContext>().release();
    int32_t netType = 0;
    NAPI_CALL(env, napi_get_value_int32(env, argv[ARGV_INDEX_0], &netType));
    context->netType = static_cast<int8_t>(netType);
    NAPI_CALL(env, napi_get_value_int32(env, argv[ARGV_INDEX_1], &context->simId));
    napi_value result = nullptr;
    if (argc == ARGV_NUM_2) { // promise call
        if (!context->callbackRef) {
            // promise call, other callref call
            NAPI_CALL(env, napi_create_promise(env, &context->deferred, &result));
        } else {
            NAPI_CALL(env, napi_get_undefined(env, &result));
        }
    } else if (argc == ARGV_NUM_3) { // callback
        NAPI_CALL(env, napi_create_reference(env, argv[ARGV_INDEX_2], CALLBACK_REF_CNT, &context->callbackRef));
    } else {
        NETMGR_LOG_E("Unexpected parameters.");
    }
    // creat async work
    napi_value resource = nullptr;
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &resource));
    NAPI_CALL(env, napi_create_string_utf8(env, "SetSnoozePolicy", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env, resource, resourceName, ExecSetSnoozePolicy, CompleteSetSnoozePolicy,
                                          context, &context->work));
    NAPI_CALL(env, napi_queue_async_work(env, context->work));
    return result;
}

napi_value NapiNetPolicy::On(napi_env env, napi_callback_info info)
{
    NETMGR_LOG_D("On");
    size_t argc = ARGV_NUM_2;
    napi_value argv[] = {nullptr, nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    NETMGR_LOG_D("On agvc = %{public}d", static_cast<int32_t>(argc));
    NetPolicyAsyncContext *context = std::make_unique<NetPolicyAsyncContext>().release();
    NAPI_CALL(env, napi_get_value_string_utf8(env, argv[ARGV_INDEX_0], context->eventContent, EVENT_CONTENT_MAX_BYTE,
                                              &(context->eventContentRealBytes)));
    NETMGR_LOG_D("On eventContent = [%{public}s]", context->eventContent);
    std::string content = context->eventContent;
    napi_value result = nullptr;
    if (argc == ARGV_NUM_2) {
        if (NapiCommon::IsValidEvent(content, context->eventPolicyId)) {
            NAPI_CALL(env, napi_create_reference(env, argv[ARGV_INDEX_1], CALLBACK_REF_CNT, &context->callbackRef));
        } else {
            NETMGR_LOG_E("NapiNetConn::On exception[event]");
            return nullptr;
        }
    } else {
        NETMGR_LOG_E("NapiNetConn::On exception [num]");
        return nullptr;
    }
    napi_value resource = nullptr;
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &resource));
    NAPI_CALL(env, napi_create_string_utf8(env, "On", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env, resource, resourceName, ExecOn, CompleteOn, context, &context->work));
    NAPI_CALL(env, napi_queue_async_work(env, context->work));
    return result;
}

napi_value NapiNetPolicy::Off(napi_env env, napi_callback_info info)
{
    NETMGR_LOG_D("On");
    size_t argc = ARGV_NUM_2;
    napi_value argv[] = {nullptr, nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    NETMGR_LOG_D("Off agvc = %{public}d", static_cast<int32_t>(argc));
    NetPolicyAsyncContext *context = std::make_unique<NetPolicyAsyncContext>().release();
    NAPI_CALL(env, napi_get_value_string_utf8(env, argv[ARGV_INDEX_0], context->eventContent, EVENT_CONTENT_MAX_BYTE,
                                              &(context->eventContentRealBytes)));
    NETMGR_LOG_D("Off eventContent = [%{public}s]", context->eventContent);
    std::string content = context->eventContent;
    napi_value result = nullptr;
    if (argc == ARGV_NUM_2) {
        if (NapiCommon::IsValidEvent(content, context->eventPolicyId)) {
            NAPI_CALL(env, napi_create_reference(env, argv[ARGV_INDEX_1], CALLBACK_REF_CNT, &context->callbackRef));
        } else {
            NETMGR_LOG_E("NapiNetConn::Off exception[event]");
            return nullptr;
        }
    } else {
        NETMGR_LOG_E("NapiNetConn::Off exception [num]");
        return nullptr;
    }
    napi_value resource = nullptr;
    napi_value resourceName = nullptr;
    NAPI_CALL(env, napi_get_undefined(env, &resource));
    NAPI_CALL(env, napi_create_string_utf8(env, "Off", NAPI_AUTO_LENGTH, &resourceName));
    NAPI_CALL(env, napi_create_async_work(env, resource, resourceName, ExecOff, CompleteOff, context, &context->work));
    NAPI_CALL(env, napi_queue_async_work(env, context->work));
    return result;
}

static napi_value CreateNetUidPolicy(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "NET_POLICY_NONE", NapiCommon::NapiValueByInt32(env, static_cast<int32_t>(NetUidPolicy::NET_POLICY_NONE))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "NET_POLICY_ALLOW_METERED_BACKGROUND",
            NapiCommon::NapiValueByInt32(env, static_cast<int32_t>(NetUidPolicy::NET_POLICY_ALLOW_METERED_BACKGROUND))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "NET_POLICY_TEMPORARY_ALLOW_METERED",
            NapiCommon::NapiValueByInt32(env, static_cast<int32_t>(NetUidPolicy::NET_POLICY_TEMPORARY_ALLOW_METERED))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "NET_POLICY_REJECT_METERED_BACKGROUND",
            NapiCommon::NapiValueByInt32(env,
                                         static_cast<int32_t>(NetUidPolicy::NET_POLICY_REJECT_METERED_BACKGROUND))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "NET_POLICY_ALLOW_ALL",
            NapiCommon::NapiValueByInt32(env, static_cast<int32_t>(NetUidPolicy::NET_POLICY_ALLOW_ALL))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "NET_POLICY_REJECT_ALL",
            NapiCommon::NapiValueByInt32(env, static_cast<int32_t>(NetUidPolicy::NET_POLICY_REJECT_ALL))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "NetUidPolicy", NAPI_AUTO_LENGTH, NapiCommon::CreateEnumConstructor, nullptr,
                      sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "NetUidPolicy", result);
    return exports;
}

static napi_value CreateBackgroundPolicy(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY(
            "BACKGROUND_POLICY_DISABLE",
            NapiCommon::NapiValueByInt32(env,
                                         static_cast<int32_t>(NetBackgroundPolicy::NET_BACKGROUND_POLICY_DISABLE))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "BACKGROUND_POLICY_ALLOWLISTED",
            NapiCommon::NapiValueByInt32(env,
                                         static_cast<int32_t>(NetBackgroundPolicy::NET_BACKGROUND_POLICY_ALLOWLISTED))),
        DECLARE_NAPI_STATIC_PROPERTY(
            "BACKGROUND_POLICY_ENABLED",
            NapiCommon::NapiValueByInt32(env,
                                         static_cast<int32_t>(NetBackgroundPolicy::NET_BACKGROUND_POLICY_ENABLED))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "BackgroundPolicy", NAPI_AUTO_LENGTH, NapiCommon::CreateEnumConstructor, nullptr,
                      sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "BackgroundPolicy", result);
    return exports;
}

static napi_value CreateMeteringMode(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_STATIC_PROPERTY("UN_METERED",
                                     NapiCommon::NapiValueByInt32(env, static_cast<int32_t>(MeteringMode::UN_METERED))),
        DECLARE_NAPI_STATIC_PROPERTY("METERED",
                                     NapiCommon::NapiValueByInt32(env, static_cast<int32_t>(MeteringMode::METERED))),
    };
    napi_value result = nullptr;
    napi_define_class(env, "MeteringMode", NAPI_AUTO_LENGTH, NapiCommon::CreateEnumConstructor, nullptr,
                      sizeof(desc) / sizeof(*desc), desc, &result);
    napi_set_named_property(env, exports, "MeteringMode", result);
    return exports;
}

napi_value NapiNetPolicy::RegisterNetPolicyInterface(napi_env env, napi_value exports)
{
    DeclareNapiNetPolicyInterface(env, exports);
    CreateNetUidPolicy(env, exports);
    CreateBackgroundPolicy(env, exports);
    CreateMeteringMode(env, exports);
    return exports;
}

static napi_module _netPolicyModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = NapiNetPolicy::RegisterNetPolicyInterface,
    .nm_modname = "net.policy",
    .nm_priv = ((void *)0),
    .reserved = {0},
};

extern "C" __attribute__((constructor)) void RegisterNetPolicyModule(void)
{
    napi_module_register(&_netPolicyModule);
}
} // namespace NetManagerStandard
} // namespace OHOS
