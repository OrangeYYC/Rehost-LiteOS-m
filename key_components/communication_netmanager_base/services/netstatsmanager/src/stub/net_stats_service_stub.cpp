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

#include "net_stats_service_stub.h"

#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "netmanager_base_permission.h"

namespace OHOS {
namespace NetManagerStandard {
NetStatsServiceStub::NetStatsServiceStub()
{
    memberFuncMap_[CMD_NSM_REGISTER_NET_STATS_CALLBACK] = &NetStatsServiceStub::OnRegisterNetStatsCallback;
    memberFuncMap_[CMD_NSM_UNREGISTER_NET_STATS_CALLBACK] = &NetStatsServiceStub::OnUnregisterNetStatsCallback;
    memberFuncMap_[CMD_GET_IFACE_RXBYTES] = &NetStatsServiceStub::OnGetIfaceRxBytes;
    memberFuncMap_[CMD_GET_IFACE_TXBYTES] = &NetStatsServiceStub::OnGetIfaceTxBytes;
    memberFuncMap_[CMD_GET_CELLULAR_RXBYTES] = &NetStatsServiceStub::OnGetCellularRxBytes;
    memberFuncMap_[CMD_GET_CELLULAR_TXBYTES] = &NetStatsServiceStub::OnGetCellularTxBytes;
    memberFuncMap_[CMD_GET_ALL_RXBYTES] = &NetStatsServiceStub::OnGetAllRxBytes;
    memberFuncMap_[CMD_GET_ALL_TXBYTES] = &NetStatsServiceStub::OnGetAllTxBytes;
    memberFuncMap_[CMD_GET_UID_RXBYTES] = &NetStatsServiceStub::OnGetUidRxBytes;
    memberFuncMap_[CMD_GET_UID_TXBYTES] = &NetStatsServiceStub::OnGetUidTxBytes;
    memberFuncMap_[CMD_GET_IFACE_STATS_DETAIL] = &NetStatsServiceStub::OnGetIfaceStatsDetail;
    memberFuncMap_[CMD_GET_UID_STATS_DETAIL] = &NetStatsServiceStub::OnGetUidStatsDetail;
    memberFuncMap_[CMD_UPDATE_IFACES_STATS] = &NetStatsServiceStub::OnUpdateIfacesStats;
    memberFuncMap_[CMD_UPDATE_STATS_DATA] = &NetStatsServiceStub::OnUpdateStatsData;
    memberFuncMap_[CMD_NSM_RESET_FACTORY] = &NetStatsServiceStub::OnResetFactory;
}

NetStatsServiceStub::~NetStatsServiceStub() = default;

int32_t NetStatsServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                             MessageOption &option)
{
    NETMGR_LOG_D("stub call start, code = [%{public}d]", code);

    std::u16string myDescripters = NetStatsServiceStub::GetDescriptor();
    std::u16string remoteDescripters = data.ReadInterfaceToken();
    if (myDescripters != remoteDescripters) {
        NETMGR_LOG_D("descriptor checked fail");
        return NETMANAGER_ERR_DESCRIPTOR_MISMATCH;
    }

    auto itFunc = memberFuncMap_.find(code);
    if (itFunc != memberFuncMap_.end()) {
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            return (this->*requestFunc)(data, reply);
        }
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int32_t NetStatsServiceStub::OnRegisterNetStatsCallback(MessageParcel &data, MessageParcel &reply)
{
    if (!NetManagerPermission::IsSystemCaller()) {
        NETMGR_LOG_E("Permission check failed.");
        return NETMANAGER_ERR_NOT_SYSTEM_CALL;
    }
    if (!NetManagerPermission::CheckPermission(Permission::CONNECTIVITY_INTERNAL)) {
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }
    int32_t result = NETMANAGER_ERR_LOCAL_PTR_NULL;
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("Callback ptr is nullptr.");
        reply.WriteInt32(result);
        return result;
    }

    sptr<INetStatsCallback> callback = iface_cast<INetStatsCallback>(remote);
    result = RegisterNetStatsCallback(callback);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsServiceStub::OnUnregisterNetStatsCallback(MessageParcel &data, MessageParcel &reply)
{
    if (!NetManagerPermission::IsSystemCaller()) {
        NETMGR_LOG_E("Permission check failed.");
        return NETMANAGER_ERR_NOT_SYSTEM_CALL;
    }
    if (!NetManagerPermission::CheckPermission(Permission::CONNECTIVITY_INTERNAL)) {
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }
    int32_t result = NETMANAGER_ERR_LOCAL_PTR_NULL;
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("callback ptr is nullptr.");
        reply.WriteInt32(result);
        return result;
    }
    sptr<INetStatsCallback> callback = iface_cast<INetStatsCallback>(remote);
    result = UnregisterNetStatsCallback(callback);
    if (!reply.WriteInt32(result)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return result;
}

int32_t NetStatsServiceStub::OnGetIfaceRxBytes(MessageParcel &data, MessageParcel &reply)
{
    uint64_t stats = 0;
    std::string iface;
    if (!data.ReadString(iface)) {
        NETMGR_LOG_E("Read string failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    int32_t result = GetIfaceRxBytes(stats, iface);
    if (!reply.WriteUint64(stats)) {
        NETMGR_LOG_E("WriteUint64 failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("WriteInt32 failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsServiceStub::OnGetIfaceTxBytes(MessageParcel &data, MessageParcel &reply)
{
    uint64_t stats = 0;
    std::string iface;
    if (!data.ReadString(iface)) {
        NETMGR_LOG_E("Read string failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = GetIfaceTxBytes(stats, iface);
    if (!reply.WriteUint64(stats)) {
        NETMGR_LOG_E("WriteUint64 failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("WriteInt32 failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetStatsServiceStub::OnGetCellularRxBytes(MessageParcel &data, MessageParcel &reply)
{
    uint64_t stats = 0;
    int32_t ret = GetCellularRxBytes(stats);
    if (!reply.WriteUint64(stats)) {
        NETMGR_LOG_E("WriteUint64 failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (!reply.WriteInt32(ret)) {
        NETMGR_LOG_E("WriteInt32 failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetStatsServiceStub::OnGetCellularTxBytes(MessageParcel &data, MessageParcel &reply)
{
    uint64_t stats = 0;
    int32_t ret = GetCellularTxBytes(stats);
    if (!reply.WriteUint64(stats)) {
        NETMGR_LOG_E("WriteUint64 failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (!reply.WriteInt32(ret)) {
        NETMGR_LOG_E("WriteInt32 failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetStatsServiceStub::OnGetAllRxBytes(MessageParcel &data, MessageParcel &reply)
{
    uint64_t stats = 0;
    int32_t ret = GetAllRxBytes(stats);
    if (!reply.WriteUint64(stats)) {
        NETMGR_LOG_E("WriteUint64 failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (!reply.WriteInt32(ret)) {
        NETMGR_LOG_E("WriteInt32 failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetStatsServiceStub::OnGetAllTxBytes(MessageParcel &data, MessageParcel &reply)
{
    uint64_t stats = 0;
    int32_t ret = GetAllTxBytes(stats);
    if (!reply.WriteUint64(stats)) {
        NETMGR_LOG_E("WriteUint64 failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (!reply.WriteInt32(ret)) {
        NETMGR_LOG_E("WriteInt32 failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetStatsServiceStub::OnGetUidRxBytes(MessageParcel &data, MessageParcel &reply)
{
    uint32_t uid;
    uint64_t stats = 0;
    if (!data.ReadUint32(uid)) {
        NETMGR_LOG_E("ReadInt32 failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = GetUidRxBytes(stats, uid);
    if (!reply.WriteUint64(stats)) {
        NETMGR_LOG_E("WriteUint64 failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("WriteInt32 failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsServiceStub::OnGetUidTxBytes(MessageParcel &data, MessageParcel &reply)
{
    uint32_t uid;
    uint64_t stats = 0;
    if (!data.ReadUint32(uid)) {
        NETMGR_LOG_E("ReadInt32 failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = GetUidTxBytes(stats, uid);
    if (!reply.WriteUint64(stats)) {
        NETMGR_LOG_E("WriteUint64 failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("WriteInt32 failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsServiceStub::OnGetIfaceStatsDetail(MessageParcel &data, MessageParcel &reply)
{
    if (!NetManagerPermission::IsSystemCaller()) {
        NETMGR_LOG_E("Permission check failed.");
        return NETMANAGER_ERR_NOT_SYSTEM_CALL;
    }
    if (!NetManagerPermission::CheckPermission(Permission::CONNECTIVITY_INTERNAL)) {
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }
    std::string iface;
    uint64_t start = 0;
    uint64_t end = 0;
    if (!(data.ReadString(iface) && data.ReadUint64(start) && data.ReadUint64(end))) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    NetStatsInfo info;
    int32_t ret = GetIfaceStatsDetail(iface, start, end, info);
    if (!info.Marshalling(reply)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsServiceStub::OnGetUidStatsDetail(MessageParcel &data, MessageParcel &reply)
{
    if (!NetManagerPermission::IsSystemCaller()) {
        NETMGR_LOG_E("Permission check failed.");
        return NETMANAGER_ERR_NOT_SYSTEM_CALL;
    }
    if (!NetManagerPermission::CheckPermission(Permission::CONNECTIVITY_INTERNAL)) {
        return NETMANAGER_ERR_PERMISSION_DENIED;
    }
    std::string iface;
    uint32_t uid = 0;
    uint64_t start = 0;
    uint64_t end = 0;
    if (!(data.ReadString(iface) && data.ReadUint32(uid) && data.ReadUint64(start) && data.ReadUint64(end))) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }
    NetStatsInfo info;
    int32_t ret = GetUidStatsDetail(iface, uid, start, end, info);
    if (!info.Marshalling(reply)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsServiceStub::OnUpdateIfacesStats(MessageParcel &data, MessageParcel &reply)
{
    std::string iface;
    uint64_t start = 0;
    uint64_t end = 0;
    if (!(data.ReadString(iface) && data.ReadUint64(start) && data.ReadUint64(end))) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    NetStatsInfo infos;
    if (!NetStatsInfo::Unmarshalling(data, infos)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t ret = UpdateIfacesStats(iface, start, end, infos);
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsServiceStub::OnUpdateStatsData(MessageParcel &data, MessageParcel &reply)
{
    int32_t ret = UpdateStatsData();
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetStatsServiceStub::OnResetFactory(MessageParcel &data, MessageParcel &reply)
{
    int32_t ret = ResetFactory();
    if (!reply.WriteInt32(ret)) {
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}
} // namespace NetManagerStandard
} // namespace OHOS
