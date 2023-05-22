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

#ifndef COMMUNICATIONNETSTACK_FETCH_MODULE_H
#define COMMUNICATIONNETSTACK_FETCH_MODULE_H

#include "napi_utils.h"
#include "nocopyable.h"

namespace OHOS::NetStack {
class FetchModule {
public:
    static constexpr const char *FUNCTION_FETCH = "fetch";

    static napi_value InitFetchModule(napi_env env, napi_value exports);

private:
    static napi_value Fetch(napi_env env, napi_callback_info info);
};
} // namespace OHOS::NetStack
#endif /* COMMUNICATIONNETSTACK_FETCH_MODULE_H */
