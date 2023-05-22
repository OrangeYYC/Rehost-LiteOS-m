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

#include "websocket_async_work.h"

#include "base_async_work.h"

namespace OHOS::NetStack {
void WebSocketAsyncWork::ExecConnect(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<ConnectContext, WebSocketExec::ExecConnect>(env, data);
}

void WebSocketAsyncWork::ExecSend(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<SendContext, WebSocketExec::ExecSend>(env, data);
}

void WebSocketAsyncWork::ExecClose(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<CloseContext, WebSocketExec::ExecClose>(env, data);
}

void WebSocketAsyncWork::ConnectCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<ConnectContext, WebSocketExec::ConnectCallback>(env, status, data);
}

void WebSocketAsyncWork::SendCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<SendContext, WebSocketExec::SendCallback>(env, status, data);
}

void WebSocketAsyncWork::CloseCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<CloseContext, WebSocketExec::CloseCallback>(env, status, data);
}
} // namespace OHOS::NetStack
