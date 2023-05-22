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

#include "none_params_context.h"

#include "napi_constant.h"
#include "napi_utils.h"

namespace OHOS::NetManagerStandard {
NoneParamsContext::NoneParamsContext(napi_env env, EventManager *manager) : BaseContext(env, manager) {}

void NoneParamsContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_NONE) {
        SetNeedPromise(true);
        SetParseOK(true);
        return;
    }
    if (paramsCount == PARAM_JUST_CALLBACK) {
        if (NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_function) {
            SetParseOK(SetCallback(params[ARG_INDEX_0]) == napi_ok);
            SetNeedPromise(false);
            return;
        }
        SetNeedPromise(true);
        SetParseOK(true);
        return;
    }
    SetParseOK(false);
    SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
}
} // namespace OHOS::NetManagerStandard
