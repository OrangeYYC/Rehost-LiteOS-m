/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "get_all_active_ifaces_context.h"

#include "constant.h"

namespace OHOS {
namespace NetManagerStandard {
GetAllActiveIfacesContext::GetAllActiveIfacesContext(napi_env env, EventManager *manager) : BaseContext(env, manager) {}

void GetAllActiveIfacesContext::ParseParams(napi_value *params, size_t paramsCount)
{
    if (paramsCount == PARAM_JUST_CALLBACK) {
        SetParseOK(SetCallback(params[0]) == napi_ok);
        return;
    }
    SetParseOK(true);
}
} // namespace NetManagerStandard
} // namespace OHOS
