/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "napi_utils.h"
#include "netmanager_ext_log.h"

#include "constant.h"
#include "mdns_resolvelocalservice_context.h"

namespace OHOS::NetManagerStandard {
std::map<std::string, sptr<IResolveCallback>> MDnsResolveLocalServiceContext::resolveCallbackMap_;
std::mutex g_mDNSResolveMutex;

MDnsResolveLocalServiceContext::MDnsResolveLocalServiceContext(napi_env env, EventManager *manager)
    : MDnsBaseContext(env, manager), resolveObserver_(new (std::nothrow) MDnsResolveObserver())
{
}

void MDnsResolveLocalServiceContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(params, paramsCount)) {
        SetNeedThrowException(true);
        SetErrorCode(NET_MDNS_ERR_ILLEGAL_ARGUMENT);
        return;
    }

    ParseServiceInfo(GetEnv(), params[ARG_NUM_1]);

    if (paramsCount == PARAM_OPTIONS_AND_CALLBACK) {
        SetParseOK(SetCallback(params[ARG_NUM_2]) == napi_ok);
        return;
    }
    SetParseOK(true);
}

sptr<IResolveCallback> MDnsResolveLocalServiceContext::GetObserver()
{
    return resolveObserver_;
}

bool MDnsResolveLocalServiceContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    bool bRet = false;
    if (paramsCount == PARAM_JUST_OPTIONS) {
        bRet = NapiUtils::GetValueType(GetEnv(), params[ARG_NUM_0]) == napi_object &&
               NapiUtils::GetValueType(GetEnv(), params[ARG_NUM_1]) == napi_object;
    } else if (paramsCount == PARAM_OPTIONS_AND_CALLBACK) {
        bRet = NapiUtils::GetValueType(GetEnv(), params[ARG_NUM_0]) == napi_object &&
               NapiUtils::GetValueType(GetEnv(), params[ARG_NUM_1]) == napi_object &&
               NapiUtils::GetValueType(GetEnv(), params[ARG_NUM_2]) == napi_function;
    }
    return bRet;
}
} // namespace OHOS::NetManagerStandard
