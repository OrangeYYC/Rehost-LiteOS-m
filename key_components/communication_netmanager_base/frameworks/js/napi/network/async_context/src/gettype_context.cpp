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

#include "gettype_context.h"

#include "network_constant.h"
#include "netmanager_base_log.h"
#include "napi_utils.h"

static constexpr const int PARAM_NO_OPTIONS = 0;

static constexpr const int PARAM_HAS_OPTIONS = 1;

namespace OHOS::NetManagerStandard {
GetTypeContext::GetTypeContext(napi_env env, EventManager *manager)
    : BaseContext(env, manager), successCallback_(nullptr), failCallback_(nullptr), completeCallback_(nullptr)
{
}

GetTypeContext::~GetTypeContext()
{
    if (successCallback_ != nullptr) {
        (void)napi_delete_reference(GetEnv(), successCallback_);
    }
    if (failCallback_ != nullptr) {
        (void)napi_delete_reference(GetEnv(), failCallback_);
    }
    if (completeCallback_ != nullptr) {
        (void)napi_delete_reference(GetEnv(), completeCallback_);
    }
}

void GetTypeContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(params, paramsCount)) {
        return;
    }

    SetNeedPromise(false);

    if (paramsCount == PARAM_HAS_OPTIONS) {
        SetParseOK(SetSuccessCallback(params[0]) && SetFailCallback(params[0]) && SetCompleteCallback(params[0]));
        return;
    }
    NETMANAGER_BASE_LOGI("no callback");
    SetParseOK(true);
}

bool GetTypeContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_NO_OPTIONS) {
        return true;
    }

    if (paramsCount == PARAM_HAS_OPTIONS) {
        return NapiUtils::GetValueType(GetEnv(), params[0]) == napi_object;
    }
    return false;
}

bool GetTypeContext::SetSuccessCallback(napi_value options)
{
    if (!NapiUtils::HasNamedProperty(GetEnv(), options, KEY_SUCCESS)) {
        NETMANAGER_BASE_LOGI("do not need success fun");
        return true;
    }
    napi_value callback = NapiUtils::GetNamedProperty(GetEnv(), options, KEY_SUCCESS);
    if (NapiUtils::GetValueType(GetEnv(), callback) != napi_function) {
        NETMANAGER_BASE_LOGE("success should be function");
        return false;
    }
    if (successCallback_ != nullptr) {
        (void)napi_delete_reference(GetEnv(), successCallback_);
    }
    return napi_create_reference(GetEnv(), callback, 1, &successCallback_) == napi_ok;
}

bool GetTypeContext::SetFailCallback(napi_value options)
{
    if (!NapiUtils::HasNamedProperty(GetEnv(), options, KEY_FAIL)) {
        NETMANAGER_BASE_LOGI("do not need fail fun");
        return true;
    }
    napi_value callback = NapiUtils::GetNamedProperty(GetEnv(), options, KEY_FAIL);
    if (NapiUtils::GetValueType(GetEnv(), callback) != napi_function) {
        NETMANAGER_BASE_LOGE("success should be function");
        return false;
    }
    if (failCallback_ != nullptr) {
        (void)napi_delete_reference(GetEnv(), failCallback_);
    }
    return napi_create_reference(GetEnv(), callback, 1, &failCallback_) == napi_ok;
}

bool GetTypeContext::SetCompleteCallback(napi_value options)
{
    if (!NapiUtils::HasNamedProperty(GetEnv(), options, KEY_COMPLETE)) {
        NETMANAGER_BASE_LOGI("do not need complete fun");
        return true;
    }
    napi_value callback = NapiUtils::GetNamedProperty(GetEnv(), options, KEY_COMPLETE);
    if (NapiUtils::GetValueType(GetEnv(), callback) != napi_function) {
        NETMANAGER_BASE_LOGE("success should be function");
        return false;
    }
    if (completeCallback_ != nullptr) {
        (void)napi_delete_reference(GetEnv(), completeCallback_);
    }
    return napi_create_reference(GetEnv(), callback, 1, &completeCallback_) == napi_ok;
}

napi_value GetTypeContext::GetSuccessCallback() const
{
    if (successCallback_ == nullptr) {
        return nullptr;
    }
    napi_value callback = nullptr;
    NAPI_CALL(GetEnv(), napi_get_reference_value(GetEnv(), successCallback_, &callback));
    return callback;
}

napi_value GetTypeContext::GetFailCallback() const
{
    if (failCallback_ == nullptr) {
        return nullptr;
    }
    napi_value callback = nullptr;
    NAPI_CALL(GetEnv(), napi_get_reference_value(GetEnv(), failCallback_, &callback));
    return callback;
}

napi_value GetTypeContext::GetCompleteCallback() const
{
    if (completeCallback_ == nullptr) {
        return nullptr;
    }
    napi_value callback = nullptr;
    NAPI_CALL(GetEnv(), napi_get_reference_value(GetEnv(), completeCallback_, &callback));
    return callback;
}

void GetTypeContext::SetCap(const NetAllCapabilities &cap)
{
    cap_ = cap;
}

NetAllCapabilities GetTypeContext::GetCap()
{
    return cap_;
}
} // namespace OHOS::NetManagerStandard