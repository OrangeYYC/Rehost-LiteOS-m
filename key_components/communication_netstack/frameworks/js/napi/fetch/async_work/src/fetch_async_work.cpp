/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     fetch://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "fetch_async_work.h"

#include "fetch_exec.h"
#include "base_async_work.h"

namespace OHOS::NetStack {
void FetchAsyncWork::ExecFetch(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<FetchContext, FetchExec::ExecFetch>(env, data);
}

void FetchAsyncWork::FetchCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallbackForSystem<FetchContext, FetchExec::FetchCallback>(env, status, data);
}
} // namespace OHOS::NetStack
