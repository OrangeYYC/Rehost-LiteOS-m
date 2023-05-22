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

#include "networkshare_service_proxy.h"

#include "ipc_types.h"
#include "net_manager_constants.h"
#include "net_manager_ext_constants.h"
#include "netmgr_ext_log_wrapper.h"
#include "networkshare_constants.h"

namespace OHOS {
namespace NetManagerStandard {
NetworkShareServiceProxy::NetworkShareServiceProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<INetworkShareService>(impl)
{
}

bool NetworkShareServiceProxy::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(NetworkShareServiceProxy::GetDescriptor())) {
        NETMGR_EXT_LOG_E("WriteInterfaceToken failed");
        return false;
    }
    return true;
}

int32_t NetworkShareServiceProxy::SendRequest(INetworkShareService::MessageCode code, MessageParcel &data,
                                              MessageParcel &reply)
{
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        return NETMANAGER_EXT_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageOption option(MessageOption::TF_SYNC);
    return remote->SendRequest(static_cast<uint32_t>(code), data, reply, option);
}

int32_t NetworkShareServiceProxy::IsNetworkSharingSupported(int32_t &supported)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return NETMANAGER_EXT_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    MessageParcel reply;
    int32_t ret = SendRequest(INetworkShareService::MessageCode::CMD_GET_SHARING_SUPPORTED, data, reply);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("IsNetworkSharingSupported proxy SendRequest failed, error code: [%{public}d]", ret);
        return NETMANAGER_EXT_ERR_IPC_CONNECT_STUB_FAIL;
    }
    supported = reply.ReadInt32();
    return reply.ReadInt32();
}

int32_t NetworkShareServiceProxy::IsSharing(int32_t &sharingStatus)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return NETMANAGER_EXT_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    MessageParcel reply;
    int32_t ret = SendRequest(INetworkShareService::MessageCode::CMD_GET_IS_SHARING, data, reply);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("IsSharing proxy SendRequest failed, error code: [%{public}d]", ret);
        return NETMANAGER_EXT_ERR_IPC_CONNECT_STUB_FAIL;
    }
    sharingStatus = reply.ReadInt32();
    return reply.ReadInt32();
}

int32_t NetworkShareServiceProxy::StartNetworkSharing(const SharingIfaceType &type)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return NETMANAGER_EXT_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteInt32(static_cast<int32_t>(type))) {
        return NETMANAGER_EXT_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t ret = SendRequest(INetworkShareService::MessageCode::CMD_START_NETWORKSHARE, data, reply);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("StartNetworkSharing proxy SendRequest failed, error code: [%{public}d]", ret);
        return NETMANAGER_EXT_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return reply.ReadInt32();
}

int32_t NetworkShareServiceProxy::StopNetworkSharing(const SharingIfaceType &type)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return NETMANAGER_EXT_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteInt32(static_cast<int32_t>(type))) {
        return NETMANAGER_EXT_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t ret = SendRequest(INetworkShareService::MessageCode::CMD_STOP_NETWORKSHARE, data, reply);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("StopNetworkSharing proxy SendRequest failed, error code: [%{public}d]", ret);
        return NETMANAGER_EXT_ERR_IPC_CONNECT_STUB_FAIL;
    }

    return reply.ReadInt32();
}

int32_t NetworkShareServiceProxy::GetSharableRegexs(SharingIfaceType type, std::vector<std::string> &ifaceRegexs)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return NETMANAGER_EXT_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteInt32(static_cast<int32_t>(type))) {
        return NETMANAGER_EXT_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t ret = SendRequest(INetworkShareService::MessageCode::CMD_GET_SHARABLE_REGEXS, data, reply);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("proxy GetSharableRegexs SendRequest failed, error code: [%{public}d]", ret);
        return NETMANAGER_EXT_ERR_IPC_CONNECT_STUB_FAIL;
    }

    int32_t size = reply.ReadInt32();
    for (int i = 0; i < size; i++) {
        ifaceRegexs.push_back(reply.ReadString());
    }
    return reply.ReadInt32();
}

int32_t NetworkShareServiceProxy::GetSharingState(SharingIfaceType type, SharingIfaceState &state)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return NETMANAGER_EXT_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteInt32(static_cast<int32_t>(type))) {
        return NETMANAGER_EXT_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t ret = SendRequest(INetworkShareService::MessageCode::CMD_GET_SHARING_STATE, data, reply);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("proxy GetSharingState SendRequest failed, error code: [%{public}d]", ret);
        return NETMANAGER_EXT_ERR_IPC_CONNECT_STUB_FAIL;
    }

    state = static_cast<SharingIfaceState>(reply.ReadInt32());
    return reply.ReadInt32();
}

int32_t NetworkShareServiceProxy::GetNetSharingIfaces(const SharingIfaceState &state, std::vector<std::string> &ifaces)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return NETMANAGER_EXT_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteInt32(static_cast<int32_t>(state))) {
        return NETMANAGER_EXT_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t ret = SendRequest(INetworkShareService::MessageCode::CMD_GET_SHARING_IFACES, data, reply);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("proxy GetNetSharingIfaces SendRequest failed, error code: [%{public}d]", ret);
        return NETMANAGER_EXT_ERR_IPC_CONNECT_STUB_FAIL;
    }

    int32_t size = reply.ReadInt32();
    for (int i = 0; i < size; i++) {
        ifaces.push_back(reply.ReadString());
    }
    return reply.ReadInt32();
}

int32_t NetworkShareServiceProxy::RegisterSharingEvent(sptr<ISharingEventCallback> callback)
{
    if (callback == nullptr) {
        NETMGR_EXT_LOG_E("RegisterSharingEvent callback is null.");
        return NETMANAGER_EXT_ERR_LOCAL_PTR_NULL;
    }
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return NETMANAGER_EXT_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        NETMGR_EXT_LOG_E("RegisterSharingEvent proxy write callback failed");
        return NETMANAGER_EXT_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t ret = SendRequest(INetworkShareService::MessageCode::CMD_REGISTER_EVENT_CALLBACK, data, reply);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("RegisterSharingEvent proxy SendRequest failed, error code: [%{public}d]", ret);
        return NETMANAGER_EXT_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return reply.ReadInt32();
}

int32_t NetworkShareServiceProxy::UnregisterSharingEvent(sptr<ISharingEventCallback> callback)
{
    if (callback == nullptr) {
        NETMGR_EXT_LOG_E("UnregisterSharingEvent callback is null.");
        return NETMANAGER_EXT_ERR_LOCAL_PTR_NULL;
    }
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return NETMANAGER_EXT_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        NETMGR_EXT_LOG_E("UnregisterSharingEvent proxy write callback failed");
        return NETMANAGER_EXT_ERR_WRITE_DATA_FAIL;
    }

    MessageParcel reply;
    int32_t ret = SendRequest(INetworkShareService::MessageCode::CMD_UNREGISTER_EVENT_CALLBACK, data, reply);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("UnregisterSharingEvent proxy SendRequest failed, error code: [%{public}d]", ret);
        return NETMANAGER_EXT_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return reply.ReadInt32();
}

int32_t NetworkShareServiceProxy::GetStatsRxBytes(int32_t &bytes)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return NETMANAGER_EXT_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    MessageParcel reply;
    int32_t ret = SendRequest(INetworkShareService::MessageCode::CMD_GET_RX_BYTES, data, reply);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("GetStatsRxBytes proxy SendRequest failed, error code: [%{public}d]", ret);
        return NETMANAGER_EXT_ERR_IPC_CONNECT_STUB_FAIL;
    }
    bytes = reply.ReadInt32();
    return reply.ReadInt32();
}

int32_t NetworkShareServiceProxy::GetStatsTxBytes(int32_t &bytes)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return NETMANAGER_EXT_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    MessageParcel reply;
    int32_t ret = SendRequest(INetworkShareService::MessageCode::CMD_GET_TX_BYTES, data, reply);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("GetStatsTxBytes proxy SendRequest failed, error code: [%{public}d]", ret);
        return NETMANAGER_EXT_ERR_IPC_CONNECT_STUB_FAIL;
    }
    bytes = reply.ReadInt32();
    return reply.ReadInt32();
}

int32_t NetworkShareServiceProxy::GetStatsTotalBytes(int32_t &bytes)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return NETMANAGER_EXT_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    MessageParcel reply;
    int32_t ret = SendRequest(INetworkShareService::MessageCode::CMD_GET_TOTAL_BYTES, data, reply);
    if (ret != ERR_NONE) {
        NETMGR_EXT_LOG_E("GetStatsTotalBytes proxy SendRequest failed, error code: [%{public}d]", ret);
        return NETMANAGER_EXT_ERR_IPC_CONNECT_STUB_FAIL;
    }
    bytes = reply.ReadInt32();
    return reply.ReadInt32();
}
} // namespace NetManagerStandard
} // namespace OHOS
