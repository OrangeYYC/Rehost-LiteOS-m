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

#ifndef COMMUNICATIONNETSTACK_SOCKET_MODULE_H
#define COMMUNICATIONNETSTACK_SOCKET_MODULE_H

#include "napi/native_api.h"

namespace OHOS::NetStack {
class SocketModuleExports {
public:
    class UDPSocket {
    public:
        static constexpr const char *FUNCTION_BIND = "bind";
        static constexpr const char *FUNCTION_SEND = "send";
        static constexpr const char *FUNCTION_CLOSE = "close";
        static constexpr const char *FUNCTION_GET_STATE = "getState";
        static constexpr const char *FUNCTION_SET_EXTRA_OPTIONS = "setExtraOptions";
        static constexpr const char *FUNCTION_ON = "on";
        static constexpr const char *FUNCTION_OFF = "off";

        static napi_value Bind(napi_env env, napi_callback_info info);
        static napi_value Send(napi_env env, napi_callback_info info);
        static napi_value Close(napi_env env, napi_callback_info info);
        static napi_value GetState(napi_env env, napi_callback_info info);
        static napi_value SetExtraOptions(napi_env env, napi_callback_info info);
        static napi_value On(napi_env env, napi_callback_info info);
        static napi_value Off(napi_env env, napi_callback_info info);
    };

    class TCPSocket {
    public:
        static constexpr const char *FUNCTION_BIND = "bind";
        static constexpr const char *FUNCTION_CONNECT = "connect";
        static constexpr const char *FUNCTION_SEND = "send";
        static constexpr const char *FUNCTION_CLOSE = "close";
        static constexpr const char *FUNCTION_GET_REMOTE_ADDRESS = "getRemoteAddress";
        static constexpr const char *FUNCTION_GET_STATE = "getState";
        static constexpr const char *FUNCTION_SET_EXTRA_OPTIONS = "setExtraOptions";
        static constexpr const char *FUNCTION_ON = "on";
        static constexpr const char *FUNCTION_OFF = "off";

        static napi_value Bind(napi_env env, napi_callback_info info);
        static napi_value Connect(napi_env env, napi_callback_info info);
        static napi_value Send(napi_env env, napi_callback_info info);
        static napi_value Close(napi_env env, napi_callback_info info);
        static napi_value GetRemoteAddress(napi_env env, napi_callback_info info);
        static napi_value GetState(napi_env env, napi_callback_info info);
        static napi_value SetExtraOptions(napi_env env, napi_callback_info info);
        static napi_value On(napi_env env, napi_callback_info info);
        static napi_value Off(napi_env env, napi_callback_info info);
    };

    static constexpr const char *FUNCTION_CONSTRUCTOR_UDP_SOCKET_INSTANCE = "constructUDPSocketInstance";
    static constexpr const char *FUNCTION_CONSTRUCTOR_TCP_SOCKET_INSTANCE = "constructTCPSocketInstance";
    static constexpr const char *INTERFACE_UDP_SOCKET = "UDPSocket";
    static constexpr const char *INTERFACE_TCP_SOCKET = "TCPSocket";

    static napi_value InitSocketModule(napi_env env, napi_value exports);

private:
    static napi_value ConstructUDPSocketInstance(napi_env env, napi_callback_info info);

    static napi_value ConstructTCPSocketInstance(napi_env env, napi_callback_info info);

    static void DefineUDPSocketClass(napi_env env, napi_value exports);

    static void DefineTCPSocketClass(napi_env env, napi_value exports);

    static void InitSocketProperties(napi_env env, napi_value exports);
};
} // namespace OHOS::NetStack
#endif // COMMUNICATIONNETSTACK_SOCKET_MODULE_H