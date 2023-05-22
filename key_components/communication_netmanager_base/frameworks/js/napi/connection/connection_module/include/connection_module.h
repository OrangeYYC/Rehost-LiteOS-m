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

#ifndef COMMUNICATIONNETMANAGERBASE_CONNECTION_MODULE_H
#define COMMUNICATIONNETMANAGERBASE_CONNECTION_MODULE_H

#include <napi/native_api.h>

namespace OHOS::NetManagerStandard {
class ConnectionModule final {
public:
    static constexpr const char *FUNCTION_GET_DEFAULT_NET = "getDefaultNet";
    static constexpr const char *FUNCTION_GET_DEFAULT_NET_SYNC = "getDefaultNetSync";
    static constexpr const char *FUNCTION_HAS_DEFAULT_NET = "hasDefaultNet";
    static constexpr const char *FUNCTION_IS_DEFAULT_NET_METERED = "isDefaultNetMetered";
    static constexpr const char *FUNCTION_GET_NET_CAPABILITIES = "getNetCapabilities";
    static constexpr const char *FUNCTION_GET_CONNECTION_PROPERTIES = "getConnectionProperties";
    static constexpr const char *FUNCTION_CREATE_NET_CONNECTION = "createNetConnection";
    static constexpr const char *FUNCTION_GET_ADDRESSES_BY_NAME = "getAddressesByName";
    static constexpr const char *FUNCTION_GET_ALL_NETS = "getAllNets";
    static constexpr const char *FUNCTION_ENABLE_AIRPLANE_MODE = "enableAirplaneMode";
    static constexpr const char *FUNCTION_DISABLE_AIRPLANE_MODE = "disableAirplaneMode";
    static constexpr const char *FUNCTION_REPORT_NET_CONNECTED = "reportNetConnected";
    static constexpr const char *FUNCTION_REPORT_NET_DISCONNECTED = "reportNetDisconnected";
    static constexpr const char *FUNCTION_GET_GLOBAL_HTTP_PROXY = "getGlobalHttpProxy";
    static constexpr const char *FUNCTION_SET_GLOBAL_HTTP_PROXY = "setGlobalHttpProxy";
    static constexpr const char *FUNCTION_GET_APP_NET = "getAppNet";
    static constexpr const char *FUNCTION_SET_APP_NET = "setAppNet";
    static constexpr const char *INTERFACE_NET_CONNECTION = "NetConnection";
    static constexpr const char *INTERFACE_NET_CAP = "NetCap";
    static constexpr const char *INTERFACE_NET_BEAR_TYPE = "NetBearType";

    static napi_value InitConnectionModule(napi_env env, napi_value exports);

    class NetHandleInterface final {
    public:
        static constexpr const char *PROPERTY_NET_ID = "netId";
        static constexpr const char *FUNCTION_GET_ADDRESSES_BY_NAME = "getAddressesByName";
        static constexpr const char *FUNCTION_GET_ADDRESS_BY_NAME = "getAddressByName";
        static constexpr const char *FUNCTION_BIND_SOCKET = "bindSocket";

        static napi_value GetAddressesByName(napi_env env, napi_callback_info info);
        static napi_value GetAddressByName(napi_env env, napi_callback_info info);
        static napi_value BindSocket(napi_env env, napi_callback_info info);
    };

    class NetConnectionInterface final {
    public:
        static constexpr const char *FUNCTION_ON = "on";
        static constexpr const char *FUNCTION_REGISTER = "register";
        static constexpr const char *FUNCTION_UNREGISTER = "unregister";

        static napi_value On(napi_env env, napi_callback_info info);
        static napi_value Register(napi_env env, napi_callback_info info);
        static napi_value Unregister(napi_env env, napi_callback_info info);
    };

private:
    static void InitProperties(napi_env env, napi_value exports);

    static napi_value GetDefaultNet(napi_env env, napi_callback_info info);
    static napi_value GetDefaultNetSync(napi_env env, napi_callback_info info);
    static napi_value CreateNetConnection(napi_env env, napi_callback_info info);
    static napi_value GetAddressesByName(napi_env env, napi_callback_info info);
    static napi_value HasDefaultNet(napi_env env, napi_callback_info info);
    static napi_value IsDefaultNetMetered(napi_env env, napi_callback_info info);
    static napi_value GetNetCapabilities(napi_env env, napi_callback_info info);
    static napi_value GetConnectionProperties(napi_env env, napi_callback_info info);
    static napi_value GetAllNets(napi_env env, napi_callback_info info);
    static napi_value EnableAirplaneMode(napi_env env, napi_callback_info info);
    static napi_value DisableAirplaneMode(napi_env env, napi_callback_info info);
    static napi_value ReportNetConnected(napi_env env, napi_callback_info info);
    static napi_value ReportNetDisconnected(napi_env env, napi_callback_info info);
    static napi_value GetGlobalHttpProxy(napi_env env, napi_callback_info info);
    static napi_value SetGlobalHttpProxy(napi_env env, napi_callback_info info);
    static napi_value GetAppNet(napi_env env, napi_callback_info info);
    static napi_value SetAppNet(napi_env env, napi_callback_info info);
};
} // namespace OHOS::NetManagerStandard

#endif /* COMMUNICATIONNETMANAGERBASE_CONNECTION_MODULE_H */
