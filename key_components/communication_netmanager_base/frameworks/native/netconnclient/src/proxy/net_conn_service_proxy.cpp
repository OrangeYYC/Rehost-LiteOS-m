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
#include "net_conn_service_proxy.h"

#include "net_conn_constants.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
static constexpr uint32_t MAX_IFACE_NUM = 16;
static constexpr uint32_t MAX_NET_CAP_NUM = 32;

NetConnServiceProxy::NetConnServiceProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<INetConnService>(impl) {}

NetConnServiceProxy::~NetConnServiceProxy() {}

int32_t NetConnServiceProxy::SystemReady()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(CMD_NM_SYSTEM_READY, data, reply, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceProxy::RegisterNetSupplier(NetBearType bearerType, const std::string &ident,
                                                 const std::set<NetCap> &netCaps, uint32_t &supplierId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteUint32(static_cast<uint32_t>(bearerType))) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(ident)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    uint32_t size = static_cast<uint32_t>(netCaps.size());
    if (!data.WriteUint32(size)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    for (auto netCap : netCaps) {
        if (!data.WriteUint32(static_cast<uint32_t>(netCap))) {
            return NETMANAGER_ERR_WRITE_DATA_FAIL;
        }
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(CMD_NM_REG_NET_SUPPLIER, data, reply, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    int32_t ret;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.ReadUint32(supplierId)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::UnregisterNetSupplier(uint32_t supplierId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    NETMGR_LOG_D("proxy supplierId[%{public}d]", supplierId);
    if (!data.WriteUint32(supplierId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(CMD_NM_UNREG_NETWORK, data, reply, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    return reply.ReadInt32();
}

int32_t NetConnServiceProxy::RegisterNetSupplierCallback(uint32_t supplierId,
                                                         const sptr<INetSupplierCallback> &callback)
{
    if (callback == nullptr) {
        NETMGR_LOG_E("The parameter of callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteUint32(supplierId);
    dataParcel.WriteRemoteObject(callback->AsObject().GetRefPtr());

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageOption option;
    MessageParcel replyParcel;
    int32_t retCode = remote->SendRequest(CMD_NM_REGISTER_NET_SUPPLIER_CALLBACK, dataParcel, replyParcel, option);
    NETMGR_LOG_I("SendRequest retCode:[%{public}d]", retCode);
    if (retCode != ERR_NONE) {
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    return replyParcel.ReadInt32();
}

int32_t NetConnServiceProxy::RegisterNetConnCallback(const sptr<INetConnCallback> &callback)
{
    if (callback == nullptr) {
        NETMGR_LOG_E("The parameter of callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteRemoteObject(callback->AsObject().GetRefPtr());

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageOption option;
    MessageParcel replyParcel;
    int32_t retCode = remote->SendRequest(CMD_NM_REGISTER_NET_CONN_CALLBACK, dataParcel, replyParcel, option);
    NETMGR_LOG_D("SendRequest retCode:[%{public}d]", retCode);
    if (retCode != ERR_NONE) {
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    return replyParcel.ReadInt32();
}

int32_t NetConnServiceProxy::RegisterNetConnCallback(const sptr<NetSpecifier> &netSpecifier,
                                                     const sptr<INetConnCallback> &callback, const uint32_t &timeoutMS)
{
    if (netSpecifier == nullptr || callback == nullptr) {
        NETMGR_LOG_E("The parameter of netSpecifier or callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    netSpecifier->Marshalling(dataParcel);
    dataParcel.WriteUint32(timeoutMS);
    dataParcel.WriteRemoteObject(callback->AsObject().GetRefPtr());

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageOption option;
    MessageParcel replyParcel;
    int32_t retCode =
        remote->SendRequest(CMD_NM_REGISTER_NET_CONN_CALLBACK_BY_SPECIFIER, dataParcel, replyParcel, option);
    NETMGR_LOG_D("SendRequest retCode:[%{public}d]", retCode);
    if (retCode != ERR_NONE) {
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    return replyParcel.ReadInt32();
}

int32_t NetConnServiceProxy::UnregisterNetConnCallback(const sptr<INetConnCallback> &callback)
{
    if (callback == nullptr) {
        NETMGR_LOG_E("The parameter of callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    dataParcel.WriteRemoteObject(callback->AsObject().GetRefPtr());

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageOption option;
    MessageParcel replyParcel;
    int32_t retCode = remote->SendRequest(CMD_NM_UNREGISTER_NET_CONN_CALLBACK, dataParcel, replyParcel, option);
    NETMGR_LOG_D("SendRequest retCode:[%{public}d]", retCode);
    if (retCode != ERR_NONE) {
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    return replyParcel.ReadInt32();
}

int32_t NetConnServiceProxy::UpdateNetStateForTest(const sptr<NetSpecifier> &netSpecifier, int32_t netState)
{
    NETMGR_LOG_I("Test NetConnServiceProxy::UpdateNetStateForTest(), begin");
    if (netSpecifier == nullptr) {
        NETMGR_LOG_E("The parameter of netSpecifier is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    netSpecifier->Marshalling(dataParcel);

    if (!dataParcel.WriteInt32(netState)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageOption option;
    MessageParcel replyParcel;
    int32_t retCode = remote->SendRequest(CMD_NM_UPDATE_NET_STATE_FOR_TEST, dataParcel, replyParcel, option);
    NETMGR_LOG_I("NetConnServiceProxy::UpdateNetStateForTest(), SendRequest retCode:[%{public}d]", retCode);
    if (retCode != ERR_NONE) {
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    return replyParcel.ReadInt32();
}

int32_t NetConnServiceProxy::UpdateNetSupplierInfo(uint32_t supplierId, const sptr<NetSupplierInfo> &netSupplierInfo)
{
    if (netSupplierInfo == nullptr) {
        NETMGR_LOG_E("netSupplierInfo is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    NETMGR_LOG_D("proxy supplierId[%{public}d]", supplierId);
    if (!data.WriteUint32(supplierId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    NETMGR_LOG_D("proxy supplierId[%{public}d] Marshalling success", supplierId);
    if (!netSupplierInfo->Marshalling(data)) {
        NETMGR_LOG_E("proxy Marshalling failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    NETMGR_LOG_D("proxy Marshalling success");

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(CMD_NM_SET_NET_SUPPLIER_INFO, data, reply, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    NETMGR_LOG_D("UpdateNetSupplierInfo out.");
    return reply.ReadInt32();
}

int32_t NetConnServiceProxy::UpdateNetLinkInfo(uint32_t supplierId, const sptr<NetLinkInfo> &netLinkInfo)
{
    if (netLinkInfo == nullptr) {
        NETMGR_LOG_E("netLinkInfo is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteUint32(supplierId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!netLinkInfo->Marshalling(data)) {
        NETMGR_LOG_E("proxy Marshalling failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t error = remote->SendRequest(CMD_NM_SET_NET_LINK_INFO, data, reply, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    return reply.ReadInt32();
}

int32_t NetConnServiceProxy::RegisterNetDetectionCallback(int32_t netId, const sptr<INetDetectionCallback> &callback)
{
    if (callback == nullptr) {
        NETMGR_LOG_E("The parameter of callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!dataParcel.WriteInt32(netId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    dataParcel.WriteRemoteObject(callback->AsObject().GetRefPtr());

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = remote->SendRequest(CMD_NM_REGISTER_NET_DETECTION_RET_CALLBACK, dataParcel, replyParcel, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    return replyParcel.ReadInt32();
}

int32_t NetConnServiceProxy::UnRegisterNetDetectionCallback(int32_t netId, const sptr<INetDetectionCallback> &callback)
{
    if (callback == nullptr) {
        NETMGR_LOG_E("The parameter of callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!dataParcel.WriteInt32(netId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    dataParcel.WriteRemoteObject(callback->AsObject().GetRefPtr());

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = remote->SendRequest(CMD_NM_UNREGISTER_NET_DETECTION_RET_CALLBACK, dataParcel, replyParcel, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    return replyParcel.ReadInt32();
}

int32_t NetConnServiceProxy::NetDetection(int32_t netId)
{
    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!dataParcel.WriteInt32(netId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageParcel replyParcel;
    MessageOption option;
    int32_t error = remote->SendRequest(CMD_NM_NET_DETECTION, dataParcel, replyParcel, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    return replyParcel.ReadInt32();
}

int32_t NetConnServiceProxy::GetIfaceNames(NetBearType bearerType, std::list<std::string> &ifaceNames)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteUint32(bearerType)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(CMD_NM_GET_IFACE_NAMES, data, reply, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        uint32_t size = 0;
        if (!reply.ReadUint32(size)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
        size = size > MAX_IFACE_NUM ? MAX_IFACE_NUM : size;
        for (uint32_t i = 0; i < size; ++i) {
            std::string value;
            if (!reply.ReadString(value)) {
                return NETMANAGER_ERR_READ_REPLY_FAIL;
            }
            ifaceNames.push_back(value);
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::GetIfaceNameByType(NetBearType bearerType, const std::string &ident,
                                                std::string &ifaceName)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    uint32_t netType = static_cast<NetBearType>(bearerType);
    if (!data.WriteUint32(netType)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteString(ident)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(CMD_NM_GET_IFACENAME_BY_TYPE, data, reply, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    int32_t ret = 0;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.ReadString(ifaceName)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}

bool NetConnServiceProxy::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(NetConnServiceProxy::GetDescriptor())) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return false;
    }
    return true;
}

int32_t NetConnServiceProxy::GetDefaultNet(int32_t &netId)
{
    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    MessageOption option;
    MessageParcel replyParcel;
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    int32_t errCode = remote->SendRequest(CMD_NM_GETDEFAULTNETWORK, dataParcel, replyParcel, option);
    NETMGR_LOG_D("SendRequest errcode:[%{public}d]", errCode);
    if (errCode != ERR_NONE) {
        return NETMANAGER_ERR_OPERATION_FAILED;
    }
    int32_t ret = 0;
    if (!replyParcel.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!replyParcel.ReadInt32(netId)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::HasDefaultNet(bool &flag)
{
    MessageParcel dataParcel;
    if (!WriteInterfaceToken(dataParcel)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    MessageOption option;
    MessageParcel replyParcel;
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    int32_t retCode = remote->SendRequest(CMD_NM_HASDEFAULTNET, dataParcel, replyParcel, option);
    NETMGR_LOG_D("SendRequest retCode:[%{public}d]", retCode);
    if (retCode != ERR_NONE) {
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    int32_t ret = 0;
    if (!replyParcel.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!replyParcel.ReadBool(flag)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::GetSpecificNet(NetBearType bearerType, std::list<int32_t> &netIdList)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    uint32_t type = static_cast<uint32_t>(bearerType);
    if (!data.WriteUint32(type)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(CMD_NM_GET_SPECIFIC_NET, data, reply, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        int32_t size = 0;
        if (!reply.ReadInt32(size)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
        size = size > MAX_IFACE_NUM ? MAX_IFACE_NUM : size;
        for (int32_t i = 0; i < size; ++i) {
            uint32_t value;
            if (!reply.ReadUint32(value)) {
                return NETMANAGER_ERR_READ_REPLY_FAIL;
            }
            netIdList.push_back(value);
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::GetAllNets(std::list<int32_t> &netIdList)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(CMD_NM_GET_ALL_NETS, data, reply, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        int32_t size;
        if (!reply.ReadInt32(size)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
        size = size > MAX_IFACE_NUM ? MAX_IFACE_NUM : size;
        for (int32_t i = 0; i < size; ++i) {
            uint32_t value;
            if (!reply.ReadUint32(value)) {
                return NETMANAGER_ERR_READ_REPLY_FAIL;
            }
            netIdList.push_back(value);
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::GetSpecificUidNet(int32_t uid, int32_t &netId)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteInt32(uid)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(CMD_NM_GET_SPECIFIC_UID_NET, data, reply, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.ReadInt32(netId)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::GetConnectionProperties(int32_t netId, NetLinkInfo &info)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteInt32(netId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(CMD_NM_GET_CONNECTION_PROPERTIES, data, reply, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        sptr<NetLinkInfo> netLinkInfo_ptr = NetLinkInfo::Unmarshalling(reply);
        if (netLinkInfo_ptr != nullptr) {
            info = *netLinkInfo_ptr;
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::GetNetCapabilities(int32_t netId, NetAllCapabilities &netAllCap)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteInt32(netId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(CMD_NM_GET_NET_CAPABILITIES, data, reply, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return (ret == NETMANAGER_SUCCESS) ? GetNetCapData(reply, netAllCap) : ret;
}

int32_t NetConnServiceProxy::GetNetCapData(MessageParcel &reply, NetAllCapabilities &netAllCap)
{
    if (!reply.ReadUint32(netAllCap.linkUpBandwidthKbps_)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (!reply.ReadUint32(netAllCap.linkDownBandwidthKbps_)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    uint32_t size = 0;
    if (!reply.ReadUint32(size)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    size = size > MAX_NET_CAP_NUM ? MAX_NET_CAP_NUM : size;
    uint32_t value = 0;
    for (uint32_t i = 0; i < size; ++i) {
        if (!reply.ReadUint32(value)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
        netAllCap.netCaps_.insert(static_cast<NetCap>(value));
    }
    if (!reply.ReadUint32(size)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    size = size > MAX_NET_CAP_NUM ? MAX_NET_CAP_NUM : size;
    for (uint32_t i = 0; i < size; ++i) {
        if (!reply.ReadUint32(value)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
        netAllCap.bearerTypes_.insert(static_cast<NetBearType>(value));
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceProxy::GetAddressesByName(const std::string &host, int32_t netId, std::vector<INetAddr> &addrList)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    if (!data.WriteString(host)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteInt32(netId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(CMD_NM_GET_ADDRESSES_BY_NAME, data, reply, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }

    if (ret == NETMANAGER_SUCCESS) {
        int32_t size;
        if (!reply.ReadInt32(size)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
        size = size > MAX_IFACE_NUM ? MAX_IFACE_NUM : size;
        for (int32_t i = 0; i < size; ++i) {
            sptr<INetAddr> netaddr_ptr = INetAddr::Unmarshalling(reply);
            if (netaddr_ptr != nullptr) {
                addrList.push_back(*netaddr_ptr);
            }
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::GetAddressByName(const std::string &host, int32_t netId, INetAddr &addr)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteString(host)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteInt32(netId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(CMD_NM_GET_ADDRESS_BY_NAME, data, reply, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        sptr<INetAddr> netaddr_ptr = INetAddr::Unmarshalling(reply);
        if (netaddr_ptr != nullptr) {
            addr = *netaddr_ptr;
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::BindSocket(int32_t socket_fd, int32_t netId)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteInt32(socket_fd)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    if (!data.WriteInt32(netId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(CMD_NM_BIND_SOCKET, data, reply, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return ret;
}

int32_t NetConnServiceProxy::SetAirplaneMode(bool state)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteBool(state)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(CMD_NM_SET_AIRPLANE_MODE, data, reply, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return ret;
}

int32_t NetConnServiceProxy::IsDefaultNetMetered(bool &isMetered)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(CMD_NM_IS_DEFAULT_NET_METERED, data, reply, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.ReadBool(isMetered)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::SetGlobalHttpProxy(const HttpProxy &httpProxy)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!httpProxy.Marshalling(data)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(CMD_NM_SET_HTTP_PROXY, data, reply, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return ret;
}

int32_t NetConnServiceProxy::GetGlobalHttpProxy(HttpProxy &httpProxy)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(CMD_NM_GET_HTTP_PROXY, data, reply, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }

    if (ret == NETMANAGER_SUCCESS) {
        if (!HttpProxy::Unmarshalling(reply, httpProxy)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::GetNetIdByIdentifier(const std::string &ident, std::list<int32_t> &netIdList)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteString(ident)) {
        NETMGR_LOG_E("Write string data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(CMD_NM_GET_NET_ID_BY_IDENTIFIER, data, reply, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        NETMGR_LOG_E("Read return code failed");
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }

    if (ret == NETMANAGER_SUCCESS) {
        int32_t size = 0;
        if (!reply.ReadInt32(size)) {
            return NETMANAGER_ERR_READ_REPLY_FAIL;
        }
        size = size > MAX_IFACE_NUM ? MAX_IFACE_NUM : size;
        int32_t value = 0;
        for (int32_t i = 0; i < size; ++i) {
            if (!reply.ReadInt32(value)) {
                return NETMANAGER_ERR_READ_REPLY_FAIL;
            }
            netIdList.push_back(value);
        }
    }
    return ret;
}

int32_t NetConnServiceProxy::SetAppNet(int32_t netId)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteInt32(netId)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t error = remote->SendRequest(CMD_NM_SET_APP_NET, data, reply, option);
    if (error != ERR_NONE) {
        NETMGR_LOG_E("proxy SendRequest failed, error code: [%{public}d]", error);
        return NETMANAGER_ERR_OPERATION_FAILED;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    if (!reply.ReadInt32(ret)) {
        return NETMANAGER_ERR_READ_REPLY_FAIL;
    }
    return ret;
}
} // namespace NetManagerStandard
} // namespace OHOS
