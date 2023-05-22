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

#include "net_policy_service_stub.h"

#include "net_mgr_log_wrapper.h"
#include "net_policy_core.h"
#include "net_quota_policy.h"
#include "netmanager_base_permission.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
std::map<uint32_t, const char *> g_codeNPS = {
    {INetPolicyService::CMD_NPS_SET_POLICY_BY_UID, Permission::SET_NETWORK_POLICY},
    {INetPolicyService::CMD_NPS_SET_NET_QUOTA_POLICIES, Permission::SET_NETWORK_POLICY},
    {INetPolicyService::CMD_NPS_RESET_POLICIES, Permission::SET_NETWORK_POLICY},
    {INetPolicyService::CMD_NPS_SET_BACKGROUND_POLICY, Permission::SET_NETWORK_POLICY},
    {INetPolicyService::CMD_NPS_UPDATE_REMIND_POLICY, Permission::SET_NETWORK_POLICY},

    {INetPolicyService::CMD_NPS_GET_POLICY_BY_UID, Permission::GET_NETWORK_POLICY},
    {INetPolicyService::CMD_NPS_GET_UIDS_BY_POLICY, Permission::GET_NETWORK_POLICY},
    {INetPolicyService::CMD_NPS_GET_NET_QUOTA_POLICIES, Permission::GET_NETWORK_POLICY},
    {INetPolicyService::CMD_NPS_GET_BACKGROUND_POLICY, Permission::GET_NETWORK_POLICY},

    {INetPolicyService::CMD_NPS_IS_NET_ALLOWED_BY_METERED, Permission::CONNECTIVITY_INTERNAL},
    {INetPolicyService::CMD_NPS_IS_NET_ALLOWED_BY_IFACE, Permission::CONNECTIVITY_INTERNAL},
    {INetPolicyService::CMD_NPS_REGISTER_NET_POLICY_CALLBACK, Permission::CONNECTIVITY_INTERNAL},
    {INetPolicyService::CMD_NPS_UNREGISTER_NET_POLICY_CALLBACK, Permission::CONNECTIVITY_INTERNAL},
    {INetPolicyService::CMD_NPS_GET_BACKGROUND_POLICY_BY_UID, Permission::CONNECTIVITY_INTERNAL},
    {INetPolicyService::CMD_NPS_SET_IDLE_ALLOWED_LIST, Permission::CONNECTIVITY_INTERNAL},
    {INetPolicyService::CMD_NPS_GET_IDLE_ALLOWED_LIST, Permission::CONNECTIVITY_INTERNAL},
    {INetPolicyService::CMD_NPS_SET_DEVICE_IDLE_POLICY, Permission::CONNECTIVITY_INTERNAL},
};
} // namespace

NetPolicyServiceStub::NetPolicyServiceStub()
{
    memberFuncMap_[CMD_NPS_SET_POLICY_BY_UID] = &NetPolicyServiceStub::OnSetPolicyByUid;
    memberFuncMap_[CMD_NPS_GET_POLICY_BY_UID] = &NetPolicyServiceStub::OnGetPolicyByUid;
    memberFuncMap_[CMD_NPS_GET_UIDS_BY_POLICY] = &NetPolicyServiceStub::OnGetUidsByPolicy;
    memberFuncMap_[CMD_NPS_IS_NET_ALLOWED_BY_METERED] = &NetPolicyServiceStub::OnIsUidNetAllowedMetered;
    memberFuncMap_[CMD_NPS_IS_NET_ALLOWED_BY_IFACE] = &NetPolicyServiceStub::OnIsUidNetAllowedIfaceName;
    memberFuncMap_[CMD_NPS_REGISTER_NET_POLICY_CALLBACK] = &NetPolicyServiceStub::OnRegisterNetPolicyCallback;
    memberFuncMap_[CMD_NPS_UNREGISTER_NET_POLICY_CALLBACK] = &NetPolicyServiceStub::OnUnregisterNetPolicyCallback;
    memberFuncMap_[CMD_NPS_SET_NET_QUOTA_POLICIES] = &NetPolicyServiceStub::OnSetNetQuotaPolicies;
    memberFuncMap_[CMD_NPS_GET_NET_QUOTA_POLICIES] = &NetPolicyServiceStub::OnGetNetQuotaPolicies;
    memberFuncMap_[CMD_NPS_RESET_POLICIES] = &NetPolicyServiceStub::OnResetPolicies;
    memberFuncMap_[CMD_NPS_UPDATE_REMIND_POLICY] = &NetPolicyServiceStub::OnSnoozePolicy;
    memberFuncMap_[CMD_NPS_SET_IDLE_ALLOWED_LIST] = &NetPolicyServiceStub::OnSetDeviceIdleAllowedList;
    memberFuncMap_[CMD_NPS_GET_IDLE_ALLOWED_LIST] = &NetPolicyServiceStub::OnGetDeviceIdleAllowedList;
    memberFuncMap_[CMD_NPS_SET_DEVICE_IDLE_POLICY] = &NetPolicyServiceStub::OnSetDeviceIdlePolicy;
    memberFuncMap_[CMD_NPS_SET_BACKGROUND_POLICY] = &NetPolicyServiceStub::OnSetBackgroundPolicy;
    memberFuncMap_[CMD_NPS_GET_BACKGROUND_POLICY] = &NetPolicyServiceStub::OnGetBackgroundPolicy;
    memberFuncMap_[CMD_NPS_GET_BACKGROUND_POLICY_BY_UID] = &NetPolicyServiceStub::OnGetBackgroundPolicyByUid;
    InitEventHandler();
}

NetPolicyServiceStub::~NetPolicyServiceStub() = default;

void NetPolicyServiceStub::InitEventHandler()
{
    runner_ = AppExecFwk::EventRunner::Create(NET_POLICY_WORK_THREAD);
    if (!runner_) {
        NETMGR_LOG_E("Create net policy work event runner.");
        return;
    }
    auto core = DelayedSingleton<NetPolicyCore>::GetInstance();
    handler_ = std::make_shared<NetPolicyEventHandler>(runner_, core);
    core->Init(handler_);
}

int32_t NetPolicyServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                              MessageOption &option)
{
    std::u16string myDescriptor = NetPolicyServiceStub::GetDescriptor();
    std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (myDescriptor != remoteDescriptor) {
        NETMGR_LOG_E("descriptor checked fail");
        return NETMANAGER_ERR_DESCRIPTOR_MISMATCH;
    }

    if (handler_ == nullptr) {
        NETMGR_LOG_E("Net policy handler is null, recreate handler.");
        InitEventHandler();
        if (handler_ == nullptr) {
            NETMGR_LOG_E("recreate net policy handler failed.");
            return NETMANAGER_ERR_INTERNAL;
        }
    }
    int32_t checkPermissionResult = CheckPolicyPermission(code);
    if (checkPermissionResult != NETMANAGER_SUCCESS) {
        return checkPermissionResult;
    }
    auto itFunc = memberFuncMap_.find(code);
    int32_t result = NETMANAGER_SUCCESS;
    if (itFunc != memberFuncMap_.end()) {
        auto requestFunc = itFunc->second;
        if (requestFunc != nullptr) {
            handler_->PostSyncTask(
                [this, &data, &reply, &requestFunc, &result]() { result = (this->*requestFunc)(data, reply); },
                AppExecFwk::EventQueue::Priority::HIGH);
            return result;
        }
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

bool NetPolicyServiceStub::CheckPermission(const std::string &permission, uint32_t funcCode)
{
    if (NetManagerPermission::CheckPermission(permission)) {
        return true;
    }
    NETMGR_LOG_E("Permission denied funcCode: %{public}d permission: %{public}s", funcCode, permission.c_str());
    return false;
}

int32_t NetPolicyServiceStub::CheckPolicyPermission(uint32_t code)
{
    bool result = NetManagerPermission::IsSystemCaller();
    if (!result) {
        return NETMANAGER_ERR_NOT_SYSTEM_CALL;
    }
    if (g_codeNPS.find(code) != g_codeNPS.end()) {
        result = CheckPermission(g_codeNPS[code], code);
        if (!result) {
            return NETMANAGER_ERR_PERMISSION_DENIED;
        }
        return NETMANAGER_SUCCESS;
    }
    NETMGR_LOG_E("Error funcCode, need check");
    return NETMANAGER_ERR_PERMISSION_DENIED;
}

int32_t NetPolicyServiceStub::OnSetPolicyByUid(MessageParcel &data, MessageParcel &reply)
{
    uint32_t uid;
    if (!data.ReadUint32(uid)) {
        NETMGR_LOG_E("Read Uint32 data failed.");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    uint32_t netPolicy;
    if (!data.ReadUint32(netPolicy)) {
        NETMGR_LOG_E("Read Uint32 data failed.");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = SetPolicyByUid(uid, netPolicy);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed.");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnGetPolicyByUid(MessageParcel &data, MessageParcel &reply)
{
    uint32_t uid = 0;
    if (!data.ReadUint32(uid)) {
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    uint32_t policy = 0;
    int32_t result = GetPolicyByUid(uid, policy);
    if (!reply.WriteInt32(policy)) {
        NETMGR_LOG_E("Write int32 reply failed.");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed.");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnGetUidsByPolicy(MessageParcel &data, MessageParcel &reply)
{
    uint32_t policy;
    if (!data.ReadUint32(policy)) {
        NETMGR_LOG_E("Read uint32 data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::vector<uint32_t> uids;
    int32_t result = GetUidsByPolicy(policy, uids);
    if (!reply.WriteUInt32Vector(uids)) {
        NETMGR_LOG_E("Write uint32 vector reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnIsUidNetAllowedMetered(MessageParcel &data, MessageParcel &reply)
{
    uint32_t uid = 0;
    bool metered = false;
    if (!data.ReadUint32(uid)) {
        NETMGR_LOG_E("Read uint32 data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    if (!data.ReadBool(metered)) {
        NETMGR_LOG_E("Read Bool data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    bool isAllowed = false;
    int32_t result = IsUidNetAllowed(uid, metered, isAllowed);
    if (!reply.WriteBool(isAllowed)) {
        NETMGR_LOG_E("Write Bool reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnIsUidNetAllowedIfaceName(MessageParcel &data, MessageParcel &reply)
{
    uint32_t uid = 0;
    std::string ifaceName;
    if (!data.ReadUint32(uid)) {
        NETMGR_LOG_E("Read uint32 data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    if (!data.ReadString(ifaceName)) {
        NETMGR_LOG_E("Read String data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    bool isAllowed = false;
    int32_t result = IsUidNetAllowed(uid, ifaceName, isAllowed);

    if (!reply.WriteBool(isAllowed)) {
        NETMGR_LOG_E("Write Bool reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnRegisterNetPolicyCallback(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("Callback ptr is nullptr.");
        reply.WriteInt32(NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL);
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }

    sptr<INetPolicyCallback> callback = iface_cast<INetPolicyCallback>(remote);
    int32_t result = RegisterNetPolicyCallback(callback);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnUnregisterNetPolicyCallback(MessageParcel &data, MessageParcel &reply)
{
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETMGR_LOG_E("callback ptr is nullptr.");
        reply.WriteInt32(NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL);
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    sptr<INetPolicyCallback> callback = iface_cast<INetPolicyCallback>(remote);
    int32_t result = UnregisterNetPolicyCallback(callback);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnSetNetQuotaPolicies(MessageParcel &data, MessageParcel &reply)
{
    std::vector<NetQuotaPolicy> quotaPolicies;
    if (!NetQuotaPolicy::Unmarshalling(data, quotaPolicies)) {
        NETMGR_LOG_E("Unmarshalling failed.");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = SetNetQuotaPolicies(quotaPolicies);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnGetNetQuotaPolicies(MessageParcel &data, MessageParcel &reply)
{
    std::vector<NetQuotaPolicy> quotaPolicies;

    int32_t result = GetNetQuotaPolicies(quotaPolicies);

    if (!NetQuotaPolicy::Marshalling(reply, quotaPolicies)) {
        NETMGR_LOG_E("Marshalling failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnResetPolicies(MessageParcel &data, MessageParcel &reply)
{
    std::string subscriberId;
    if (!data.ReadString(subscriberId)) {
        NETMGR_LOG_E("Read String data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = ResetPolicies(subscriberId);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnSetBackgroundPolicy(MessageParcel &data, MessageParcel &reply)
{
    bool isBackgroundPolicyAllow = false;
    if (!data.ReadBool(isBackgroundPolicyAllow)) {
        NETMGR_LOG_E("Read Bool data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = SetBackgroundPolicy(isBackgroundPolicyAllow);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnGetBackgroundPolicy(MessageParcel &data, MessageParcel &reply)
{
    bool backgroundPolicy = false;
    int32_t result = GetBackgroundPolicy(backgroundPolicy);

    if (!reply.WriteBool(backgroundPolicy)) {
        NETMGR_LOG_E("Write Bool reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnGetBackgroundPolicyByUid(MessageParcel &data, MessageParcel &reply)
{
    uint32_t uid = 0;
    if (!data.ReadUint32(uid)) {
        NETMGR_LOG_E("Read uint32 data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    uint32_t backgroundPolicyOfUid = 0;
    int32_t result = GetBackgroundPolicyByUid(uid, backgroundPolicyOfUid);

    if (!reply.WriteUint32(backgroundPolicyOfUid)) {
        NETMGR_LOG_E("Write uint32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnSnoozePolicy(MessageParcel &data, MessageParcel &reply)
{
    int32_t netType = 0;
    if (!data.ReadInt32(netType)) {
        NETMGR_LOG_E("Read int32 data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    std::string iccid;
    if (!data.ReadString(iccid)) {
        NETMGR_LOG_E("Read String data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    uint32_t remindType = 0;
    if (!data.ReadUint32(remindType)) {
        NETMGR_LOG_E("Read uint32 data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = UpdateRemindPolicy(netType, iccid, remindType);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnSetDeviceIdleAllowedList(MessageParcel &data, MessageParcel &reply)
{
    uint32_t uid;
    if (!data.ReadUint32(uid)) {
        NETMGR_LOG_E("Read uint32 data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    bool isAllowed = false;
    if (!data.ReadBool(isAllowed)) {
        NETMGR_LOG_E("Read Bool data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = SetDeviceIdleAllowedList(uid, isAllowed);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnGetDeviceIdleAllowedList(MessageParcel &data, MessageParcel &reply)
{
    std::vector<uint32_t> uids;
    int32_t result = GetDeviceIdleAllowedList(uids);

    if (!reply.WriteUInt32Vector(uids)) {
        NETMGR_LOG_E("Write uint32 vector reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetPolicyServiceStub::OnSetDeviceIdlePolicy(MessageParcel &data, MessageParcel &reply)
{
    bool isAllowed = false;
    if (!data.ReadBool(isAllowed)) {
        NETMGR_LOG_E("Read Bool data failed");
        return NETMANAGER_ERR_READ_DATA_FAIL;
    }

    int32_t result = SetDeviceIdlePolicy(isAllowed);
    if (!reply.WriteInt32(result)) {
        NETMGR_LOG_E("Write int32 reply failed");
        return NETMANAGER_ERR_WRITE_REPLY_FAIL;
    }

    return NETMANAGER_SUCCESS;
}
} // namespace NetManagerStandard
} // namespace OHOS
