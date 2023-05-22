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

#include "socket_async_work.h"

#include "bind_context.h"
#include "common_context.h"
#include "connect_context.h"
#include "base_async_work.h"
#include "socket_exec.h"
#include "tcp_extra_context.h"
#include "tcp_send_context.h"
#include "udp_extra_context.h"
#include "udp_send_context.h"

namespace OHOS::NetStack {
void SocketAsyncWork::ExecUdpBind(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<BindContext, SocketExec::ExecUdpBind>(env, data);
}

void SocketAsyncWork::ExecUdpSend(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<UdpSendContext, SocketExec::ExecUdpSend>(env, data);
}

void SocketAsyncWork::ExecTcpBind(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<BindContext, SocketExec::ExecTcpBind>(env, data);
}

void SocketAsyncWork::ExecConnect(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<ConnectContext, SocketExec::ExecConnect>(env, data);
}

void SocketAsyncWork::ExecClose(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<CloseContext, SocketExec::ExecClose>(env, data);
}

void SocketAsyncWork::ExecTcpSend(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<TcpSendContext, SocketExec::ExecTcpSend>(env, data);
}

void SocketAsyncWork::ExecGetState(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetStateContext, SocketExec::ExecGetState>(env, data);
}

void SocketAsyncWork::ExecGetRemoteAddress(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetRemoteAddressContext, SocketExec::ExecGetRemoteAddress>(env, data);
}

void SocketAsyncWork::ExecTcpSetExtraOptions(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<TcpSetExtraOptionsContext, SocketExec::ExecTcpSetExtraOptions>(env, data);
}

void SocketAsyncWork::ExecUdpSetExtraOptions(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<UdpSetExtraOptionsContext, SocketExec::ExecUdpSetExtraOptions>(env, data);
}

void SocketAsyncWork::BindCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<BindContext, SocketExec::BindCallback>(env, status, data);
}

void SocketAsyncWork::UdpSendCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<UdpSendContext, SocketExec::UdpSendCallback>(env, status, data);
}

void SocketAsyncWork::ConnectCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<ConnectContext, SocketExec::ConnectCallback>(env, status, data);
}

void SocketAsyncWork::TcpSendCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<TcpSendContext, SocketExec::TcpSendCallback>(env, status, data);
}

void SocketAsyncWork::CloseCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<CloseContext, SocketExec::CloseCallback>(env, status, data);
}

void SocketAsyncWork::GetStateCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetStateContext, SocketExec::GetStateCallback>(env, status, data);
}

void SocketAsyncWork::GetRemoteAddressCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetRemoteAddressContext, SocketExec::GetRemoteAddressCallback>(env, status, data);
}

void SocketAsyncWork::TcpSetExtraOptionsCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<TcpSetExtraOptionsContext, SocketExec::TcpSetExtraOptionsCallback>(env, status,
                                                                                                        data);
}

void SocketAsyncWork::UdpSetExtraOptionsCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<UdpSetExtraOptionsContext, SocketExec::UdpSetExtraOptionsCallback>(env, status,
                                                                                                        data);
}
} // namespace OHOS::NetStack
