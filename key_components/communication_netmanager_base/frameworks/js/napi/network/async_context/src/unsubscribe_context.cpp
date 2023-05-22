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

#include "unsubscribe_context.h"

static constexpr const int PARAM_NO_OPTIONS = 0;

namespace OHOS::NetManagerStandard {
UnsubscribeContext::UnsubscribeContext(napi_env env, EventManager *manager) : BaseContext(env, manager) {}

void UnsubscribeContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (!CheckParamsType(params, paramsCount)) {
        return;
    }

    SetNeedPromise(false);

    SetParseOK(true);
}

bool UnsubscribeContext::CheckParamsType(napi_value *params, size_t paramsCount)
{
    return paramsCount == PARAM_NO_OPTIONS;
}
} // namespace OHOS::NetManagerStandard
