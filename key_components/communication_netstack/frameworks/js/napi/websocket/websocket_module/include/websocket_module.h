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

#ifndef COMMUNICATIONNETSTACK_WEBSOCKET_MODULE_H
#define COMMUNICATIONNETSTACK_WEBSOCKET_MODULE_H

#include "napi/native_api.h"

namespace OHOS::NetStack {
class WebSocketModule final {
public:
    class WebSocket {
    public:
        static constexpr const char *FUNCTION_CONNECT = "connect";
        static constexpr const char *FUNCTION_SEND = "send";
        static constexpr const char *FUNCTION_CLOSE = "close";
        static constexpr const char *FUNCTION_ON = "on";
        static constexpr const char *FUNCTION_OFF = "off";

        static napi_value Connect(napi_env env, napi_callback_info info);

        static napi_value Send(napi_env env, napi_callback_info info);

        static napi_value Close(napi_env env, napi_callback_info info);

        static napi_value On(napi_env env, napi_callback_info info);

        static napi_value Off(napi_env env, napi_callback_info info);
    };

    static constexpr const char *FUNCTION_CREATE_WEB_SOCKET = "createWebSocket";
    static constexpr const char *INTERFACE_WEB_SOCKET = "WebSocket";

    static napi_value InitWebSocketModule(napi_env env, napi_value exports);

private:
    static napi_value CreateWebSocket(napi_env env, napi_callback_info info);

    static void DefineWebSocketClass(napi_env env, napi_value exports);

    static void FinalizeWebSocketInstance(napi_env env, void *data, void *hint);

    static void InitWebSocketProperties(napi_env env, napi_value exports);
};
} // namespace OHOS::NetStack
#endif /* COMMUNICATIONNETSTACK_WEBSOCKET_MODULE_H */
