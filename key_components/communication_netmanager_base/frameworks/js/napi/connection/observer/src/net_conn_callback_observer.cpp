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

#include "net_conn_callback_observer.h"

#include "connection_exec.h"
#include "constant.h"
#include "netconnection.h"
#include "netmanager_base_log.h"

namespace OHOS::NetManagerStandard {
int32_t NetConnCallbackObserver::NetAvailable(sptr<NetHandle> &netHandle)
{
    if (netHandle == nullptr) {
        return 0;
    }
    NetConnection *netConnection = NET_CONNECTIONS[this];
    if (netConnection == nullptr) {
        NETMANAGER_BASE_LOGI("can not find netConnection handle");
        return 0;
    }
    if (!netConnection->GetEventManager()->HasEventListener(EVENT_NET_AVAILABLE)) {
        NETMANAGER_BASE_LOGI("no event listener find %{public}s", EVENT_NET_AVAILABLE);
        return 0;
    }
    netConnection->GetEventManager()->EmitByUv(EVENT_NET_AVAILABLE, new NetHandle(*netHandle), NetAvailableCallback);
    return 0;
}

int32_t NetConnCallbackObserver::NetCapabilitiesChange(sptr<NetHandle> &netHandle,
                                                       const sptr<NetAllCapabilities> &netAllCap)
{
    if (netHandle == nullptr) {
        return 0;
    }
    NetConnection *netConnection = NET_CONNECTIONS[this];
    if (netConnection == nullptr) {
        NETMANAGER_BASE_LOGI("can not find netConnection handle");
        return 0;
    }
    if (!netConnection->GetEventManager()->HasEventListener(EVENT_NET_CAPABILITIES_CHANGE)) {
        NETMANAGER_BASE_LOGI("no event listener find %{public}s", EVENT_NET_CAPABILITIES_CHANGE);
        return 0;
    }
    auto pair = new std::pair<NetHandle *, NetAllCapabilities *>;
    pair->first = new NetHandle(*netHandle);
    pair->second = new NetAllCapabilities(*netAllCap);
    netConnection->GetEventManager()->EmitByUv(EVENT_NET_CAPABILITIES_CHANGE, pair, NetCapabilitiesChangeCallback);
    return 0;
}

int32_t NetConnCallbackObserver::NetConnectionPropertiesChange(sptr<NetHandle> &netHandle,
                                                               const sptr<NetLinkInfo> &info)
{
    if (netHandle == nullptr) {
        return 0;
    }
    NetConnection *netConnection = NET_CONNECTIONS[this];
    if (netConnection == nullptr) {
        NETMANAGER_BASE_LOGI("can not find netConnection handle");
        return 0;
    }
    if (!netConnection->GetEventManager()->HasEventListener(EVENT_NET_CONNECTION_PROPERTIES_CHANGE)) {
        NETMANAGER_BASE_LOGI("no event listener find %{public}s", EVENT_NET_CONNECTION_PROPERTIES_CHANGE);
        return 0;
    }
    auto pair = new std::pair<NetHandle *, NetLinkInfo *>;
    pair->first = new NetHandle(*netHandle);
    pair->second = new NetLinkInfo(*info);
    netConnection->GetEventManager()->EmitByUv(EVENT_NET_CONNECTION_PROPERTIES_CHANGE, pair,
                                               NetConnectionPropertiesChangeCallback);
    return 0;
}

int32_t NetConnCallbackObserver::NetLost(sptr<NetHandle> &netHandle)
{
    if (netHandle == nullptr) {
        return 0;
    }
    NetConnection *netConnection = NET_CONNECTIONS[this];
    if (netConnection == nullptr) {
        NETMANAGER_BASE_LOGI("can not find netConnection handle");
        return 0;
    }
    if (!netConnection->GetEventManager()->HasEventListener(EVENT_NET_LOST)) {
        NETMANAGER_BASE_LOGI("no event listener find %{public}s", EVENT_NET_LOST);
        return 0;
    }
    netConnection->GetEventManager()->EmitByUv(EVENT_NET_LOST, new NetHandle(*netHandle), NetLostCallback);
    return 0;
}

int32_t NetConnCallbackObserver::NetUnavailable()
{
    NetConnection *netConnection = NET_CONNECTIONS[this];
    if (netConnection == nullptr) {
        NETMANAGER_BASE_LOGI("can not find netConnection handle");
        return 0;
    }
    if (!netConnection->GetEventManager()->HasEventListener(EVENT_NET_UNAVAILABLE)) {
        NETMANAGER_BASE_LOGI("no event listener find %{public}s", EVENT_NET_UNAVAILABLE);
        return 0;
    }
    netConnection->GetEventManager()->EmitByUv(EVENT_NET_UNAVAILABLE, nullptr, NetUnavailableCallback);
    return 0;
}

int32_t NetConnCallbackObserver::NetBlockStatusChange(sptr<NetHandle> &netHandle, bool blocked)
{
    NetConnection *netConnection = NET_CONNECTIONS[this];
    if (netConnection == nullptr) {
        NETMANAGER_BASE_LOGI("can not find netConnection handle");
        return 0;
    }
    if (!netConnection->GetEventManager()->HasEventListener(EVENT_NET_BLOCK_STATUS_CHANGE)) {
        NETMANAGER_BASE_LOGI("no event listener find %{public}s", EVENT_NET_BLOCK_STATUS_CHANGE);
        return 0;
    }
    auto pair = new std::pair<NetHandle *, bool>;
    pair->first = new NetHandle(*netHandle);
    pair->second = blocked;
    netConnection->GetEventManager()->EmitByUv(EVENT_NET_BLOCK_STATUS_CHANGE, pair, NetBlockStatusChangeCallback);
    return 0;
}

napi_value NetConnCallbackObserver::CreateNetHandle(napi_env env, NetHandle *handle)
{
    napi_value netHandle = ConnectionExec::CreateNetHandle(env, handle);
    delete handle;
    return netHandle;
}

napi_value NetConnCallbackObserver::CreateNetCapabilities(napi_env env, NetAllCapabilities *capabilities)
{
    napi_value netCapabilities = ConnectionExec::CreateNetCapabilities(env, capabilities);
    delete capabilities;
    return netCapabilities;
}

napi_value NetConnCallbackObserver::CreateConnectionProperties(napi_env env, NetLinkInfo *linkInfo)
{
    napi_value connectionProperties = ConnectionExec::CreateConnectionProperties(env, linkInfo);
    delete linkInfo;
    return connectionProperties;
}

napi_value NetConnCallbackObserver::CreateNetAvailableParam(napi_env env, void *data)
{
    return CreateNetHandle(env, static_cast<NetHandle *>(data));
}

napi_value NetConnCallbackObserver::CreateNetCapabilitiesChangeParam(napi_env env, void *data)
{
    auto pair = static_cast<std::pair<NetHandle *, NetAllCapabilities *> *>(data);
    napi_value netHandle = CreateNetHandle(env, pair->first);
    napi_value capabilities = CreateNetCapabilities(env, pair->second);
    napi_value obj = NapiUtils::CreateObject(env);
    NapiUtils::SetNamedProperty(env, obj, KEY_NET_HANDLE, netHandle);
    NapiUtils::SetNamedProperty(env, obj, KEY_NET_CAP, capabilities);
    delete pair;
    return obj;
}

napi_value NetConnCallbackObserver::CreateNetConnectionPropertiesChangeParam(napi_env env, void *data)
{
    auto pair = static_cast<std::pair<NetHandle *, NetLinkInfo *> *>(data);
    napi_value netHandle = CreateNetHandle(env, pair->first);
    napi_value properties = CreateConnectionProperties(env, pair->second);
    napi_value obj = NapiUtils::CreateObject(env);
    NapiUtils::SetNamedProperty(env, obj, KEY_NET_HANDLE, netHandle);
    NapiUtils::SetNamedProperty(env, obj, KEY_CONNECTION_PROPERTIES, properties);
    delete pair;
    return obj;
}

napi_value NetConnCallbackObserver::CreateNetLostParam(napi_env env, void *data)
{
    return CreateNetHandle(env, static_cast<NetHandle *>(data));
}

napi_value NetConnCallbackObserver::CreateNetUnavailableParam(napi_env env, void *data)
{
    (void)data;

    return NapiUtils::GetUndefined(env);
}

napi_value NetConnCallbackObserver::CreateNetBlockStatusChangeParam(napi_env env, void *data)
{
    auto pair = static_cast<std::pair<NetHandle *, bool> *>(data);
    napi_value netHandle = CreateNetHandle(env, pair->first);
    napi_value obj = NapiUtils::CreateObject(env);
    NapiUtils::SetNamedProperty(env, obj, KEY_NET_HANDLE, netHandle);
    NapiUtils::SetBooleanProperty(env, obj, KEY_BLOCKED, pair->second);
    delete pair;
    return obj;
}

void NetConnCallbackObserver::NetAvailableCallback(uv_work_t *work, int status)
{
    CallbackTemplate<CreateNetAvailableParam>(work, status);
}

void NetConnCallbackObserver::NetCapabilitiesChangeCallback(uv_work_t *work, int status)
{
    CallbackTemplate<CreateNetCapabilitiesChangeParam>(work, status);
}

void NetConnCallbackObserver::NetConnectionPropertiesChangeCallback(uv_work_t *work, int status)
{
    CallbackTemplate<CreateNetConnectionPropertiesChangeParam>(work, status);
}

void NetConnCallbackObserver::NetLostCallback(uv_work_t *work, int status)
{
    CallbackTemplate<CreateNetLostParam>(work, status);
}

void NetConnCallbackObserver::NetUnavailableCallback(uv_work_t *work, int status)
{
    CallbackTemplate<CreateNetUnavailableParam>(work, status);
}

void NetConnCallbackObserver::NetBlockStatusChangeCallback(uv_work_t *work, int status)
{
    CallbackTemplate<CreateNetBlockStatusChangeParam>(work, status);
}
} // namespace OHOS::NetManagerStandard
