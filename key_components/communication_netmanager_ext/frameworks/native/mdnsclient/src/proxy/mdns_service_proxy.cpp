/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "mdns_service_proxy.h"

#include "iremote_object.h"
#include "net_manager_constants.h"
#include "netmgr_ext_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
MDnsServiceProxy::MDnsServiceProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IMDnsService>(impl) {}

MDnsServiceProxy::~MDnsServiceProxy() = default;

int32_t MDnsServiceProxy::RegisterService(const MDnsServiceInfo &serviceInfo, const sptr<IRegistrationCallback> &cb)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(MDnsServiceProxy::GetDescriptor())) {
        NETMGR_EXT_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_EXT_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    sptr<MDnsServiceInfo> info = new (std::nothrow) MDnsServiceInfo(serviceInfo);
    if (info == nullptr) {
        NETMGR_EXT_LOG_E("info is nullptr");
        return NETMANAGER_EXT_ERR_LOCAL_PTR_NULL;
    }
    if (!MDnsServiceInfo::Marshalling(data, info)) {
        NETMGR_EXT_LOG_E("Marshalling failed");
        return NETMANAGER_EXT_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteRemoteObject(cb->AsObject().GetRefPtr())) {
        NETMGR_EXT_LOG_E("proxy write callback failed");
        return NETMANAGER_EXT_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("Remote is null");
        return NETMANAGER_EXT_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(CMD_REGISTER, data, reply, option);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("SendRequest failed, error code: [%{public}d]", ret);
        return NETMANAGER_EXT_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t retCode = reply.ReadInt32();
    if (retCode != NETMANAGER_EXT_SUCCESS) {
        NETMGR_EXT_LOG_E("MDnsService::RegisterService return: [%{public}d]", retCode);
    }
    return retCode;
}

int32_t MDnsServiceProxy::UnRegisterService(const sptr<IRegistrationCallback> &cb)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(MDnsServiceProxy::GetDescriptor())) {
        NETMGR_EXT_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_EXT_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteRemoteObject(cb->AsObject().GetRefPtr())) {
        NETMGR_EXT_LOG_E("proxy write callback failed");
        return NETMANAGER_EXT_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("Remote is null");
        return NETMANAGER_EXT_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(CMD_STOP_REGISTER, data, reply, option);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("SendRequest failed, error code: [%{public}d]", ret);
        return NETMANAGER_EXT_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t retCode = reply.ReadInt32();
    if (retCode != NETMANAGER_EXT_SUCCESS) {
        NETMGR_EXT_LOG_E("MDnsService::UnRegisterService return: [%{public}d]", retCode);
    }
    return retCode;
}

int32_t MDnsServiceProxy::StartDiscoverService(const std::string &serviceType, const sptr<IDiscoveryCallback> &cb)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(MDnsServiceProxy::GetDescriptor())) {
        NETMGR_EXT_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_EXT_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteString(serviceType)) {
        return NETMANAGER_EXT_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteRemoteObject(cb->AsObject().GetRefPtr())) {
        NETMGR_EXT_LOG_E("proxy write callback failed");
        return NETMANAGER_EXT_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("Remote is null");
        return NETMANAGER_EXT_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(CMD_DISCOVER, data, reply, option);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("SendRequest failed, error code: [%{public}d]", ret);
        return NETMANAGER_EXT_ERR_IPC_CONNECT_STUB_FAIL;
    }

    int32_t retCode = reply.ReadInt32();
    if (retCode != NETMANAGER_EXT_SUCCESS) {
        NETMGR_EXT_LOG_E("MDnsService::DiscoverServices return: [%{public}d]", retCode);
    }
    return retCode;
}

int32_t MDnsServiceProxy::StopDiscoverService(const sptr<IDiscoveryCallback> &cb)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(MDnsServiceProxy::GetDescriptor())) {
        NETMGR_EXT_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_EXT_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteRemoteObject(cb->AsObject().GetRefPtr())) {
        NETMGR_EXT_LOG_E("proxy write callback failed");
        return NETMANAGER_EXT_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("Remote is null");
        return NETMANAGER_EXT_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(CMD_STOP_DISCOVER, data, reply, option);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("SendRequest failed, error code: [%{public}d]", ret);
        return NETMANAGER_EXT_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t retCode = reply.ReadInt32();
    if (retCode != NETMANAGER_EXT_SUCCESS) {
        NETMGR_EXT_LOG_E("MDnsService::StopServiceDiscovery return: [%{public}d]", retCode);
    }
    return retCode;
}

int32_t MDnsServiceProxy::ResolveService(const MDnsServiceInfo &serviceInfo, const sptr<IResolveCallback> &cb)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(MDnsServiceProxy::GetDescriptor())) {
        NETMGR_EXT_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_EXT_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    sptr<MDnsServiceInfo> info = new (std::nothrow) MDnsServiceInfo(serviceInfo);
    if (info == nullptr) {
        NETMGR_EXT_LOG_E("info is nullptr");
        return NETMANAGER_EXT_ERR_LOCAL_PTR_NULL;
    }
    if (!MDnsServiceInfo::Marshalling(data, info)) {
        NETMGR_EXT_LOG_E("Marshalling failed");
        return NETMANAGER_EXT_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteRemoteObject(cb->AsObject().GetRefPtr())) {
        NETMGR_EXT_LOG_E("proxy write callback failed");
        return NETMANAGER_EXT_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("Remote is null");
        return NETMANAGER_EXT_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(CMD_RESOLVE, data, reply, option);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("SendRequest failed, error code: [%d]", ret);
        return ret;
    }
    int32_t retCode = reply.ReadInt32();
    if (retCode != NETMANAGER_EXT_SUCCESS) {
        NETMGR_EXT_LOG_E("MDnsService::ResolveService return: [%{public}d]", retCode);
    }
    return retCode;
}
} // namespace NetManagerStandard
} // namespace OHOS