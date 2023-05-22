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

#include "net_stats_client.h"

#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
NetStatsClient::NetStatsClient() : netStatsService_(nullptr), deathRecipient_(nullptr) {}

NetStatsClient::~NetStatsClient() = default;

int32_t NetStatsClient::RegisterNetStatsCallback(const sptr<INetStatsCallback> &callback)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->RegisterNetStatsCallback(callback);
}

int32_t NetStatsClient::UnregisterNetStatsCallback(const sptr<INetStatsCallback> &callback)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }

    return proxy->UnregisterNetStatsCallback(callback);
}

sptr<INetStatsService> NetStatsClient::GetProxy()
{
    std::lock_guard lock(mutex_);

    if (netStatsService_ != nullptr) {
        NETMGR_LOG_D("get proxy is ok");
        return netStatsService_;
    }

    NETMGR_LOG_D("execute GetSystemAbilityManager");
    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        NETMGR_LOG_E("NetPolicyManager::GetProxy(), get SystemAbilityManager failed");
        return nullptr;
    }

    sptr<IRemoteObject> remote = sam->CheckSystemAbility(COMM_NET_STATS_MANAGER_SYS_ABILITY_ID);
    if (remote == nullptr) {
        NETMGR_LOG_E("get Remote service failed");
        return nullptr;
    }

    deathRecipient_ = new (std::nothrow) NetStatsDeathRecipient(*this);
    if (deathRecipient_ == nullptr) {
        NETMGR_LOG_E("get deathRecipient_ failed");
        return nullptr;
    }
    if ((remote->IsProxyObject()) && (!remote->AddDeathRecipient(deathRecipient_))) {
        NETMGR_LOG_E("add death recipient failed");
        return nullptr;
    }

    netStatsService_ = iface_cast<INetStatsService>(remote);
    if (netStatsService_ == nullptr) {
        NETMGR_LOG_E("get Remote service proxy failed");
        return nullptr;
    }
    return netStatsService_;
}

void NetStatsClient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    NETMGR_LOG_D("on remote died");
    if (remote == nullptr) {
        NETMGR_LOG_E("remote object is nullptr");
        return;
    }

    std::lock_guard lock(mutex_);
    if (netStatsService_ == nullptr) {
        NETMGR_LOG_E("NetConnService_ is nullptr");
        return;
    }

    sptr<IRemoteObject> local = netStatsService_->AsObject();
    if (local != remote.promote()) {
        NETMGR_LOG_E("proxy and stub is not same remote object");
        return;
    }

    local->RemoveDeathRecipient(deathRecipient_);
    netStatsService_ = nullptr;
}

int32_t NetStatsClient::GetIfaceRxBytes(uint64_t &stats, const std::string &interfaceName)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetIfaceRxBytes(stats, interfaceName);
}

int32_t NetStatsClient::GetIfaceTxBytes(uint64_t &stats, const std::string &interfaceName)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetIfaceTxBytes(stats, interfaceName);
}

int32_t NetStatsClient::GetCellularRxBytes(uint64_t &stats)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetCellularRxBytes(stats);
}

int32_t NetStatsClient::GetCellularTxBytes(uint64_t &stats)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetCellularTxBytes(stats);
}

int32_t NetStatsClient::GetAllRxBytes(uint64_t &stats)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetAllRxBytes(stats);
}

int32_t NetStatsClient::GetAllTxBytes(uint64_t &stats)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetAllTxBytes(stats);
}

int32_t NetStatsClient::GetUidRxBytes(uint64_t &stats, uint32_t uid)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetUidRxBytes(stats, uid);
}

int32_t NetStatsClient::GetUidTxBytes(uint64_t &stats, uint32_t uid)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetUidTxBytes(stats, uid);
}

int32_t NetStatsClient::GetIfaceStatsDetail(const std::string &iface, uint64_t start, uint64_t end,
                                            NetStatsInfo &statsInfo)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetIfaceStatsDetail(iface, start, end, statsInfo);
}

int32_t NetStatsClient::GetUidStatsDetail(const std::string &iface, uint32_t uid, uint64_t start, uint64_t end,
                                          NetStatsInfo &statsInfo)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetUidStatsDetail(iface, uid, start, end, statsInfo);
}

int32_t NetStatsClient::UpdateIfacesStats(const std::string &iface, uint64_t start, uint64_t end,
                                          const NetStatsInfo &stats)
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->UpdateIfacesStats(iface, start, end, stats);
}

int32_t NetStatsClient::UpdateStatsData()
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->UpdateStatsData();
}

int32_t NetStatsClient::ResetFactory()
{
    sptr<INetStatsService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->ResetFactory();
}
} // namespace NetManagerStandard
} // namespace OHOS
