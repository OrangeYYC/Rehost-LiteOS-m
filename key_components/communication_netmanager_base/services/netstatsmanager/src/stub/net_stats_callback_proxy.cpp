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

#include "net_stats_callback_proxy.h"

#include "net_mgr_log_wrapper.h"
#include "net_stats_constants.h"

namespace OHOS {
namespace NetManagerStandard {
NetStatsCallbackProxy::NetStatsCallbackProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<INetStatsCallback>(impl)
{
}

NetStatsCallbackProxy::~NetStatsCallbackProxy() = default;

int32_t NetStatsCallbackProxy::NetIfaceStatsChanged(const std::string &iface)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    NETMGR_LOG_D("proxy iface[%{public}s]", iface.c_str());
    if (!data.WriteString(iface)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(NET_STATS_IFACE_CHANGED, data, reply, option);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Proxy SendRequest failed, ret code:[%{public}d]", ret);
    }
    return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
}

int32_t NetStatsCallbackProxy::NetUidStatsChanged(const std::string &iface, uint32_t uid)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return NETMANAGER_ERR_WRITE_DESCRIPTOR_TOKEN_FAIL;
    }
    NETMGR_LOG_D("proxy iface[%{public}s], uid[%{public}d]", iface.c_str(), uid);
    if (!data.WriteString(iface)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    if (!data.WriteUint32(uid)) {
        return NETMANAGER_ERR_WRITE_DATA_FAIL;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        NETMGR_LOG_E("Remote is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t ret = remote->SendRequest(NET_STATS_UID_CHANGED, data, reply, option);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Proxy SendRequest failed, ret code:[%{public}d]", ret);
    }
    return NETMANAGER_ERR_IPC_CONNECT_STUB_FAIL;
}

bool NetStatsCallbackProxy::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(NetStatsCallbackProxy::GetDescriptor())) {
        NETMGR_LOG_E("WriteInterfaceToken failed");
        return false;
    }
    return true;
}
} // namespace NetManagerStandard
} // namespace OHOS
