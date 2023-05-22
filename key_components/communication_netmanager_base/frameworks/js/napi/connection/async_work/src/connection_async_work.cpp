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

#include "connection_async_work.h"

#include "connection_exec.h"
#include "base_async_work.h"
#include "getappnet_context.h"
#include "getglobalhttpproxy_context.h"
#include "none_params_context.h"
#include "parse_nethandle_context.h"
#include "setappnet_context.h"
#include "setglobalhttpproxy_context.h"

namespace OHOS::NetManagerStandard {
void ConnectionAsyncWork::ExecGetAddressesByName(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetAddressByNameContext, ConnectionExec::ExecGetAddressByName>(env, data);
}

void ConnectionAsyncWork::GetAddressesByNameCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetAddressByNameContext, ConnectionExec::GetAddressByNameCallback>(env, status,
                                                                                                        data);
}

void ConnectionAsyncWork::ExecHasDefaultNet(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<HasDefaultNetContext, ConnectionExec::ExecHasDefaultNet>(env, data);
}

void ConnectionAsyncWork::HasDefaultNetCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<HasDefaultNetContext, ConnectionExec::HasDefaultNetCallback>(env, status, data);
}

void ConnectionAsyncWork::ExecIsDefaultNetMetered(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<IsDefaultNetMeteredContext, ConnectionExec::ExecIsDefaultNetMetered>(env, data);
}

void ConnectionAsyncWork::IsDefaultNetMeteredCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<IsDefaultNetMeteredContext, ConnectionExec::IsDefaultNetMeteredCallback>(
        env, status, data);
}

void ConnectionAsyncWork::ExecGetNetCapabilities(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetNetCapabilitiesContext, ConnectionExec::ExecGetNetCapabilities>(env, data);
}

void ConnectionAsyncWork::GetNetCapabilitiesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetNetCapabilitiesContext, ConnectionExec::GetNetCapabilitiesCallback>(env, status,
                                                                                                            data);
}

void ConnectionAsyncWork::ExecGetConnectionProperties(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetConnectionPropertiesContext, ConnectionExec::ExecGetConnectionProperties>(env,
                                                                                                              data);
}

void ConnectionAsyncWork::GetConnectionPropertiesCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetConnectionPropertiesContext, ConnectionExec::GetConnectionPropertiesCallback>(
        env, status, data);
}

void ConnectionAsyncWork::ExecGetDefaultNet(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetDefaultNetContext, ConnectionExec::ExecGetDefaultNet>(env, data);
}

void ConnectionAsyncWork::GetDefaultNetCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetDefaultNetContext, ConnectionExec::GetDefaultNetCallback>(env, status, data);
}

void ConnectionAsyncWork::ExecGetAllNets(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetAllNetsContext, ConnectionExec::ExecGetAllNets>(env, data);
}

void ConnectionAsyncWork::GetAllNetsCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetAllNetsContext, ConnectionExec::GetAllNetsCallback>(env, status, data);
}

void ConnectionAsyncWork::ExecEnableAirplaneMode(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<EnableAirplaneModeContext, ConnectionExec::ExecEnableAirplaneMode>(env, data);
}

void ConnectionAsyncWork::EnableAirplaneModeCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<EnableAirplaneModeContext, ConnectionExec::EnableAirplaneModeCallback>(env, status,
                                                                                                            data);
}

void ConnectionAsyncWork::ExecDisableAirplaneMode(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<DisableAirplaneModeContext, ConnectionExec::ExecDisableAirplaneMode>(env, data);
}

void ConnectionAsyncWork::DisableAirplaneModeCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<DisableAirplaneModeContext, ConnectionExec::DisableAirplaneModeCallback>(
        env, status, data);
}

void ConnectionAsyncWork::ExecReportNetConnected(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<ReportNetConnectedContext, ConnectionExec::ExecReportNetConnected>(env, data);
}

void ConnectionAsyncWork::ReportNetConnectedCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<ReportNetConnectedContext, ConnectionExec::ReportNetConnectedCallback>(env, status,
                                                                                                            data);
}

void ConnectionAsyncWork::ExecReportNetDisconnected(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<ReportNetDisconnectedContext, ConnectionExec::ExecReportNetDisconnected>(env, data);
}

void ConnectionAsyncWork::ReportNetDisconnectedCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<ReportNetDisconnectedContext, ConnectionExec::ReportNetDisconnectedCallback>(
        env, status, data);
}

void ConnectionAsyncWork::ExecGetGlobalHttpProxy(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetGlobalHttpProxyContext, ConnectionExec::ExecGetGlobalHttpProxy>(env, data);
}

void ConnectionAsyncWork::GetGlobalHttpProxyCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetGlobalHttpProxyContext, ConnectionExec::GetGlobalHttpProxyCallback>(
        env, status, data);
}

void ConnectionAsyncWork::ExecSetGlobalHttpProxy(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<SetGlobalHttpProxyContext, ConnectionExec::ExecSetGlobalHttpProxy>(env, data);
}

void ConnectionAsyncWork::SetGlobalHttpProxyCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<SetGlobalHttpProxyContext, ConnectionExec::SetGlobalHttpProxyCallback>(
        env, status, data);
}

void ConnectionAsyncWork::ExecGetAppNet(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetAppNetContext, ConnectionExec::ExecGetAppNet>(env, data);
}

void ConnectionAsyncWork::GetAppNetCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetAppNetContext, ConnectionExec::GetAppNetCallback>(env, status, data);
}

void ConnectionAsyncWork::ExecSetAppNet(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<SetAppNetContext, ConnectionExec::ExecSetAppNet>(env, data);
}

void ConnectionAsyncWork::SetAppNetCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<SetAppNetContext, ConnectionExec::SetAppNetCallback>(env, status, data);
}

void ConnectionAsyncWork::NetHandleAsyncWork::ExecGetAddressesByName(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetAddressByNameContext, ConnectionExec::NetHandleExec::ExecGetAddressesByName>(env,
                                                                                                                 data);
}

void ConnectionAsyncWork::NetHandleAsyncWork::GetAddressesByNameCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetAddressByNameContext,
                                     ConnectionExec::NetHandleExec::GetAddressesByNameCallback>(env, status, data);
}

void ConnectionAsyncWork::NetHandleAsyncWork::ExecGetAddressByName(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<GetAddressByNameContext, ConnectionExec::NetHandleExec::ExecGetAddressByName>(env,
                                                                                                               data);
}

void ConnectionAsyncWork::NetHandleAsyncWork::GetAddressByNameCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<GetAddressByNameContext, ConnectionExec::NetHandleExec::GetAddressByNameCallback>(
        env, status, data);
}

void ConnectionAsyncWork::NetHandleAsyncWork::ExecBindSocket(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<BindSocketContext, ConnectionExec::NetHandleExec::ExecBindSocket>(env, data);
}

void ConnectionAsyncWork::NetHandleAsyncWork::BindSocketCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<BindSocketContext, ConnectionExec::NetHandleExec::BindSocketCallback>(env, status,
                                                                                                           data);
}

void ConnectionAsyncWork::NetConnectionAsyncWork::ExecRegister(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<RegisterContext, ConnectionExec::NetConnectionExec::ExecRegister>(env, data);
}

void ConnectionAsyncWork::NetConnectionAsyncWork::RegisterCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<RegisterContext, ConnectionExec::NetConnectionExec::RegisterCallback>(env, status,
                                                                                                           data);
}

void ConnectionAsyncWork::NetConnectionAsyncWork::ExecUnregister(napi_env env, void *data)
{
    BaseAsyncWork::ExecAsyncWork<UnregisterContext, ConnectionExec::NetConnectionExec::ExecUnregister>(env, data);
}

void ConnectionAsyncWork::NetConnectionAsyncWork::UnregisterCallback(napi_env env, napi_status status, void *data)
{
    BaseAsyncWork::AsyncWorkCallback<UnregisterContext, ConnectionExec::NetConnectionExec::UnregisterCallback>(
        env, status, data);
}
} // namespace OHOS::NetManagerStandard
