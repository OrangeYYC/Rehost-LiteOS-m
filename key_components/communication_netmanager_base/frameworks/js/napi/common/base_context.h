/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef BASE_CONTEXT_H
#define BASE_CONTEXT_H

#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace NetManagerStandard {
const int32_t ERROR_DEFAULT = -1;
struct BaseContext {
    napi_async_work work = nullptr;
    napi_deferred deferred = nullptr;
    napi_ref callbackRef = nullptr;
    int32_t errorCode = ERROR_DEFAULT;
    bool resolved = false;
};

struct BooleanValueContext : BaseContext {
    bool value;
};

struct CallbackContext {
    napi_env env = nullptr;
    napi_ref callbackRef = nullptr;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // BASE_CONTEXT_H
