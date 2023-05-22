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

#include "constant.h"
#include "mdns_startsearching_context.h"
#include "netmanager_ext_log.h"
#include "mdns_common.h"

namespace OHOS::NetManagerStandard {
MDnsStartSearchingContext::MDnsStartSearchingContext(napi_env env, EventManager *manager) : BaseContext(env, manager)
{
}

void MDnsStartSearchingContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (paramsCount > PARAM_NONE) {
        SetNeedThrowException(true);
        SetErrorCode(NET_MDNS_ERR_ILLEGAL_ARGUMENT);
        return;
    }
    SetParseOK(true);
}

bool MDnsStartSearchingContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_JUST_OPTIONS) {
        return NapiUtils::GetValueType(GetEnv(), params[0]) == napi_function;
    }

    if (paramsCount == PARAM_OPTIONS_AND_CALLBACK) {
        return NapiUtils::GetValueType(GetEnv(), params[0]) == napi_object &&
               NapiUtils::GetValueType(GetEnv(), params[1]) == napi_function;
    }
    return false;
}
} // namespace OHOS::NetManagerStandard
