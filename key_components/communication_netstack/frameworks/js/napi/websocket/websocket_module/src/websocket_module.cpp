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

#include "websocket_module.h"

#include "constant.h"
#include "netstack_log.h"
#include "module_template.h"
#include "websocket_async_work.h"

namespace OHOS::NetStack {
napi_value WebSocketModule::InitWebSocketModule(napi_env env, napi_value exports)
{
    DefineWebSocketClass(env, exports);
    InitWebSocketProperties(env, exports);

    return exports;
}

napi_value WebSocketModule::CreateWebSocket(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::NewInstance(env, info, INTERFACE_WEB_SOCKET, FinalizeWebSocketInstance);
}

void WebSocketModule::DefineWebSocketClass(napi_env env, napi_value exports)
{
    std::initializer_list<napi_property_descriptor> properties = {
        DECLARE_NAPI_FUNCTION(WebSocket::FUNCTION_CONNECT, WebSocket::Connect),
        DECLARE_NAPI_FUNCTION(WebSocket::FUNCTION_SEND, WebSocket::Send),
        DECLARE_NAPI_FUNCTION(WebSocket::FUNCTION_CLOSE, WebSocket::Close),
        DECLARE_NAPI_FUNCTION(WebSocket::FUNCTION_ON, WebSocket::On),
        DECLARE_NAPI_FUNCTION(WebSocket::FUNCTION_OFF, WebSocket::Off),
    };
    ModuleTemplate::DefineClass(env, exports, properties, INTERFACE_WEB_SOCKET);
}

void WebSocketModule::InitWebSocketProperties(napi_env env, napi_value exports)
{
    std::initializer_list<napi_property_descriptor> properties = {
        DECLARE_NAPI_FUNCTION(FUNCTION_CREATE_WEB_SOCKET, CreateWebSocket),
    };
    NapiUtils::DefineProperties(env, exports, properties);
}

void WebSocketModule::FinalizeWebSocketInstance(napi_env env, void *data, void *hint)
{
    NETSTACK_LOGI("websocket handle is finalized");
    auto manager = reinterpret_cast<EventManager *>(data);
    if (manager != nullptr) {
        manager->SetInvalid();
    }
}

napi_value WebSocketModule::WebSocket::Connect(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<ConnectContext>(
        env, info, "WebSocketConnect", nullptr, WebSocketAsyncWork::ExecConnect, WebSocketAsyncWork::ConnectCallback);
}

napi_value WebSocketModule::WebSocket::Send(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<SendContext>(env, info, "WebSocketSend", nullptr, WebSocketAsyncWork::ExecSend,
                                                  WebSocketAsyncWork::SendCallback);
}

napi_value WebSocketModule::WebSocket::Close(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Interface<CloseContext>(env, info, "WebSocketClose", nullptr, WebSocketAsyncWork::ExecClose,
                                                   WebSocketAsyncWork::CloseCallback);
}

napi_value WebSocketModule::WebSocket::On(napi_env env, napi_callback_info info)
{
    ModuleTemplate::On(env, info, {EventName::EVENT_OPEN, EventName::EVENT_MESSAGE, EventName::EVENT_CLOSE}, true);
    return ModuleTemplate::On(env, info, {EventName::EVENT_ERROR}, false);
}

napi_value WebSocketModule::WebSocket::Off(napi_env env, napi_callback_info info)
{
    return ModuleTemplate::Off(
        env, info, {EventName::EVENT_OPEN, EventName::EVENT_MESSAGE, EventName::EVENT_CLOSE, EventName::EVENT_ERROR});
}

static napi_module g_websocketModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = WebSocketModule::InitWebSocketModule,
    .nm_modname = "net.webSocket",
    .nm_priv = nullptr,
    .reserved = {nullptr},
};

extern "C" __attribute__((constructor)) void RegisterWebSocketModule(void)
{
    napi_module_register(&g_websocketModule);
}
} // namespace OHOS::NetStack
