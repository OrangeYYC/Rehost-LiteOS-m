/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "http_async_work.h"

#include "http_exec.h"
#include "base_async_work.h"
#include "napi_utils.h"

namespace OHOS::NetStack {
void HttpAsyncWork::ExecRequest(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<RequestContext, HttpExec::ExecRequest>(env, data);
}

void HttpAsyncWork::RequestCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<RequestContext, HttpExec::RequestCallback>(env, status, data);
}

void HttpAsyncWork::Request2Callback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<RequestContext, HttpExec::Request2Callback>(env, status, data);
}

void HttpAsyncWork::ExecFlush(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<BaseContext, HttpResponseCacheExec::ExecFlush>(env, data);
}

void HttpAsyncWork::FlushCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<BaseContext, HttpResponseCacheExec::FlushCallback>(env, status, data);
}

void HttpAsyncWork::ExecDelete(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<BaseContext, HttpResponseCacheExec::ExecDelete>(env, data);
}

void HttpAsyncWork::DeleteCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<BaseContext, HttpResponseCacheExec::DeleteCallback>(env, status, data);
}
} // namespace OHOS::NetStack
