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

#ifndef COMMUNICATIONNETMANAGERBASE_CONNECTION_EXEC_H
#define COMMUNICATIONNETMANAGERBASE_CONNECTION_EXEC_H

#include <netdb.h>

#include "bindsocket_context.h"
#include "getaddressbyname_context.h"
#include "getappnet_context.h"
#include "getdefaultnet_context.h"
#include "getglobalhttpproxy_context.h"
#include "napi/native_api.h"
#include "none_params_context.h"
#include "parse_nethandle_context.h"
#include "register_context.h"
#include "setappnet_context.h"
#include "setglobalhttpproxy_context.h"

namespace OHOS::NetManagerStandard {
static constexpr size_t MAX_ARRAY_LENGTH = 64;
static constexpr size_t MAX_IPV4_STR_LEN = 16;
static constexpr size_t MAX_IPV6_STR_LEN = 64;
class ConnectionExec final {
public:
    ConnectionExec() = delete;

    ~ConnectionExec() = delete;

    static napi_value CreateNetHandle(napi_env env, NetHandle *handle);

    static napi_value CreateNetCapabilities(napi_env env, NetAllCapabilities *capabilities);

    static napi_value CreateConnectionProperties(napi_env env, NetLinkInfo *linkInfo);

    static bool ExecGetDefaultNet(GetDefaultNetContext *context);

    static napi_value GetDefaultNetCallback(GetDefaultNetContext *context);

    static bool ExecHasDefaultNet(HasDefaultNetContext *context);

    static napi_value HasDefaultNetCallback(HasDefaultNetContext *context);

    static bool ExecIsDefaultNetMetered(IsDefaultNetMeteredContext *context);

    static napi_value IsDefaultNetMeteredCallback(IsDefaultNetMeteredContext *context);

    static bool ExecGetNetCapabilities(GetNetCapabilitiesContext *context);

    static napi_value GetNetCapabilitiesCallback(GetNetCapabilitiesContext *context);

    static bool ExecGetConnectionProperties(GetConnectionPropertiesContext *context);

    static napi_value GetConnectionPropertiesCallback(GetConnectionPropertiesContext *context);

    static bool ExecGetAddressByName(GetAddressByNameContext *context);

    static napi_value GetAddressByNameCallback(GetAddressByNameContext *context);

    static bool ExecGetAllNets(GetAllNetsContext *context);

    static napi_value GetAllNetsCallback(GetAllNetsContext *context);

    static bool ExecEnableAirplaneMode(EnableAirplaneModeContext *context);

    static napi_value EnableAirplaneModeCallback(EnableAirplaneModeContext *context);

    static bool ExecDisableAirplaneMode(DisableAirplaneModeContext *context);

    static napi_value DisableAirplaneModeCallback(DisableAirplaneModeContext *context);

    static bool ExecReportNetConnected(ReportNetConnectedContext *context);

    static napi_value ReportNetConnectedCallback(ReportNetConnectedContext *context);

    static bool ExecReportNetDisconnected(ReportNetDisconnectedContext *context);

    static napi_value ReportNetDisconnectedCallback(ReportNetDisconnectedContext *context);

    static bool ExecGetGlobalHttpProxy(GetGlobalHttpProxyContext *context);

    static napi_value GetGlobalHttpProxyCallback(GetGlobalHttpProxyContext *context);

    static bool ExecSetGlobalHttpProxy(SetGlobalHttpProxyContext *context);

    static napi_value SetGlobalHttpProxyCallback(SetGlobalHttpProxyContext *context);

    static bool ExecGetAppNet(GetAppNetContext *context);

    static napi_value GetAppNetCallback(GetAppNetContext *context);

    static bool ExecSetAppNet(SetAppNetContext *context);

    static napi_value SetAppNetCallback(SetAppNetContext *context);

    class NetHandleExec final {
    public:
        NetHandleExec() = delete;

        ~NetHandleExec() = delete;

        static bool ExecGetAddressByName(GetAddressByNameContext *context);

        static napi_value GetAddressByNameCallback(GetAddressByNameContext *context);

        static bool ExecGetAddressesByName(GetAddressByNameContext *context);

        static napi_value GetAddressesByNameCallback(GetAddressByNameContext *context);

        static bool ExecBindSocket(BindSocketContext *context);

        static napi_value BindSocketCallback(BindSocketContext *context);

    private:
        static napi_value MakeNetAddressJsValue(napi_env env, const NetAddress &address);

        static void SetAddressInfo(const char *host, addrinfo *info, NetAddress &address);
    };

    class NetConnectionExec final {
    public:
        NetConnectionExec() = delete;

        ~NetConnectionExec() = delete;

        static bool ExecRegister(RegisterContext *context);

        static napi_value RegisterCallback(RegisterContext *context);

        static bool ExecUnregister(UnregisterContext *context);

        static napi_value UnregisterCallback(UnregisterContext *context);
    };

private:
    static void FillLinkAddress(napi_env env, napi_value connectionProperties, NetLinkInfo *linkInfo);

    static void FillRouoteList(napi_env env, napi_value connectionProperties, NetLinkInfo *linkInfo);

    static void FillDns(napi_env env, napi_value connectionProperties, NetLinkInfo *linkInfo);
};
} // namespace OHOS::NetManagerStandard

#endif /* COMMUNICATIONNETMANAGERBASE_CONNECTION_EXEC_H */
