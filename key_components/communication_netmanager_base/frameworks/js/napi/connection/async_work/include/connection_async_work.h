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

#ifndef COMMUNICATIONNETMANAGERBASE_CONNECTION_ASYNC_WORK_H
#define COMMUNICATIONNETMANAGERBASE_CONNECTION_ASYNC_WORK_H

#include <napi/native_api.h>
#include <napi/native_node_api.h>

namespace OHOS::NetManagerStandard {
class ConnectionAsyncWork final {
public:
    ConnectionAsyncWork() = delete;

    ~ConnectionAsyncWork() = delete;

    static void ExecGetDefaultNet(napi_env env, void *data);

    static void GetDefaultNetCallback(napi_env env, napi_status status, void *data);

    static void ExecHasDefaultNet(napi_env env, void *data);

    static void HasDefaultNetCallback(napi_env env, napi_status status, void *data);

    static void ExecIsDefaultNetMetered(napi_env env, void *data);

    static void IsDefaultNetMeteredCallback(napi_env env, napi_status status, void *data);

    static void ExecGetNetCapabilities(napi_env env, void *data);

    static void GetNetCapabilitiesCallback(napi_env env, napi_status status, void *data);

    static void ExecGetConnectionProperties(napi_env env, void *data);

    static void GetConnectionPropertiesCallback(napi_env env, napi_status status, void *data);

    static void ExecGetAddressesByName(napi_env env, void *data);

    static void GetAddressesByNameCallback(napi_env env, napi_status status, void *data);

    static void ExecGetAllNets(napi_env env, void *data);

    static void GetAllNetsCallback(napi_env env, napi_status status, void *data);

    static void ExecEnableAirplaneMode(napi_env env, void *data);

    static void EnableAirplaneModeCallback(napi_env env, napi_status status, void *data);

    static void ExecDisableAirplaneMode(napi_env env, void *data);

    static void DisableAirplaneModeCallback(napi_env env, napi_status status, void *data);

    static void ExecReportNetConnected(napi_env env, void *data);

    static void ReportNetConnectedCallback(napi_env env, napi_status status, void *data);

    static void ExecReportNetDisconnected(napi_env env, void *data);

    static void ReportNetDisconnectedCallback(napi_env env, napi_status status, void *data);

    static void ExecGetGlobalHttpProxy(napi_env env, void *data);

    static void GetGlobalHttpProxyCallback(napi_env env, napi_status status, void *data);

    static void ExecSetGlobalHttpProxy(napi_env env, void *data);

    static void SetGlobalHttpProxyCallback(napi_env env, napi_status status, void *data);

    static void ExecGetAppNet(napi_env env, void *data);

    static void GetAppNetCallback(napi_env env, napi_status status, void *data);

    static void ExecSetAppNet(napi_env env, void *data);

    static void SetAppNetCallback(napi_env env, napi_status status, void *data);

    class NetHandleAsyncWork final {
    public:
        NetHandleAsyncWork() = delete;

        ~NetHandleAsyncWork() = delete;

        static void ExecGetAddressByName(napi_env env, void *data);

        static void GetAddressByNameCallback(napi_env env, napi_status status, void *data);

        static void ExecGetAddressesByName(napi_env env, void *data);

        static void GetAddressesByNameCallback(napi_env env, napi_status status, void *data);

        static void ExecBindSocket(napi_env env, void *data);

        static void BindSocketCallback(napi_env env, napi_status status, void *data);
    };

    class NetConnectionAsyncWork final {
    public:
        NetConnectionAsyncWork() = delete;

        ~NetConnectionAsyncWork() = delete;

        static void ExecRegister(napi_env env, void *data);

        static void RegisterCallback(napi_env env, napi_status status, void *data);

        static void ExecUnregister(napi_env env, void *data);

        static void UnregisterCallback(napi_env env, napi_status status, void *data);
    };
};
} // namespace OHOS::NetManagerStandard

#endif /* COMMUNICATIONNETMANAGERBASE_CONNECTION_ASYNC_WORK_H */
