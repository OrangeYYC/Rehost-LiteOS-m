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

#include "get_uid_rxbytes_context.h"

#include "constant.h"
#include "napi_constant.h"
#include "napi_utils.h"
#include "net_manager_constants.h"

namespace OHOS {
namespace NetManagerStandard {
GetUidRxBytesContext::GetUidRxBytesContext(napi_env env, EventManager *manager) : BaseContext(env, manager) {}

void GetUidRxBytesContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(params, paramsCount)) {
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        SetNeedThrowException(true);
        return;
    }
    uid_ = NapiUtils::GetInt32FromValue(GetEnv(), params[ARG_INDEX_0]);
    if (paramsCount == PARAM_OPTIONS_AND_CALLBACK) {
        SetParseOK(SetCallback(params[ARG_INDEX_1]) == napi_ok);
        return;
    }
    SetParseOK(true);
}

bool GetUidRxBytesContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_JUST_OPTIONS) {
        return NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_number;
    }
    if (paramsCount == PARAM_OPTIONS_AND_CALLBACK) {
        return NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_1]) == napi_function &&
               NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_number;
    }
    return false;
}
} // namespace NetManagerStandard
} // namespace OHOS
