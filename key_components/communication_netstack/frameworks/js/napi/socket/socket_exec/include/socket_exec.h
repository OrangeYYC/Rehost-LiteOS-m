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

#ifndef COMMUNICATIONNETSTACK_SOCKET_EXEC_H
#define COMMUNICATIONNETSTACK_SOCKET_EXEC_H

#include "bind_context.h"
#include "common_context.h"
#include "connect_context.h"
#include "tcp_extra_context.h"
#include "tcp_send_context.h"
#include "udp_extra_context.h"
#include "udp_send_context.h"

namespace OHOS::NetStack::SocketExec {
int MakeTcpSocket(sa_family_t family);

int MakeUdpSocket(sa_family_t family);

/* async work execute */
bool ExecUdpBind(BindContext *context);

bool ExecUdpSend(UdpSendContext *context);

bool ExecTcpBind(BindContext *context);

bool ExecConnect(ConnectContext *context);

bool ExecTcpSend(TcpSendContext *context);

bool ExecClose(CloseContext *context);

bool ExecGetState(GetStateContext *context);

bool ExecGetRemoteAddress(GetRemoteAddressContext *context);

bool ExecTcpSetExtraOptions(TcpSetExtraOptionsContext *context);

bool ExecUdpSetExtraOptions(UdpSetExtraOptionsContext *context);

/* async work callback */
napi_value BindCallback(BindContext *context);

napi_value UdpSendCallback(UdpSendContext *context);

napi_value ConnectCallback(ConnectContext *context);

napi_value TcpSendCallback(TcpSendContext *context);

napi_value CloseCallback(CloseContext *context);

napi_value GetStateCallback(GetStateContext *context);

napi_value GetRemoteAddressCallback(GetRemoteAddressContext *context);

napi_value TcpSetExtraOptionsCallback(TcpSetExtraOptionsContext *context);

napi_value UdpSetExtraOptionsCallback(UdpSetExtraOptionsContext *context);
} // namespace OHOS::NetStack::SocketExec

#endif /* COMMUNICATIONNETSTACK_SOCKET_EXEC_H */
