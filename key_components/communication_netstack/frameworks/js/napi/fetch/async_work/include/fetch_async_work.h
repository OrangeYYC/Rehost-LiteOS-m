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

#ifndef COMMUNICATIONNETSTACK_FETCH_ASYNC_WORK_H
#define COMMUNICATIONNETSTACK_FETCH_ASYNC_WORK_H

#include "napi/native_api.h"
#include "napi/native_common.h"
#include "nocopyable.h"
#include "fetch_context.h"

namespace OHOS::NetStack {
class FetchAsyncWork final {
public:
    DISALLOW_COPY_AND_MOVE(FetchAsyncWork);

    static void ExecFetch(napi_env env, void *data);

    static void FetchCallback(napi_env env, napi_status status, void *data);
};
} // namespace OHOS::NetStack

#endif /* COMMUNICATIONNETSTACK_FETCH_ASYNC_WORK_H */
