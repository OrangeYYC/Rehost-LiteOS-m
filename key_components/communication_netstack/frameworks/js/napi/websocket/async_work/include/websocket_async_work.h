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

#ifndef COMMUNICATIONNETSTACK_WEBSOCKET_ASYNC_WORK_H
#define COMMUNICATIONNETSTACK_WEBSOCKET_ASYNC_WORK_H

#include "websocket_exec.h"

namespace OHOS::NetStack {
class WebSocketAsyncWork final {
public:
    DISALLOW_COPY_AND_MOVE(WebSocketAsyncWork);

    /* executor */
    static void ExecConnect(napi_env env, void *data);

    static void ExecSend(napi_env env, void *data);

    static void ExecClose(napi_env env, void *data);

    /* callback */
    static void ConnectCallback(napi_env env, napi_status status, void *data);

    static void SendCallback(napi_env env, napi_status status, void *data);

    static void CloseCallback(napi_env env, napi_status status, void *data);
};
} // namespace OHOS::NetStack
#endif /* COMMUNICATIONNETSTACK_WEBSOCKET_ASYNC_WORK_H */
