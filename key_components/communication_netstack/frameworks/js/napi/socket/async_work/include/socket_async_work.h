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

#ifndef COMMUNICATIONNETSTACK_SOCKET_ASYNC_WORK_H
#define COMMUNICATIONNETSTACK_SOCKET_ASYNC_WORK_H

#include "napi/native_api.h"
#include "nocopyable.h"

namespace OHOS::NetStack {
class SocketAsyncWork final {
public:
    DISALLOW_COPY_AND_MOVE(SocketAsyncWork);

    /* executor */
    static void ExecUdpBind(napi_env env, void *data);

    static void ExecUdpSend(napi_env env, void *data);

    static void ExecTcpBind(napi_env env, void *data);

    static void ExecConnect(napi_env env, void *data);

    static void ExecTcpSend(napi_env env, void *data);

    static void ExecClose(napi_env env, void *data);

    static void ExecGetState(napi_env env, void *data);

    static void ExecGetRemoteAddress(napi_env env, void *data);

    static void ExecTcpSetExtraOptions(napi_env env, void *data);

    static void ExecUdpSetExtraOptions(napi_env env, void *data);

    /* callback */
    static void BindCallback(napi_env env, napi_status status, void *data);

    static void UdpSendCallback(napi_env env, napi_status status, void *data);

    static void ConnectCallback(napi_env env, napi_status status, void *data);

    static void TcpSendCallback(napi_env env, napi_status status, void *data);

    static void CloseCallback(napi_env env, napi_status status, void *data);

    static void GetStateCallback(napi_env env, napi_status status, void *data);

    static void GetRemoteAddressCallback(napi_env env, napi_status status, void *data);

    static void TcpSetExtraOptionsCallback(napi_env env, napi_status status, void *data);

    static void UdpSetExtraOptionsCallback(napi_env env, napi_status status, void *data);
};
} // namespace OHOS::NetStack

#endif /* COMMUNICATIONNETSTACK_SOCKET_ASYNC_WORK_H */
