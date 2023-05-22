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

#include "update_remind_policy_context.h"

#include "constant.h"
#include "napi_constant.h"
#include "napi_utils.h"
#include "netmanager_base_log.h"

namespace OHOS {
namespace NetManagerStandard {
UpdateRemindPolicyContext::UpdateRemindPolicyContext(napi_env env, EventManager *manager) : BaseContext(env, manager) {}

void UpdateRemindPolicyContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(params, paramsCount)) {
        NETMANAGER_BASE_LOGE("Check params failed");
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        SetNeedThrowException(true);
        return;
    }
    netType_ = NapiUtils::GetInt32FromValue(GetEnv(), params[ARG_INDEX_0]);
    iccid_ = NapiUtils::GetStringFromValueUtf8(GetEnv(), params[ARG_INDEX_1]);
    remindType_ = NapiUtils::GetInt32FromValue(GetEnv(), params[ARG_INDEX_2]);
    if (paramsCount == PARAM_TRIPLE_OPTIONS_AND_CALLBACK) {
        SetParseOK(SetCallback(params[ARG_INDEX_3]) == napi_ok);
        return;
    }
    SetParseOK(true);
}

bool UpdateRemindPolicyContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_TRIPLE_OPTIONS) {
        return NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_number &&
               NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_1]) == napi_string &&
               NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_2]) == napi_number;
    }

    if (paramsCount == PARAM_TRIPLE_OPTIONS_AND_CALLBACK) {
        return NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_number &&
               NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_1]) == napi_string &&
               NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_2]) == napi_number &&
               NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_3]) == napi_function;
        ;
    }
    return false;
}
} // namespace NetManagerStandard
} // namespace OHOS
