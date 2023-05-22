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

#include "set_net_quota_policies_context.h"

#include <cstdint>

#include "constant.h"
#include "napi_constant.h"
#include "napi_utils.h"
#include "netmanager_base_log.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
NetQuotaPolicy ReadQuotaPolicy(napi_env env, napi_value value)
{
    NetQuotaPolicy data;
    data.netType = NapiUtils::GetInt32Property(env, value, "netType");
    data.iccid = NapiUtils::GetStringPropertyUtf8(env, value, "iccid");
    data.ident = NapiUtils::GetStringPropertyUtf8(env, value, "ident");
    data.periodDuration = NapiUtils::GetStringPropertyUtf8(env, value, "periodDuration");
    data.warningBytes = NapiUtils::GetInt64Property(env, value, "warningBytes");
    data.limitBytes = NapiUtils::GetInt64Property(env, value, "limitBytes");
    data.lastWarningRemind = NapiUtils::GetInt64Property(env, value, "lastWarningRemind");
    data.lastLimitRemind = NapiUtils::GetInt64Property(env, value, "lastLimitRemind");
    data.metered = NapiUtils::GetBooleanProperty(env, value, "metered");
    data.limitAction = NapiUtils::GetInt32Property(env, value, "limitAction");
    return data;
}
} // namespace
SetNetQuotaPoliciesContext::SetNetQuotaPoliciesContext(napi_env env, EventManager *manager) : BaseContext(env, manager)
{
}

void SetNetQuotaPoliciesContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(params, paramsCount)) {
        NETMANAGER_BASE_LOGE("Check params failed");
        SetErrorCode(NETMANAGER_ERR_PARAMETER_ERROR);
        SetNeedThrowException(true);
        return;
    }

    uint32_t arrayLength = NapiUtils::GetArrayLength(GetEnv(), params[ARG_INDEX_0]);
    arrayLength = arrayLength > ARRAY_LIMIT ? ARRAY_LIMIT : arrayLength;
    napi_value elementValue = nullptr;
    for (uint32_t i = 0; i < arrayLength; i++) {
        elementValue = NapiUtils::GetArrayElement(GetEnv(), params[ARG_INDEX_0], i);
        quotaPolicys_.push_back(ReadQuotaPolicy(GetEnv(), elementValue));
    }

    if (paramsCount == PARAM_OPTIONS_AND_CALLBACK) {
        SetParseOK(SetCallback(params[ARG_INDEX_1]) == napi_ok);
        return;
    }
    SetParseOK(true);
}

bool SetNetQuotaPoliciesContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_JUST_OPTIONS) {
        return NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_object;
    }

    if (paramsCount == PARAM_OPTIONS_AND_CALLBACK) {
        return NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_0]) == napi_object &&
               NapiUtils::GetValueType(GetEnv(), params[ARG_INDEX_1]) == napi_function;
    }
    return false;
}
} // namespace NetManagerStandard
} // namespace OHOS
