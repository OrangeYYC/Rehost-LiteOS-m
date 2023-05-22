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

#ifndef NONE_PARAMS_CONTEXT_H
#define NONE_PARAMS_CONTEXT_H

#include <cstddef>

#include <napi/native_api.h>

#include "base_context.h"
#include "event_manager.h"

namespace OHOS::NetManagerStandard {
class NoneParamsContext final : public BaseContext {
public:
    NoneParamsContext() = delete;
    explicit NoneParamsContext(napi_env env, EventManager *manager);

public:
    void ParseParams(napi_value *params, size_t paramsCount);

public:
    bool hasDefaultNet_ = false;
    bool isMetered_ = false;
};

using HasDefaultNetContext = NoneParamsContext;
using IsDefaultNetMeteredContext = NoneParamsContext;
} // namespace OHOS::NetManagerStandard
#endif // NONE_PARAMS_CONTEXT_H
