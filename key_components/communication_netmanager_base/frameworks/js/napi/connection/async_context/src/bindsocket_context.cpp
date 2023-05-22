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

#include "bindsocket_context.h"

#include "constant.h"
#include "netmanager_base_log.h"
#include "napi_constant.h"
#include "napi_utils.h"

namespace OHOS::NetManagerStandard {
BindSocketContext::BindSocketContext(napi_env env, EventManager *manager)
    : BaseContext(env, manager), netId_(0), socketFd_(0)
{
}

void BindSocketContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(params, paramsCount)) {
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        return;
    }

    socketFd_ = NapiUtils::GetInt32Property(GetEnv(), params[ARG_INDEX_0], KEY_SOCKET_FD);
    if (socketFd_ == 0) {
        NETMANAGER_BASE_LOGE("socket is not bind");
        return;
    }

    if (paramsCount == PARAM_OPTIONS_AND_CALLBACK) {
        SetParseOK(SetCallback(params[ARG_INDEX_1]) == napi_ok);
        return;
    }
    NETMANAGER_BASE_LOGI("socket is %{public}d", socketFd_);
    SetParseOK(true);
}

bool BindSocketContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_JUST_OPTIONS) {
        if (NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_object) {
            return true;
        }
        if (NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_function) {
            SetCallback(params[paramsCount - 1]);
        }
    }

    if (paramsCount == PARAM_OPTIONS_AND_CALLBACK) {
        if (NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_object &&
               NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_1]) == napi_function) {
            return true;
        }
        if (NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_1]) == napi_function) {
            SetCallback(params[paramsCount - 1]);
        }
    }
    return false;
}
} // namespace OHOS::NetManagerStandard
