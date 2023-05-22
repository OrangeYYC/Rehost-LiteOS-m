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

#include "socket_module.h"

#include <cstdint>
#include <unistd.h>
#include <initializer_list>
#include <new>
#include <utility>

#include "bind_context.h"
#include "common_context.h"
#include "connect_context.h"
#include "context_key.h"
#include "event_list.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "net_address.h"
#include "event_manager.h"
#include "netstack_common_utils.h"
#include "netstack_log.h"
#include "module_template.h"
#include "napi_utils.h"
#include "node_api.h"
#include "socket_async_work.h"
#include "socket_exec.h"
#include "tcp_extra_context.h"
#include "tcp_send_context.h"
#include "tlssocket_module.h"
#include "udp_extra_context.h"
#include "udp_send_context.h"

static constexpr const char *SOCKET_MODULE_NAME = "net.socket";

static const char *UDP_BIND_NAME = "UdpBind";
static const char *UDP_SEND_NAME = "UdpSend";
static const char *UDP_CLOSE_NAME = "UdpClose";
static const char *UDP_GET_STATE = "UdpGetState";
static const char *UDP_SET_EXTRA_OPTIONS_NAME = "UdpSetExtraOptions";

static const char *TCP_BIND_NAME = "TcpBind";
static const char *TCP_CONNECT_NAME = "TcpConnect";
static const char *TCP_SEND_NAME = "TcpSend";
static const char *TCP_CLOSE_NAME = "TcpClose";
static const char *TCP_GET_STATE = "TcpGetState";
static const char *TCP_GET_REMOTE_ADDRESS = "TcpGetRemoteAddress";
static const char *TCP_SET_EXTRA_OPTIONS_NAME = "TcpSetExtraOptions";

static constexpr const char *KEY_SOCKET_FD = "socketFd";

#define SOCKET_INTERFACE(Context, executor, callback, work, name) \
    ModuleTemplate::Interface<Context>(env, info, name, work, SocketAsyncWork::executor, SocketAsyncWork::callback)

namespace OHOS::NetStack {
void Finalize(napi_env, void *data, void *)
{
    NETSTACK_LOGI("socket handle is finalized");
    auto manager = static_cast<EventManager *>(data);
    if (manager != nullptr) {
        manager->SetInvalid();
        int sock = static_cast<int>(reinterpret_cast<uint64_t>(manager->GetData()));
        if (sock != 0) {
            close(sock);
        }
    }
}

static bool SetSocket(napi_env env, napi_value thisVal, BindContext *context, int sock)
{
    if (sock < 0) {
        napi_value error = NapiUtils::CreateObject(env);
        if (NapiUtils::GetValueType(env, error) != napi_object) {
            return false;
        }
        NapiUtils::SetUint32Property(env, error, KEY_ERROR_CODE, errno);
        context->Emit(EVENT_ERROR, std::make_pair(NapiUtils::GetUndefined(env), error));
        return false;
    }

    EventManager *manager = nullptr;
    if (napi_unwrap(env, thisVal, reinterpret_cast<void **>(&manager)) != napi_ok || manager == nullptr) {
        return false;
    }

    manager->SetData(reinterpret_cast<void *>(sock));
    NapiUtils::SetInt32Property(env, thisVal, KEY_SOCKET_FD, sock);
    return true;
}

static bool MakeTcpSocket(napi_env env, napi_value thisVal, BindContext *context)
{
    if (!context->IsParseOK()) {
        return false;
    }
    if (!CommonUtils::HasInternetPermission()) {
        context->SetPermissionDenied(true);
        return false;
    }
    int sock = SocketExec::MakeTcpSocket(context->address_.GetSaFamily());
    if (!SetSocket(env, thisVal, context, sock)) {
        return false;
    }
    context->SetExecOK(true);
    return true;
}

static bool MakeUdpSocket(napi_env env, napi_value thisVal, BindContext *context)
{
    if (!context->IsParseOK()) {
        return false;
    }
    if (!CommonUtils::HasInternetPermission()) {
        context->SetPermissionDenied(true);
        return false;
    }
    int sock = SocketExec::MakeUdpSocket(context->address_.GetSaFamily());
    if (!SetSocket(env, thisVal, context, sock)) {
        return false;
    }
    context->SetExecOK(true);
    return true;
}

napi_value SocketModuleExports::InitSocketModule(napi_env env, napi_value exports)
{
    TLSSocketModuleExports::InitTLSSocketModule(env, exports);
    DefineUDPSocketClass(env, exports);
    DefineTCPSocketClass(env, exports);
    InitSocketProperties(env, exports);

    return exports;
}

napi_value SocketModuleExports::ConstructUDPSocketInstance(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::NewInstance(env, info, INTERFACE_UDP_SOCKET, Finalize);
}

void SocketModuleExports::DefineUDPSocketClass(napi_env env, napi_value exports)
{
    std::initializer_list<napi_property_descriptor> properties = {
        DECLARE_NAPI_FUNCTION(UDPSocket::FUNCTION_BIND, UDPSocket::Bind),
        DECLARE_NAPI_FUNCTION(UDPSocket::FUNCTION_SEND, UDPSocket::Send),
        DECLARE_NAPI_FUNCTION(UDPSocket::FUNCTION_CLOSE, UDPSocket::Close),
        DECLARE_NAPI_FUNCTION(UDPSocket::FUNCTION_GET_STATE, UDPSocket::GetState),
        DECLARE_NAPI_FUNCTION(UDPSocket::FUNCTION_SET_EXTRA_OPTIONS, UDPSocket::SetExtraOptions),
        DECLARE_NAPI_FUNCTION(UDPSocket::FUNCTION_ON, UDPSocket::On),
        DECLARE_NAPI_FUNCTION(UDPSocket::FUNCTION_OFF, UDPSocket::Off),
    };
    ModuleTemplate::DefineClass(env, exports, properties, INTERFACE_UDP_SOCKET);
}

napi_value SocketModuleExports::ConstructTCPSocketInstance(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::NewInstance(env, info, INTERFACE_TCP_SOCKET, Finalize);
}

void SocketModuleExports::DefineTCPSocketClass(napi_env env, napi_value exports)
{
    std::initializer_list<napi_property_descriptor> properties = {
        DECLARE_NAPI_FUNCTION(TCPSocket::FUNCTION_BIND, TCPSocket::Bind),
        DECLARE_NAPI_FUNCTION(TCPSocket::FUNCTION_CONNECT, TCPSocket::Connect),
        DECLARE_NAPI_FUNCTION(TCPSocket::FUNCTION_SEND, TCPSocket::Send),
        DECLARE_NAPI_FUNCTION(TCPSocket::FUNCTION_CLOSE, TCPSocket::Close),
        DECLARE_NAPI_FUNCTION(TCPSocket::FUNCTION_GET_REMOTE_ADDRESS, TCPSocket::GetRemoteAddress),
        DECLARE_NAPI_FUNCTION(TCPSocket::FUNCTION_GET_STATE, TCPSocket::GetState),
        DECLARE_NAPI_FUNCTION(TCPSocket::FUNCTION_SET_EXTRA_OPTIONS, TCPSocket::SetExtraOptions),
        DECLARE_NAPI_FUNCTION(TCPSocket::FUNCTION_ON, TCPSocket::On),
        DECLARE_NAPI_FUNCTION(TCPSocket::FUNCTION_OFF, TCPSocket::Off),
    };
    ModuleTemplate::DefineClass(env, exports, properties, INTERFACE_TCP_SOCKET);
}

void SocketModuleExports::InitSocketProperties(napi_env env, napi_value exports)
{
    std::initializer_list<napi_property_descriptor> properties = {
        DECLARE_NAPI_FUNCTION(FUNCTION_CONSTRUCTOR_UDP_SOCKET_INSTANCE, ConstructUDPSocketInstance),
        DECLARE_NAPI_FUNCTION(FUNCTION_CONSTRUCTOR_TCP_SOCKET_INSTANCE, ConstructTCPSocketInstance),
    };
    NapiUtils::DefineProperties(env, exports, properties);
}

/* udp async works */
napi_value SocketModuleExports::UDPSocket::Bind(napi_env env, napi_callback_info info)
{
    return SOCKET_INTERFACE(BindContext, ExecUdpBind, BindCallback, MakeUdpSocket, UDP_BIND_NAME);
}

napi_value SocketModuleExports::UDPSocket::Send(napi_env env, napi_callback_info info)
{
    return SOCKET_INTERFACE(UdpSendContext, ExecUdpSend, UdpSendCallback, nullptr, UDP_SEND_NAME);
}

napi_value SocketModuleExports::UDPSocket::Close(napi_env env, napi_callback_info info)
{
    return SOCKET_INTERFACE(CloseContext, ExecClose, CloseCallback, nullptr, UDP_CLOSE_NAME);
}

napi_value SocketModuleExports::UDPSocket::GetState(napi_env env, napi_callback_info info)
{
    return SOCKET_INTERFACE(GetStateContext, ExecGetState, GetStateCallback, nullptr, UDP_GET_STATE);
}

napi_value SocketModuleExports::UDPSocket::SetExtraOptions(napi_env env, napi_callback_info info)
{
    return SOCKET_INTERFACE(UdpSetExtraOptionsContext, ExecUdpSetExtraOptions, UdpSetExtraOptionsCallback, nullptr,
                            UDP_SET_EXTRA_OPTIONS_NAME);
}

napi_value SocketModuleExports::UDPSocket::On(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::On(env, info, {EVENT_MESSAGE, EVENT_LISTENING, EVENT_ERROR, EVENT_CLOSE}, false);
}

napi_value SocketModuleExports::UDPSocket::Off(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Off(env, info, {EVENT_MESSAGE, EVENT_LISTENING, EVENT_ERROR, EVENT_CLOSE});
}

/* tcp async works */
napi_value SocketModuleExports::TCPSocket::Bind(napi_env env, napi_callback_info info)
{
    return SOCKET_INTERFACE(BindContext, ExecTcpBind, BindCallback, MakeTcpSocket, TCP_BIND_NAME);
}

napi_value SocketModuleExports::TCPSocket::Connect(napi_env env, napi_callback_info info)
{
    return SOCKET_INTERFACE(ConnectContext, ExecConnect, ConnectCallback, nullptr, TCP_CONNECT_NAME);
}

napi_value SocketModuleExports::TCPSocket::Send(napi_env env, napi_callback_info info)
{
    return SOCKET_INTERFACE(TcpSendContext, ExecTcpSend, TcpSendCallback, nullptr, TCP_SEND_NAME);
}

napi_value SocketModuleExports::TCPSocket::Close(napi_env env, napi_callback_info info)
{
    return SOCKET_INTERFACE(CloseContext, ExecClose, CloseCallback, nullptr, TCP_CLOSE_NAME);
}

napi_value SocketModuleExports::TCPSocket::GetRemoteAddress(napi_env env, napi_callback_info info)
{
    return SOCKET_INTERFACE(GetRemoteAddressContext, ExecGetRemoteAddress, GetRemoteAddressCallback, nullptr,
                            TCP_GET_REMOTE_ADDRESS);
}

napi_value SocketModuleExports::TCPSocket::GetState(napi_env env, napi_callback_info info)
{
    return SOCKET_INTERFACE(GetStateContext, ExecGetState, GetStateCallback, nullptr, TCP_GET_STATE);
}

napi_value SocketModuleExports::TCPSocket::SetExtraOptions(napi_env env, napi_callback_info info)
{
    return SOCKET_INTERFACE(TcpSetExtraOptionsContext, ExecTcpSetExtraOptions, TcpSetExtraOptionsCallback, nullptr,
                            TCP_SET_EXTRA_OPTIONS_NAME);
}

napi_value SocketModuleExports::TCPSocket::On(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::On(env, info, {EVENT_MESSAGE, EVENT_CONNECT, EVENT_ERROR, EVENT_CLOSE}, false);
}

napi_value SocketModuleExports::TCPSocket::Off(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Off(env, info, {EVENT_MESSAGE, EVENT_CONNECT, EVENT_ERROR, EVENT_CLOSE});
}

static napi_module g_socketModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = SocketModuleExports::InitSocketModule,
    .nm_modname = SOCKET_MODULE_NAME,
    .nm_priv = nullptr,
    .reserved = {nullptr},
};
/*
 * Module register function
 */
extern "C" __attribute__((constructor)) void RegisterSocketModule(void)
{
    napi_module_register(&g_socketModule);
}
} // namespace OHOS::NetStack
