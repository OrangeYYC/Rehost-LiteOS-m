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

#include "netmanager_base_permission.h"
#include "net_conn_constants.h"
#include "net_conn_service_stub.h"
#include "net_conn_types.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
static constexpr uint32_t MAX_IFACE_NUM = 16;
static constexpr uint32_t MAX_NET_CAP_NUM = 32;

NetConnServiceStub::NetConnServiceStub()
{
    memberFuncMap_[CMD_NM_SYSTEM_READY] = {&NetConnServiceStub::OnSystemReady, {}};
    memberFuncMap_[CMD_NM_REGISTER_NET_CONN_CALLBACK] = {&NetConnServiceStub::OnRegisterNetConnCallback,
                                                         {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[CMD_NM_REGISTER_NET_CONN_CALLBACK_BY_SPECIFIER] = {
        &NetConnServiceStub::OnRegisterNetConnCallbackBySpecifier, {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[CMD_NM_UNREGISTER_NET_CONN_CALLBACK] = {&NetConnServiceStub::OnUnregisterNetConnCallback,
                                                           {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[CMD_NM_UPDATE_NET_STATE_FOR_TEST] = {&NetConnServiceStub::OnUpdateNetStateForTest, {}};
    memberFuncMap_[CMD_NM_REG_NET_SUPPLIER] = {&NetConnServiceStub::OnRegisterNetSupplier, {}};
    memberFuncMap_[CMD_NM_UNREG_NETWORK] = {&NetConnServiceStub::OnUnregisterNetSupplier, {}};
    memberFuncMap_[CMD_NM_SET_NET_SUPPLIER_INFO] = {&NetConnServiceStub::OnUpdateNetSupplierInfo, {}};
    memberFuncMap_[CMD_NM_SET_NET_LINK_INFO] = {&NetConnServiceStub::OnUpdateNetLinkInfo, {}};
    memberFuncMap_[CMD_NM_REGISTER_NET_DETECTION_RET_CALLBACK] = {&NetConnServiceStub::OnRegisterNetDetectionCallback,
                                                                  {}};
    memberFuncMap_[CMD_NM_UNREGISTER_NET_DETECTION_RET_CALLBACK] = {
        &NetConnServiceStub::OnUnRegisterNetDetectionCallback, {}};
    memberFuncMap_[CMD_NM_NET_DETECTION] = {&NetConnServiceStub::OnNetDetection,
                                            {Permission::GET_NETWORK_INFO, Permission::INTERNET}};
    memberFuncMap_[CMD_NM_GET_IFACE_NAMES] = {&NetConnServiceStub::OnGetIfaceNames, {}};
    memberFuncMap_[CMD_NM_GET_IFACENAME_BY_TYPE] = {&NetConnServiceStub::OnGetIfaceNameByType, {}};
    memberFuncMap_[CMD_NM_GETDEFAULTNETWORK] = {&NetConnServiceStub::OnGetDefaultNet, {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[CMD_NM_HASDEFAULTNET] = {&NetConnServiceStub::OnHasDefaultNet, {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[CMD_NM_GET_SPECIFIC_NET] = {&NetConnServiceStub::OnGetSpecificNet, {}};
    memberFuncMap_[CMD_NM_GET_ALL_NETS] = {&NetConnServiceStub::OnGetAllNets, {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[CMD_NM_GET_SPECIFIC_UID_NET] = {&NetConnServiceStub::OnGetSpecificUidNet, {}};
    memberFuncMap_[CMD_NM_GET_CONNECTION_PROPERTIES] = {&NetConnServiceStub::OnGetConnectionProperties,
                                                        {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[CMD_NM_GET_NET_CAPABILITIES] = {&NetConnServiceStub::OnGetNetCapabilities,
                                                   {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[CMD_NM_GET_ADDRESSES_BY_NAME] = {&NetConnServiceStub::OnGetAddressesByName,
                                                    {Permission::INTERNET}};
    memberFuncMap_[CMD_NM_GET_ADDRESS_BY_NAME] = {&NetConnServiceStub::OnGetAddressByName,
                                                  {Permission::INTERNET}};
    memberFuncMap_[CMD_NM_BIND_SOCKET] = {&NetConnServiceStub::OnBindSocket, {}};
    memberFuncMap_[CMD_NM_REGISTER_NET_SUPPLIER_CALLBACK] = {&NetConnServiceStub::OnRegisterNetSupplierCallback, {}};
    memberFuncMap_[CMD_NM_SET_AIRPLANE_MODE] = {&NetConnServiceStub::OnSetAirplaneMode,
                                                {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[CMD_NM_IS_DEFAULT_NET_METERED] = {&NetConnServiceStub::OnIsDefaultNetMetered,
                                                     {Permission::GET_NETWORK_INFO}};
    memberFuncMap_[CMD_NM_SET_HTTP_PROXY] = {&NetConnServiceStub::OnSetGlobalHttpProxy,
                                             {Permission::CONNECTIVITY_INTERNAL}};
    memberFuncMap_[CMD_NM_GET_HTTP_PROXY] = {&NetConnServiceStub::OnGetGlobalHttpProxy, {}};
    memberFuncMap_[CMD_NM_GET_NET_ID_BY_IDENTIFIER] = {&NetConnServiceStub::OnGetNetIdByIdentifier, {}};
    memberFuncMap_[CMD_NM_SET_APP_NET] = {&NetConnServiceStub::OnSetAppNet, {Permission::INTERNET}};
}

NetConnServiceStub::~NetConnServiceStub() {}

std::string ToUtf8(std::u16string str16)
{
    return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(str16);
}

int32_t NetConnServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                            MessageOption &option)
{
    NETMGR_LOG_D("stub call start, code = [%{public}d]", code);

    std::u16string myDescripter = NetConnServiceStub::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    NETMGR_LOG_D("myDescripter[%{public}s], remoteDescripter[%{public}s]", ToUtf8(myDescripter).c_str(),
                 ToUtf8(remoteDescripter).c_str());
    if (myDescripter != remoteDescripter) {
        NETMGR_LOG_E("descriptor checked fail");
        if (!reply.WriteInt32(NETMANAGER_ERR_DESCRIPTOR_MISMATCH)) {
            return IPC_STUB_WRITE_PARCEL_ERR;
        }
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    auto itFunc = memberFuncMap_.find(code);
    if (itFunc == memberFuncMap_.end()) {
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    auto requestFunc = itFunc->second.first;
    if (requestFunc == nullptr) {
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    if (code != CMD_NM_GETDEFAULTNETWORK && CheckPermission(itFunc->second.second)) {
        return (this->*requestFunc)(data, reply);
    }
    if (code == CMD_NM_GETDEFAULTNETWORK && CheckPermissionWithCache(itFunc->second.second)) {
        return (this->*requestFunc)(data, reply);
    }
    if (!reply.WriteInt32(NETMANAGER_ERR_PERMISSION_DENIED)) {
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    NETMGR_LOG_D("stub default case, need check");
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

bool NetConnServiceStub::CheckPermission(const std::set<std::string> &permissions)
{
    for (const auto &permission : permissions) {
        if (!NetManagerPermission::CheckPermission(permission)) {
            return false;
        }
    }
    return true;
}

bool NetConnServiceStub::CheckPermissionWithCache(const std::set<std::string> &permissions)
{
    for (const auto &permission : permissions) {
        if (!NetManagerPermission::CheckPermissionWithCache(permission)) {
            return false;
        }
    }
    return true;
}

int32_t NetConnServiceStub::OnSystemReady(MessageParcel &data, MessageParcel &reply)
{
    SystemReady();
    return 0;
}

int32_t NetConnServiceStub::OnRegisterNetSupplier(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("stub processing");
    NetBearType bearerType;
    std::string ident;
    std::set<NetCap> netCaps;

    uint32_t type = 0;
    if (!data.ReadUint32(type)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    bearerType = static_cast<NetBearType>(type);

    if (!data.ReadString(ident)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    uint32_t size = 0;
    uint32_t value = 0;
    if (!data.ReadUint32(size)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    size = size > MAX_NET_CAP_NUM ? MAX_NET_CAP_NUM : size;
    for (uint32_t i = 0; i < size; ++i) {
        if (!data.ReadUint32(value)) {
            return NETMANAGER_ERR_READ_DATA_FAIL;
        }
        netCaps.insert(static_cast<NetCap>(value));
    }

    uint32_t supplierId = 0;
    int32_t ret = RegisterNetSupplier(bearerType, ident, netCaps, supplierId);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("supplierId[%{public}d].", supplierId);
        if (!reply.WriteUint32(supplierId)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnUnregisterNetSupplier(MessageParcel &data, MessageParcel &reply)
{
    uint32_t supplierId;
    if (!data.ReadUint32(supplierId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = UnregisterNetSupplier(supplierId);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnRegisterNetSupplierCallback(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = NETMANAGER_SUCCESS;
    uint32_t supplierId;
    data.ReadUint32(supplierId);
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("Callback ptr is nullptr.");
        result = NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
        reply.WriteInt32(result);
        return result;
    }

    sptr<INetSupplierCallback> callback = iface_cast<INetSupplierCallback>(remote);
    if (callback == nullptr) {
        result = NETMANAGER_ERR_LOCAL_PTR_NULL;
        reply.WriteInt32(result);
        return result;
    }

    result = RegisterNetSupplierCallback(supplierId, callback);
    reply.WriteInt32(result);
    return result;
}

int32_t NetConnServiceStub::OnRegisterNetConnCallback(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = NETMANAGER_SUCCESS;
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("Callback ptr is nullptr.");
        result = NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
        reply.WriteInt32(result);
        return result;
    }

    sptr<INetConnCallback> callback = iface_cast<INetConnCallback>(remote);
    if (callback == nullptr) {
        result = NETMANAGER_ERR_LOCAL_PTR_NULL;
        reply.WriteInt32(result);
        return result;
    }

    result = RegisterNetConnCallback(callback);
    reply.WriteInt32(result);
    return result;
}

int32_t NetConnServiceStub::OnRegisterNetConnCallbackBySpecifier(MessageParcel &data, MessageParcel &reply)
{
    sptr<NetSpecifier> netSpecifier = NetSpecifier::Unmarshalling(data);
    uint32_t timeoutMS = data.ReadUint32();
    int32_t result = NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("callback ptr is nullptr.");
        reply.WriteInt32(result);
        return result;
    }

    sptr<INetConnCallback> callback = iface_cast<INetConnCallback>(remote);
    if (callback == nullptr) {
        result = NETMANAGER_ERR_LOCAL_PTR_NULL;
        reply.WriteInt32(result);
        return result;
    }

    result = RegisterNetConnCallback(netSpecifier, callback, timeoutMS);
    reply.WriteInt32(result);
    return result;
}

int32_t NetConnServiceStub::OnUnregisterNetConnCallback(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("callback ptr is nullptr.");
        reply.WriteInt32(result);
        return result;
    }

    sptr<INetConnCallback> callback = iface_cast<INetConnCallback>(remote);
    if (callback == nullptr) {
        result = NETMANAGER_ERR_LOCAL_PTR_NULL;
        reply.WriteInt32(result);
        return result;
    }

    result = UnregisterNetConnCallback(callback);
    reply.WriteInt32(result);
    return result;
}

int32_t NetConnServiceStub::OnUpdateNetStateForTest(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_I("Test NetConnServiceStub::OnUpdateNetStateForTest(), begin");
    sptr<NetSpecifier> netSpecifier = NetSpecifier::Unmarshalling(data);

    int32_t netState;
    if (!data.ReadInt32(netState)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    NETMGR_LOG_I("Test NetConnServiceStub::OnUpdateNetStateForTest(), netState[%{public}d]", netState);
    int32_t result = UpdateNetStateForTest(netSpecifier, netState);
    NETMGR_LOG_I("Test NetConnServiceStub::OnUpdateNetStateForTest(), result[%{public}d]", result);
    reply.WriteInt32(result);
    return result;
}

int32_t NetConnServiceStub::OnUpdateNetSupplierInfo(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("OnUpdateNetSupplierInfo in.");
    uint32_t supplierId;
    if (!data.ReadUint32(supplierId)) {
        NETMGR_LOG_D("fail to get supplier id.");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    NETMGR_LOG_D("OnUpdateNetSupplierInfo supplierId=[%{public}d].", supplierId);
    sptr<NetSupplierInfo> netSupplierInfo = NetSupplierInfo::Unmarshalling(data);
    int32_t ret = UpdateNetSupplierInfo(supplierId, netSupplierInfo);
    if (!reply.WriteInt32(ret)) {
        NETMGR_LOG_D("fail to update net supplier info.");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    NETMGR_LOG_D("OnUpdateNetSupplierInfo out.");

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnUpdateNetLinkInfo(MessageParcel &data, MessageParcel &reply)
{
    uint32_t supplierId;

    if (!data.ReadUint32(supplierId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    sptr<NetLinkInfo> netLinkInfo = NetLinkInfo::Unmarshalling(data);

    int32_t ret = UpdateNetLinkInfo(supplierId, netLinkInfo);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnRegisterNetDetectionCallback(MessageParcel &data, MessageParcel &reply)
{
    if (!data.ContainFileDescriptors()) {
        NETMGR_LOG_E("Execute ContainFileDescriptors failed");
    }
    int32_t netId = 0;
    if (!data.ReadInt32(netId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = NETMANAGER_SUCCESS;
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("Callback ptr is nullptr.");
        result = NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
        reply.WriteInt32(result);
        return result;
    }

    sptr<INetDetectionCallback> callback = iface_cast<INetDetectionCallback>(remote);
    if (callback == nullptr) {
        result = NETMANAGER_ERR_LOCAL_PTR_NULL;
        reply.WriteInt32(result);
        return result;
    }

    result = RegisterNetDetectionCallback(netId, callback);
    reply.WriteInt32(result);
    return result;
}

int32_t NetConnServiceStub::OnUnRegisterNetDetectionCallback(MessageParcel &data, MessageParcel &reply)
{
    if (!data.ContainFileDescriptors()) {
        NETMGR_LOG_E("Execute ContainFileDescriptors failed");
    }
    int32_t netId = 0;
    if (!data.ReadInt32(netId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = NETMANAGER_SUCCESS;
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("Callback ptr is nullptr.");
        result = NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
        reply.WriteInt32(result);
        return result;
    }

    sptr<INetDetectionCallback> callback = iface_cast<INetDetectionCallback>(remote);
    if (callback == nullptr) {
        result = NETMANAGER_ERR_LOCAL_PTR_NULL;
        reply.WriteInt32(result);
        return result;
    }

    result = UnRegisterNetDetectionCallback(netId, callback);
    reply.WriteInt32(result);
    return result;
}

int32_t NetConnServiceStub::OnNetDetection(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = 0;
    if (!data.ReadInt32(netId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int32_t ret = NetDetection(netId);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetIfaceNames(MessageParcel &data, MessageParcel &reply)
{
    uint32_t netType = 0;
    if (!data.ReadUint32(netType)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    NetBearType bearerType = static_cast<NetBearType>(netType);
    std::list<std::string> ifaceNames;
    int32_t ret = GetIfaceNames(bearerType, ifaceNames);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.WriteUint32(ifaceNames.size())) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }

        for (const auto &ifaceName : ifaceNames) {
            if (!reply.WriteString(ifaceName)) {
                return NETMANAGER_ERR_WRITE_REPLY_FAIL;
            }
        }
    }
    return ret;
}

int32_t NetConnServiceStub::OnGetIfaceNameByType(MessageParcel &data, MessageParcel &reply)
{
    uint32_t netType = 0;
    if (!data.ReadUint32(netType)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    NetBearType bearerType = static_cast<NetBearType>(netType);

    std::string ident;
    if (!data.ReadString(ident)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string ifaceName;
    int32_t ret = GetIfaceNameByType(bearerType, ident, ifaceName);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.WriteString(ifaceName)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServiceStub::OnGetDefaultNet(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("OnGetDefaultNet Begin...");
    int32_t netId;
    int32_t result = GetDefaultNet(netId);
    NETMGR_LOG_D("GetDefaultNet result is: [%{public}d]", result);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (result == NETMANAGER_SUCCESS) {
        if (!reply.WriteUint32(netId)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnHasDefaultNet(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("OnHasDefaultNet Begin...");
    bool flag = false;
    int32_t result = HasDefaultNet(flag);
    NETMGR_LOG_D("HasDefaultNet result is: [%{public}d]", result);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (result == NETMANAGER_SUCCESS) {
        if (!reply.WriteBool(flag)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetSpecificNet(MessageParcel &data, MessageParcel &reply)
{
    uint32_t type;
    if (!data.ReadUint32(type)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    NetBearType bearerType = static_cast<NetBearType>(type);

    NETMGR_LOG_D("stub execute GetSpecificNet");
    std::list<int32_t> netIdList;
    int32_t ret = GetSpecificNet(bearerType, netIdList);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        uint32_t size = static_cast<uint32_t>(netIdList.size());
        size = size > MAX_IFACE_NUM ? MAX_IFACE_NUM : size;
        if (!reply.WriteInt32(size)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }

        uint32_t index = 0;
        for (auto p = netIdList.begin(); p != netIdList.end(); ++p) {
            if (++index > MAX_IFACE_NUM) {
                break;
            }
            if (!reply.WriteInt32(*p)) {
                return NETMANAGER_ERR_WRITE_REPLY_FAIL;
            }
        }
    }
    return ret;
}

int32_t NetConnServiceStub::OnGetAllNets(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("stub execute GetAllNets");
    std::list<int32_t> netIdList;
    int32_t ret = GetAllNets(netIdList);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        int32_t size = static_cast<int32_t>(netIdList.size());
        if (!reply.WriteInt32(size)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }

        for (auto p = netIdList.begin(); p != netIdList.end(); ++p) {
            if (!reply.WriteInt32(*p)) {
                return NETMANAGER_ERR_WRITE_REPLY_FAIL;
            }
        }
    }
    return ret;
}

int32_t NetConnServiceStub::OnGetSpecificUidNet(MessageParcel &data, MessageParcel &reply)
{
    int32_t uid = 0;
    if (!data.ReadInt32(uid)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    NETMGR_LOG_D("stub execute GetSpecificUidNet");

    int32_t netId = 0;
    int32_t ret = GetSpecificUidNet(uid, netId);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.WriteInt32(netId)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServiceStub::OnGetConnectionProperties(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = 0;
    if (!data.ReadInt32(netId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    NETMGR_LOG_D("stub execute GetConnectionProperties");
    NetLinkInfo info;
    int32_t ret = GetConnectionProperties(netId, info);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        sptr<NetLinkInfo> netLinkInfo_ptr = new (std::nothrow) NetLinkInfo(info);
        if (!NetLinkInfo::Marshalling(reply, netLinkInfo_ptr)) {
            NETMGR_LOG_E("proxy Marshalling failed");
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServiceStub::OnGetNetCapabilities(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = 0;
    if (!data.ReadInt32(netId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    NETMGR_LOG_D("stub execute GetNetCapabilities");

    NetAllCapabilities netAllCap;
    int32_t ret = GetNetCapabilities(netId, netAllCap);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        if (!reply.WriteUint32(netAllCap.linkUpBandwidthKbps_) ||
            !reply.WriteUint32(netAllCap.linkDownBandwidthKbps_)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        uint32_t size = netAllCap.netCaps_.size();
        size = size > MAX_NET_CAP_NUM ? MAX_NET_CAP_NUM : size;
        if (!reply.WriteUint32(size)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        uint32_t index = 0;
        for (auto netCap : netAllCap.netCaps_) {
            if (++index > MAX_NET_CAP_NUM) {
                break;
            }
            if (!reply.WriteUint32(static_cast<uint32_t>(netCap))) {
                return NETMANAGER_ERR_WRITE_REPLY_FAIL;
            }
        }

        size = netAllCap.bearerTypes_.size();
        size = size > MAX_NET_CAP_NUM ? MAX_NET_CAP_NUM : size;
        if (!reply.WriteUint32(size)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        index = 0;
        for (auto bearerType : netAllCap.bearerTypes_) {
            if (++index > MAX_NET_CAP_NUM) {
                break;
            }
            if (!reply.WriteUint32(static_cast<uint32_t>(bearerType))) {
                return NETMANAGER_ERR_WRITE_REPLY_FAIL;
            }
        }
    }
    return ret;
}

int32_t NetConnServiceStub::OnGetAddressesByName(MessageParcel &data, MessageParcel &reply)
{
    std::string host;
    if (!data.ReadString(host)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int32_t netId;
    if (!data.ReadInt32(netId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    NETMGR_LOG_D("stub execute GetAddressesByName");
    std::vector<INetAddr> addrList;
    int32_t ret = GetAddressesByName(host, netId, addrList);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        uint32_t size = static_cast<uint32_t>(addrList.size());
        size = size > MAX_IFACE_NUM ? MAX_IFACE_NUM : size;
        if (!reply.WriteInt32(size)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        uint32_t index = 0;
        for (auto p = addrList.begin(); p != addrList.end(); ++p) {
            if (++index > MAX_IFACE_NUM) {
                break;
            }
            sptr<INetAddr> netaddr_ptr = (std::make_unique<INetAddr>(*p)).release();
            if (!INetAddr::Marshalling(reply, netaddr_ptr)) {
                NETMGR_LOG_E("proxy Marshalling failed");
                return NETMANAGER_ERR_WRITE_REPLY_FAIL;
            }
        }
    }
    return ret;
}

int32_t NetConnServiceStub::OnGetAddressByName(MessageParcel &data, MessageParcel &reply)
{
    std::string host;
    if (!data.ReadString(host)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int32_t netId;
    if (!data.ReadInt32(netId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    NETMGR_LOG_D("stub execute GetAddressByName");
    INetAddr addr;
    int32_t ret = GetAddressByName(host, netId, addr);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (ret == NETMANAGER_SUCCESS) {
        sptr<INetAddr> netaddr_ptr = (std::make_unique<INetAddr>(addr)).release();
        if (!INetAddr::Marshalling(reply, netaddr_ptr)) {
            NETMGR_LOG_E("proxy Marshalling failed");
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    return ret;
}

int32_t NetConnServiceStub::OnBindSocket(MessageParcel &data, MessageParcel &reply)
{
    int32_t socket_fd;
    if (!data.ReadInt32(socket_fd)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int32_t netId;
    if (!data.ReadInt32(netId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    NETMGR_LOG_D("stub execute BindSocket");

    int32_t ret = BindSocket(socket_fd, netId);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return ret;
}

int32_t NetConnServiceStub::OnSetAirplaneMode(MessageParcel &data, MessageParcel &reply)
{
    bool state = false;
    if (!data.ReadBool(state)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int32_t ret = SetAirplaneMode(state);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return ret;
}

int32_t NetConnServiceStub::OnIsDefaultNetMetered(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("stub execute IsDefaultNetMetered");
    bool flag = false;
    int32_t result = IsDefaultNetMetered(flag);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    if (result == NETMANAGER_SUCCESS) {
        if (!reply.WriteBool(flag)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnSetGlobalHttpProxy(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("stub execute SetGlobalHttpProxy");

    HttpProxy httpProxy;
    if (!HttpProxy::Unmarshalling(data, httpProxy)) {
        return ERR_FLATTEN_OBJECT;
    }

    int32_t ret = SetGlobalHttpProxy(httpProxy);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return ret;
}

int32_t NetConnServiceStub::OnGetGlobalHttpProxy(MessageParcel &data, MessageParcel &reply)
{
    HttpProxy httpProxy;
    int32_t result = GetGlobalHttpProxy(httpProxy);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (result != NETMANAGER_SUCCESS) {
        return result;
    }

    if (!httpProxy.Marshalling(reply)) {
        return ERR_FLATTEN_OBJECT;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetConnServiceStub::OnGetNetIdByIdentifier(MessageParcel &data, MessageParcel &reply)
{
    NETMGR_LOG_D("stub execute OnGetNetIdByIdentifier");
    std::string ident;
    if (!data.ReadString(ident)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::list<int32_t> netIdList;
    int32_t ret = GetNetIdByIdentifier(ident, netIdList);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (ret == NETMANAGER_SUCCESS) {
        int32_t size = static_cast<int32_t>(netIdList.size());
        if (!reply.WriteInt32(size)) {
            return NETMANAGER_ERR_WRITE_REPLY_FAIL;
        }
        for (auto p = netIdList.begin(); p != netIdList.end(); ++p) {
            if (!reply.WriteInt32(*p)) {
                return NETMANAGER_ERR_WRITE_REPLY_FAIL;
            }
        }
    }
    return ret;
}

int32_t NetConnServiceStub::OnSetAppNet(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = 0;
    if (!data.ReadInt32(netId)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int ret = SetAppNet(netId);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return ret;
}
} // namespace NetManagerStandard
} // namespace OHOS
