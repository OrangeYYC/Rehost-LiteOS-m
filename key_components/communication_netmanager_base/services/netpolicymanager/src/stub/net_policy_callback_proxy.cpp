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

#include "net_policy_callback_proxy.h"

#include "net_mgr_log_wrapper.h"
#include "net_quota_policy.h"

namespace OHOS {
namespace NetManagerStandard {
NetPolicyCallbackProxy::NetPolicyCallbackProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<INetPolicyCallback>(impl)
{
}

NetPolicyCallbackProxy::~NetPolicyCallbackProxy() = default;

int32_t NetPolicyCallbackProxy::NetUidPolicyChange(uint32_t uid, uint32_t policy)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteUint32(uid)) {
        NETMGR_LOG_E("Write uint32 data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteUint32(policy)) {
        NETMGR_LOG_E("Write uint32 data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(NOTIFY_NET_UID_POLICY_CHANGE, data, reply, option);
    if (ret != 0) {
        NETMGR_LOG_E("Proxy SendRequest failed, ret code:[%{public}d]", ret);
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return ret;
}

int32_t NetPolicyCallbackProxy::NetUidRuleChange(uint32_t uid, uint32_t rule)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteUint32(uid)) {
        NETMGR_LOG_E("Write uint32 data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteUint32(rule)) {
        NETMGR_LOG_E("Write uint32 data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(NOTIFY_NET_UID_RULE_CHANGE, data, reply, option);
    if (ret != 0) {
        NETMGR_LOG_E("Proxy SendRequest failed, ret code:[%{public}d]", ret);
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return ret;
}

int32_t NetPolicyCallbackProxy::NetBackgroundPolicyChange(bool isBackgroundPolicyAllow)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteBool(isBackgroundPolicyAllow)) {
        NETMGR_LOG_E("Write Bool data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(NOTIFY_BACKGROUND_POLICY_CHANGE, data, reply, option);
    if (ret != 0) {
        NETMGR_LOG_E("Proxy SendRequest failed, ret code:[%{public}d]", ret);
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return ret;
}

int32_t NetPolicyCallbackProxy::NetQuotaPolicyChange(const std::vector<NetQuotaPolicy> &quotaPolicies)
{
    if (quotaPolicies.empty()) {
        NETMGR_LOG_E("NetQuotaPolicyChange proxy quotaPolicies empty");
        return NetPolicyResultCode::POLICY_ERR_QUOTA_POLICY_NOT_EXIST;
    }

    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!NetQuotaPolicy::Marshalling(data, quotaPolicies)) {
        NETMGR_LOG_E("Marshalling failed.");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(NOTIFY_NET_QUOTA_POLICY_CHANGE, data, reply, option);
    if (ret != 0) {
        NETMGR_LOG_E("Proxy SendRequest failed, ret code:[%{public}d]", ret);
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return ret;
}

int32_t NetPolicyCallbackProxy::NetStrategySwitch(const std::string &iccid, bool enable)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }

    if (!data.WriteString(iccid)) {
        NETMGR_LOG_E("WriteString String data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteBool(enable)) {
        NETMGR_LOG_E("WriteBool Bool data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(NET_POLICY_STRATEGYSWITCH_CHANGE, data, reply, option);
    if (ret != 0) {
        NETMGR_LOG_E("Proxy SendRequest failed, ret code:[%{public}d]", ret);
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return ret;
}

int32_t NetPolicyCallbackProxy::NetMeteredIfacesChange(std::vector<std::string> &ifaces)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    uint32_t size = static_cast<uint32_t>(ifaces.size());
    if (!data.WriteUint32(size)) {
        NETMGR_LOG_E("Write uint32 data failed");
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    for (uint32_t i = 0; i < ifaces.size(); ++i) {
        if (!data.WriteString(ifaces[i])) {
            NETMGR_LOG_E("Write String data failed");
            return NETMANAGER_ERR_WRITE_DATA_FAIL;
        }
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(NOTIFY_NET_METERED_IFACES_CHANGE, data, reply, option);
    if (ret != 0) {
        NETMGR_LOG_E("Proxy SendRequest failed, ret code:[%{public}d]", ret);
        return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
    }
    return ret;
}

bool NetPolicyCallbackProxy::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(NetPolicyCallbackProxy::GetDescriptor())) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return false;
    }
    return true;
}
} // namespace NetManagerStandard
} // namespace OHOS
