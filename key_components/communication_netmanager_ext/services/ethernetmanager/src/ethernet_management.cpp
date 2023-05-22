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

#include "ethernet_management.h"

#include <regex>
#include <thread>
#include <unistd.h>

#include "net_manager_constants.h"
#include "netmgr_ext_log_wrapper.h"
#include "netsys_controller.h"
#include "securec.h"

namespace OHOS {
namespace NetManagerStandard {
const std::string IFACE_MATCH = "eth\\d";
constexpr int SLEEP_TIME_S = 2;
int32_t EthernetManagement::EhternetDhcpNotifyCallback::OnDhcpSuccess(EthernetDhcpCallback::DhcpResult &dhcpResult)
{
    ethernetManagement_.UpdateDevInterfaceLinkInfo(dhcpResult);
    return 0;
}

int32_t EthernetManagement::DevInterfaceStateCallback::OnInterfaceAddressUpdated(const std::string &,
                                                                                 const std::string &, int, int)
{
    return 0;
}

int32_t EthernetManagement::DevInterfaceStateCallback::OnInterfaceAddressRemoved(const std::string &,
                                                                                 const std::string &, int, int)
{
    return 0;
}

int32_t EthernetManagement::DevInterfaceStateCallback::OnInterfaceAdded(const std::string &iface)
{
    std::regex re(IFACE_MATCH);
    if (std::regex_search(iface, re)) {
        ethernetManagement_.DevInterfaceAdd(iface);
        if (NetsysController::GetInstance().SetInterfaceUp(iface) != ERR_NONE) {
            NETMGR_EXT_LOG_E("Iface[%{public}s] added set up fail!", iface.c_str());
        }
    }
    return 0;
}

int32_t EthernetManagement::DevInterfaceStateCallback::OnInterfaceRemoved(const std::string &iface)
{
    std::regex re(IFACE_MATCH);
    if (std::regex_search(iface, re)) {
        ethernetManagement_.DevInterfaceRemove(iface);
        if (NetsysController::GetInstance().SetInterfaceDown(iface) != ERR_NONE) {
            NETMGR_EXT_LOG_E("Iface[%{public}s] added set down fail!", iface.c_str());
        }
    }
    return 0;
}

int32_t EthernetManagement::DevInterfaceStateCallback::OnInterfaceChanged(const std::string &, bool)
{
    return 0;
}

int32_t EthernetManagement::DevInterfaceStateCallback::OnInterfaceLinkStateChanged(const std::string &ifName, bool up)
{
    NETMGR_EXT_LOG_I("DevInterfaceStateCallback::OnInterfaceLinkStateChanged iface[%{public}s] up[%{public}d]",
                     ifName.c_str(), up);
    std::regex re(IFACE_MATCH);
    if (std::regex_search(ifName, re)) {
        ethernetManagement_.UpdateInterfaceState(ifName, up);
    }
    return 0;
}

int32_t EthernetManagement::DevInterfaceStateCallback::OnRouteChanged(bool, const std::string &, const std::string &,
                                                                      const std::string &)
{
    return 0;
}

int32_t EthernetManagement::DevInterfaceStateCallback::OnDhcpSuccess(NetsysControllerCallback::DhcpResult &dhcpResult)
{
    return 0;
}

int32_t EthernetManagement::DevInterfaceStateCallback::OnBandwidthReachedLimit(const std::string &limitName,
                                                                               const std::string &iface)
{
    return 0;
}

EthernetManagement::EthernetManagement()
{
    ethDhcpController_ = std::make_unique<EthernetDhcpController>();
    ethDhcpNotifyCallback_ = new (std::nothrow) EhternetDhcpNotifyCallback(*this);
    if (ethDhcpNotifyCallback_ != nullptr) {
        ethDhcpController_->RegisterDhcpCallback(ethDhcpNotifyCallback_);
    }

    ethDevInterfaceStateCallback_ = new (std::nothrow) DevInterfaceStateCallback(*this);
    if (ethDevInterfaceStateCallback_ != nullptr) {
        NetsysController::GetInstance().RegisterCallback(ethDevInterfaceStateCallback_);
    }

    ethConfiguration_ = std::make_unique<EthernetConfiguration>();
    ethConfiguration_->ReadSysteamConfiguration(devCaps_, devCfgs_);
}

EthernetManagement::~EthernetManagement() = default;

void EthernetManagement::UpdateInterfaceState(const std::string &dev, bool up)
{
    NETMGR_EXT_LOG_D("EthernetManagement UpdateInterfaceState dev[%{public}s] up[%{public}d]", dev.c_str(), up);
    std::unique_lock<std::mutex> lock(mutex_);
    auto fit = devs_.find(dev);
    if (fit == devs_.end()) {
        return;
    }
    sptr<DevInterfaceState> devState = fit->second;
    if (devState == nullptr) {
        NETMGR_EXT_LOG_E("devState is nullptr");
        return;
    }
    devState->SetLinkUp(up);
    IPSetMode mode = devState->GetIPSetMode();
    bool dhcpReqState = devState->GetDhcpReqState();
    NETMGR_EXT_LOG_D("EthernetManagement UpdateInterfaceState mode[%{public}d] dhcpReqState[%{public}d]",
                     static_cast<int32_t>(mode), dhcpReqState);
    if (up) {
        devState->RemoteUpdateNetSupplierInfo();
        if (mode == DHCP && !dhcpReqState) {
            StartDhcpClient(dev, devState);
        } else {
            devState->RemoteUpdateNetLinkInfo();
        }
    } else {
        if (mode == DHCP && dhcpReqState) {
            StopDhcpClient(dev, devState);
        }
        devState->RemoteUpdateNetSupplierInfo();
    }
}

int32_t EthernetManagement::UpdateDevInterfaceCfg(const std::string &iface, sptr<InterfaceConfiguration> cfg)
{
    if (cfg == nullptr) {
        NETMGR_EXT_LOG_E("cfg is nullptr");
        return NETMANAGER_EXT_ERR_LOCAL_PTR_NULL;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    auto fit = devs_.find(iface);
    if (fit == devs_.end() || fit->second == nullptr) {
        NETMGR_EXT_LOG_E("The iface[%{public}s] device or device information does not exist", iface.c_str());
        return ETHERNET_ERR_DEVICE_INFORMATION_NOT_EXIST;
    }
    if (!fit->second->GetLinkUp()) {
        NETMGR_EXT_LOG_E("The iface[%{public}s] device is unlink", iface.c_str());
        return ETHERNET_ERR_DEVICE_NOT_LINK;
    }
    if (!ethConfiguration_->WriteUserConfiguration(iface, cfg)) {
        NETMGR_EXT_LOG_E("EthernetManagement write user configurations error!");
        return ETHERNET_ERR_USER_CONIFGURATION_WRITE_FAIL;
    }
    if (fit->second->GetIfcfg()->mode_ != cfg->mode_) {
        if (cfg->mode_ == DHCP) {
            StartDhcpClient(iface, fit->second);
        } else {
            StopDhcpClient(iface, fit->second);
        }
    }
    fit->second->SetIfcfg(cfg);
    return NETMANAGER_EXT_SUCCESS;
}

int32_t EthernetManagement::UpdateDevInterfaceLinkInfo(EthernetDhcpCallback::DhcpResult &dhcpResult)
{
    NETMGR_EXT_LOG_D("EthernetManagement::UpdateDevInterfaceLinkInfo");
    auto fit = devs_.find(dhcpResult.iface);
    if (fit == devs_.end() || fit->second == nullptr) {
        NETMGR_EXT_LOG_E("The iface[%{public}s] device or device information does not exist", dhcpResult.iface.c_str());
        return ETHERNET_ERR_DEVICE_INFORMATION_NOT_EXIST;
    }
    if (!fit->second->GetLinkUp()) {
        NETMGR_EXT_LOG_E("The iface[%{public}s] The device is not turned on", dhcpResult.iface.c_str());
        return ETHERNET_ERR_DEVICE_NOT_LINK;
    }
    sptr<StaticConfiguration> config = new (std::nothrow) StaticConfiguration();
    if (config == nullptr) {
        NETMGR_EXT_LOG_E("config is nullptr");
        return ETHERNET_ERR_EMPTY_CONFIGURATION;
    }

    if (!ethConfiguration_->ConvertToConfiguration(dhcpResult, config)) {
        NETMGR_EXT_LOG_E("EthernetManagement dhcp convert to configurations error!");
        return ETHERNET_ERR_CONVERT_CONFIGURATINO_FAIL;
    }
    fit->second->UpdateLinkInfo(config->ipAddr_, config->netMask_, config->gateway_, config->route_,
                                config->dnsServers_.front(), config->dnsServers_.back());
    fit->second->RemoteUpdateNetLinkInfo();
    return NETMANAGER_EXT_SUCCESS;
}

int32_t EthernetManagement::GetDevInterfaceCfg(const std::string &iface, sptr<InterfaceConfiguration> &ifaceConfig)
{
    std::unique_lock<std::mutex> lock(mutex_);
    auto fit = devs_.find(iface);
    if (fit == devs_.end() || fit->second == nullptr) {
        NETMGR_EXT_LOG_E("The iface[%{public}s] device does not exist", iface.c_str());
        return ETHERNET_ERR_DEVICE_INFORMATION_NOT_EXIST;
    }
    if (!fit->second->GetLinkUp()) {
        ifaceConfig = fit->second->GetIfcfg();
        return NETMANAGER_EXT_SUCCESS;
    }
    auto temp = ethConfiguration_->MakeInterfaceConfiguration(fit->second->GetIfcfg(), fit->second->GetLinkInfo());
    *ifaceConfig = *temp;
    return NETMANAGER_EXT_SUCCESS;
}

int32_t EthernetManagement::IsIfaceActive(const std::string &iface, int32_t &activeStatus)
{
    std::unique_lock<std::mutex> lock(mutex_);
    auto fit = devs_.find(iface);
    if (fit == devs_.end() || fit->second == nullptr) {
        NETMGR_EXT_LOG_E("The iface[%{public}s] device does not exist", iface.c_str());
        return ETHERNET_ERR_DEVICE_INFORMATION_NOT_EXIST;
    }
    activeStatus = static_cast<int32_t>(fit->second->GetLinkUp());
    return NETMANAGER_EXT_SUCCESS;
}

int32_t EthernetManagement::GetAllActiveIfaces(std::vector<std::string> &activeIfaces)
{
    std::unique_lock<std::mutex> lock(mutex_);
    for (auto it = devs_.begin(); it != devs_.end(); ++it) {
        if (it->second->GetLinkUp()) {
            activeIfaces.push_back(it->first);
        }
    }
    return NETMANAGER_EXT_SUCCESS;
}

int32_t EthernetManagement::ResetFactory()
{
    if (!ethConfiguration_->ClearAllUserConfiguration()) {
        NETMGR_EXT_LOG_E("Failed to ResetFactory!");
        return ETHERNET_ERR_USER_CONIFGURATION_CLEAR_FAIL;
    }
    NETMGR_EXT_LOG_I("Success to ResetFactory!");
    return NETMANAGER_EXT_SUCCESS;
}

void EthernetManagement::Init()
{
    std::regex re(IFACE_MATCH);
    std::vector<std::string> ifaces = NetsysController::GetInstance().InterfaceGetList();
    if (ifaces.empty()) {
        NETMGR_EXT_LOG_E("EthernetManagement link list is empty");
        return;
    }
    NETMGR_EXT_LOG_D("EthernetManagement devs size[%{public}zd]", ifaces.size());
    if (!ethConfiguration_->ReadUserConfiguration(devCfgs_)) {
        NETMGR_EXT_LOG_E("EthernetManagement read user configurations error! ");
        return;
    }
    for (const auto &devName : ifaces) {
        NETMGR_EXT_LOG_D("EthernetManagement devName[%{public}s]", devName.c_str());
        if (!std::regex_search(devName, re)) {
            continue;
        }
        DevInterfaceAdd(devName);
    }
    std::thread t(&EthernetManagement::StartSetDevUpThd, this);
    t.detach();
    NETMGR_EXT_LOG_D("EthernetManagement devs_ size[%{public}zd", devs_.size());
}

void EthernetManagement::StartSetDevUpThd()
{
    NETMGR_EXT_LOG_D("EthernetManagement StartSetDevUpThd in.");
    for (auto &dev : devs_) {
        std::string devName = dev.first;
        while (true) {
            if (NetsysController::GetInstance().SetInterfaceUp(devName) != ERR_NONE) {
                sleep(SLEEP_TIME_S);
                continue;
            }
            break;
        }
    }
}

void EthernetManagement::StartDhcpClient(const std::string &dev, sptr<DevInterfaceState> &devState)
{
    NETMGR_EXT_LOG_D("EthernetManagement StartDhcpClient[%{public}s]", dev.c_str());
    ethDhcpController_->StartDhcpClient(dev, false);
    devState->SetDhcpReqState(true);
}

void EthernetManagement::StopDhcpClient(const std::string &dev, sptr<DevInterfaceState> &devState)
{
    NETMGR_EXT_LOG_D("EthernetManagement StopDhcpClient[%{public}s]", dev.c_str());
    ethDhcpController_->StopDhcpClient(dev, false);
    devState->SetDhcpReqState(false);
}

void EthernetManagement::DevInterfaceAdd(const std::string &devName)
{
    NETMGR_EXT_LOG_D("Interface name:[%{public}s] add.", devName.c_str());
    std::unique_lock<std::mutex> lock(mutex_);
    auto fitDev = devs_.find(devName);
    if (fitDev != devs_.end()) {
        NETMGR_EXT_LOG_D("Interface name:[%{public}s] has added.", devName.c_str());
        return;
    }
    sptr<DevInterfaceState> devState = new (std::nothrow) DevInterfaceState();
    if (devState == nullptr) {
        NETMGR_EXT_LOG_E("devState is nullptr");
        return;
    }
    devs_.insert(std::make_pair(devName, devState));
    devState->SetDevName(devName);
    devState->RemoteRegisterNetSupplier();
    auto fitCfg = devCfgs_.find(devName);
    if (fitCfg != devCfgs_.end()) {
        devState->SetIfcfg(fitCfg->second);
    } else {
        sptr<InterfaceConfiguration> ifCfg = new (std::nothrow) InterfaceConfiguration();
        if (ifCfg == nullptr) {
            NETMGR_EXT_LOG_E("ifCfg is nullptr");
            return;
        }
        ifCfg->mode_ = DHCP;
        devState->SetIfcfg(ifCfg);
    }
    auto fitCap = devCaps_.find(devName);
    if (fitCap != devCaps_.end()) {
        devState->SetNetCaps(fitCap->second);
    }
}

void EthernetManagement::DevInterfaceRemove(const std::string &devName)
{
    NETMGR_EXT_LOG_D("Interface name:[%{public}s] remove.", devName.c_str());
    std::unique_lock<std::mutex> lock(mutex_);
    auto fitDev = devs_.find(devName);
    if (fitDev != devs_.end()) {
        if (fitDev->second != nullptr) {
            fitDev->second->RemoteUnregisterNetSupplier();
        }
        devs_.erase(fitDev);
    }
}

void EthernetManagement::GetDumpInfo(std::string &info)
{
    std::for_each(devs_.begin(), devs_.end(), [&info](const auto &dev) { dev.second->GetDumpInfo(info); });
}
} // namespace NetManagerStandard
} // namespace OHOS
