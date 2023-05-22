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

#include "networkshare_client.h"

#include "iservice_registry.h"
#include "system_ability_definition.h"
#include "net_manager_constants.h"
#include "netmgr_ext_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
NetworkShareClient::NetworkShareClient() : networkShareService_(nullptr), deathRecipient_(nullptr) {}

NetworkShareClient::~NetworkShareClient() {}

int32_t NetworkShareClient::StartSharing(const SharingIfaceType &type)
{
    sptr<INetworkShareService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_EXT_LOG_E("StartSharing proxy is nullptr");
        return NETMANAGER_EXT_ERR_GET_PROXY_FAIL;
    }
    return proxy->StartNetworkSharing(type);
}

int32_t NetworkShareClient::StopSharing(const SharingIfaceType &type)
{
    sptr<INetworkShareService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_EXT_LOG_E("StopSharing proxy is nullptr");
        return NETMANAGER_EXT_ERR_GET_PROXY_FAIL;
    }
    return proxy->StopNetworkSharing(type);
}

int32_t NetworkShareClient::IsSharingSupported(int32_t &supported)
{
    sptr<INetworkShareService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_EXT_LOG_E("IsSharingSupported proxy is nullptr");
        return NETMANAGER_EXT_ERR_GET_PROXY_FAIL;
    }
    return proxy->IsNetworkSharingSupported(supported);
}

int32_t NetworkShareClient::IsSharing(int32_t &sharingStatus)
{
    sptr<INetworkShareService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_EXT_LOG_E("IsSharing proxy is nullptr");
        return NETMANAGER_EXT_ERR_GET_PROXY_FAIL;
    }
    return proxy->IsSharing(sharingStatus);
}

int32_t NetworkShareClient::RegisterSharingEvent(sptr<ISharingEventCallback> callback)
{
    sptr<INetworkShareService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_EXT_LOG_E("RegisterSharingEvent proxy is nullptr");
        return NETMANAGER_EXT_ERR_GET_PROXY_FAIL;
    }
    return proxy->RegisterSharingEvent(callback);
}

int32_t NetworkShareClient::UnregisterSharingEvent(sptr<ISharingEventCallback> callback)
{
    sptr<INetworkShareService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_EXT_LOG_E("UnregisterSharingEvent proxy is nullptr");
        return NETMANAGER_EXT_ERR_GET_PROXY_FAIL;
    }
    return proxy->UnregisterSharingEvent(callback);
}

int32_t NetworkShareClient::GetSharableRegexs(const SharingIfaceType &type, std::vector<std::string> &ifaceRegexs)
{
    sptr<INetworkShareService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_EXT_LOG_E("GetSharableRegexs proxy is nullptr");
        return NETMANAGER_EXT_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetSharableRegexs(type, ifaceRegexs);
}

int32_t NetworkShareClient::GetSharingState(const SharingIfaceType &type, SharingIfaceState &state)
{
    sptr<INetworkShareService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_EXT_LOG_E("GetSharingState proxy is nullptr");
        return NETMANAGER_EXT_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetSharingState(type, state);
}

int32_t NetworkShareClient::GetSharingIfaces(const SharingIfaceState &state, std::vector<std::string> &ifaces)
{
    sptr<INetworkShareService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_EXT_LOG_E("GetSharingIfaces proxy is nullptr");
        return NETMANAGER_EXT_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetNetSharingIfaces(state, ifaces);
}

int32_t NetworkShareClient::GetStatsRxBytes(int32_t &bytes)
{
    sptr<INetworkShareService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_EXT_LOG_E("GetStatsRxBytes proxy is nullptr");
        return NETMANAGER_EXT_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetStatsRxBytes(bytes);
}

int32_t NetworkShareClient::GetStatsTxBytes(int32_t &bytes)
{
    sptr<INetworkShareService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_EXT_LOG_E("GetStatsTxBytes proxy is nullptr");
        return NETMANAGER_EXT_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetStatsTxBytes(bytes);
}

int32_t NetworkShareClient::GetStatsTotalBytes(int32_t &bytes)
{
    sptr<INetworkShareService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_EXT_LOG_E("GetStatsTotalBytes proxy is nullptr");
        return NETMANAGER_EXT_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetStatsTotalBytes(bytes);
}

sptr<INetworkShareService> NetworkShareClient::GetProxy()
{
    std::lock_guard lock(mutex_);
    if (networkShareService_ != nullptr) {
        return networkShareService_;
    }
    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        NETMGR_EXT_LOG_E("get SystemAbilityManager failed");
        return nullptr;
    }
    sptr<IRemoteObject> remote = sam->CheckSystemAbility(COMM_NET_TETHERING_MANAGER_SYS_ABILITY_ID);
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("get Remote service failed");
        return nullptr;
    }
    deathRecipient_ = new NetshareDeathRecipient(*this);
    if ((remote->IsProxyObject()) && (!remote->AddDeathRecipient(deathRecipient_))) {
        NETMGR_EXT_LOG_E("add death recipient failed");
        return nullptr;
    }
    networkShareService_ = iface_cast<INetworkShareService>(remote);
    if (networkShareService_ == nullptr) {
        NETMGR_EXT_LOG_E("get Remote service proxy failed");
        return nullptr;
    }
    return networkShareService_;
}

void NetworkShareClient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("remote object is nullptr");
        return;
    }
    std::lock_guard lock(mutex_);
    if (networkShareService_ == nullptr) {
        NETMGR_EXT_LOG_E("networkShareService_ is nullptr");
        return;
    }
    sptr<IRemoteObject> local = networkShareService_->AsObject();
    if (local != remote.promote()) {
        NETMGR_EXT_LOG_E("proxy and stub is not same remote object");
        return;
    }
    local->RemoveDeathRecipient(deathRecipient_);
    networkShareService_ = nullptr;
}
} // namespace NetManagerStandard
} // namespace OHOS
