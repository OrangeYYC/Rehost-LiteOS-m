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

#include "ethernet_client.h"

#include "i_ethernet_service.h"
#include "if_system_ability_manager.h"
#include "interface_configuration.h"
#include "ipc_types.h"
#include "iremote_broker.h"
#include "iremote_object.h"
#include "iservice_registry.h"
#include "netmgr_ext_log_wrapper.h"
#include "refbase.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace NetManagerStandard {
EthernetClient::EthernetClient() : ethernetService_(nullptr), deathRecipient_(nullptr) {}

EthernetClient::~EthernetClient() = default;

int32_t EthernetClient::SetIfaceConfig(const std::string &iface, sptr<InterfaceConfiguration> &ic)
{
    sptr<IEthernetService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_EXT_LOG_E("proxy is nullptr");
        return IPC_PROXY_ERR;
    }
    return proxy->SetIfaceConfig(iface, ic);
}

int32_t EthernetClient::GetIfaceConfig(const std::string &iface, sptr<InterfaceConfiguration> &ifaceConfig)
{
    sptr<IEthernetService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_EXT_LOG_E("proxy is nullptr");
        return IPC_PROXY_ERR;
    }
    return proxy->GetIfaceConfig(iface, ifaceConfig);
}

int32_t EthernetClient::IsIfaceActive(const std::string &iface, int32_t &activeStatus)
{
    sptr<IEthernetService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_EXT_LOG_E("proxy is nullptr");
        return IPC_PROXY_ERR;
    }
    return proxy->IsIfaceActive(iface, activeStatus);
}

int32_t EthernetClient::GetAllActiveIfaces(std::vector<std::string> &activeIfaces)
{
    sptr<IEthernetService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_EXT_LOG_E("proxy is nullptr");
        return IPC_PROXY_ERR;
    }
    return proxy->GetAllActiveIfaces(activeIfaces);
}

int32_t EthernetClient::ResetFactory()
{
    sptr<IEthernetService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_EXT_LOG_E("proxy is nullptr");
        return IPC_PROXY_ERR;
    }
    return proxy->ResetFactory();
}

sptr<IEthernetService> EthernetClient::GetProxy()
{
    std::lock_guard lock(mutex_);
    if (ethernetService_) {
        NETMGR_EXT_LOG_D("get proxy is ok");
        return ethernetService_;
    }
    sptr<ISystemAbilityManager> sam = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sam == nullptr) {
        NETMGR_EXT_LOG_E("GetProxy, get SystemAbilityManager failed");
        return nullptr;
    }
    sptr<IRemoteObject> remote = sam->CheckSystemAbility(COMM_ETHERNET_MANAGER_SYS_ABILITY_ID);
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("get Remote service failed");
        return nullptr;
    }
    deathRecipient_ = new (std::nothrow) EthernetDeathRecipient(*this);
    if (deathRecipient_ == nullptr) {
        NETMGR_EXT_LOG_E("Recipient new failed!");
    }
    if ((remote->IsProxyObject()) && (!remote->AddDeathRecipient(deathRecipient_))) {
        NETMGR_EXT_LOG_E("add death recipient failed");
        return nullptr;
    }
    ethernetService_ = iface_cast<IEthernetService>(remote);
    if (ethernetService_ == nullptr) {
        NETMGR_EXT_LOG_E("get Remote service proxy failed");
        return nullptr;
    }
    return ethernetService_;
}

void EthernetClient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    if (remote == nullptr) {
        NETMGR_EXT_LOG_E("remote object is nullptr");
        return;
    }
    std::lock_guard lock(mutex_);
    if (ethernetService_ == nullptr) {
        NETMGR_EXT_LOG_E("ethernetService_ is nullptr");
        return;
    }
    sptr<IRemoteObject> local = ethernetService_->AsObject();
    if (local == nullptr) {
        NETMGR_EXT_LOG_E("local is nullptr");
        return;
    }
    if (local != remote.promote()) {
        NETMGR_EXT_LOG_E("proxy and stub is not same remote object");
        return;
    }
    local->RemoveDeathRecipient(deathRecipient_);
    ethernetService_ = nullptr;
}

int32_t EthernetClient::SetInterfaceUp(const std::string &iface)
{
    sptr<IEthernetService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_EXT_LOG_E("proxy is nullptr");
        return IPC_PROXY_ERR;
    }
    return proxy->SetInterfaceUp(iface);
}

int32_t EthernetClient::SetInterfaceDown(const std::string &iface)
{
    sptr<IEthernetService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_EXT_LOG_E("proxy is nullptr");
        return IPC_PROXY_ERR;
    }
    return proxy->SetInterfaceDown(iface);
}

int32_t EthernetClient::GetInterfaceConfig(const std::string &iface, OHOS::nmd::InterfaceConfigurationParcel &cfg)
{
    sptr<IEthernetService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_EXT_LOG_E("proxy is nullptr");
        return IPC_PROXY_ERR;
    }
    return proxy->GetInterfaceConfig(iface, cfg);
}

int32_t EthernetClient::SetInterfaceConfig(const std::string &iface, OHOS::nmd::InterfaceConfigurationParcel &cfg)
{
    sptr<IEthernetService> proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_EXT_LOG_E("proxy is nullptr");
        return IPC_PROXY_ERR;
    }
    return proxy->SetInterfaceConfig(iface, cfg);
}
} // namespace NetManagerStandard
} // namespace OHOS
