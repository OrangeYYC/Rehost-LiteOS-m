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

#include "dev_interface_state.h"

#include "inet_addr.h"
#include "net_manager_center.h"
#include "net_manager_constants.h"
#include "netmanager_base_common_utils.h"
#include "netmgr_ext_log_wrapper.h"
#include "netsys_controller.h"
#include "route.h"
#include "static_configuration.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr const char *DEFAULT_ROUTE_ADDR = "0.0.0.0";
}

DevInterfaceState::DevInterfaceState()
{
    netSupplierInfo_ = new (std::nothrow) NetSupplierInfo();
    if (netSupplierInfo_ == nullptr) {
        NETMGR_EXT_LOG_E("NetSupplierInfo new failed");
    }
}

void DevInterfaceState::SetDevName(const std::string &devName)
{
    devName_ = devName;
}

void DevInterfaceState::SetNetCaps(const std::set<NetCap> &netCaps)
{
    netCaps_ = netCaps;
}

void DevInterfaceState::SetLinkUp(bool up)
{
    linkUp_ = up;
}

void DevInterfaceState::SetlinkInfo(sptr<NetLinkInfo> &linkInfo)
{
    linkInfo_ = linkInfo;
}

void DevInterfaceState::SetIfcfg(sptr<InterfaceConfiguration> &ifCfg)
{
    ifCfg_ = ifCfg;
    if (ifCfg_->mode_ == STATIC) {
        UpdateLinkInfo();
        if (connLinkState_ == LINK_AVAILABLE) {
            RemoteUpdateNetLinkInfo();
        }
    }
}

void DevInterfaceState::SetDhcpReqState(bool dhcpReqState)
{
    dhcpReqState_ = dhcpReqState;
}

std::string DevInterfaceState::GetDevName() const
{
    return devName_;
}

const std::set<NetCap> &DevInterfaceState::GetNetCaps() const
{
    return netCaps_;
}

std::set<NetCap> DevInterfaceState::GetNetCaps()
{
    return netCaps_;
}

bool DevInterfaceState::GetLinkUp() const
{
    return linkUp_;
}

sptr<NetLinkInfo> DevInterfaceState::GetLinkInfo() const
{
    return linkInfo_;
}

sptr<InterfaceConfiguration> DevInterfaceState::GetIfcfg() const
{
    return ifCfg_;
}

IPSetMode DevInterfaceState::GetIPSetMode() const
{
    if (ifCfg_ == nullptr) {
        return IPSetMode::STATIC;
    }
    return ifCfg_->mode_;
}

bool DevInterfaceState::GetDhcpReqState() const
{
    return dhcpReqState_;
}

void DevInterfaceState::RemoteRegisterNetSupplier()
{
    if (connLinkState_ == UNREGISTERED) {
        if (netCaps_.empty()) {
            netCaps_.insert(NET_CAPABILITY_INTERNET);
        }
        int32_t result =
            NetManagerCenter::GetInstance().RegisterNetSupplier(bearerType_, devName_, netCaps_, netSupplier_);
        if (result == NETMANAGER_SUCCESS) {
            connLinkState_ = REGISTERED;
        }
        NETMGR_EXT_LOG_D("DevInterfaceCfg RemoteRegisterNetSupplier netSupplier_[%{public}d]", netSupplier_);
    }
}

void DevInterfaceState::RemoteUnregisterNetSupplier()
{
    if (connLinkState_ == UNREGISTERED) {
        return;
    }
    int ret = NetManagerCenter::GetInstance().UnregisterNetSupplier(netSupplier_);
    if (ret == NETMANAGER_SUCCESS) {
        connLinkState_ = UNREGISTERED;
        netSupplier_ = 0;
    }
}

void DevInterfaceState::RemoteUpdateNetLinkInfo()
{
    if (connLinkState_ == LINK_UNAVAILABLE) {
        NETMGR_EXT_LOG_E("DevInterfaceCfg RemoteUpdateNetLinkInfo regState_:LINK_UNAVAILABLE");
        return;
    }
    if (linkInfo_ == nullptr) {
        NETMGR_EXT_LOG_E("DevInterfaceCfg RemoteUpdateNetLinkInfo linkInfo_ is nullptr");
        return;
    }
    NetManagerCenter::GetInstance().UpdateNetLinkInfo(netSupplier_, linkInfo_);
}

void DevInterfaceState::RemoteUpdateNetSupplierInfo()
{
    if (connLinkState_ == UNREGISTERED) {
        NETMGR_EXT_LOG_E("DevInterfaceCfg RemoteUpdateNetSupplierInfo regState_:UNREGISTERED");
        return;
    }
    if (netSupplierInfo_ == nullptr) {
        NETMGR_EXT_LOG_E("DevInterfaceCfg RemoteUpdateNetSupplierInfo netSupplierInfo_ is nullptr");
        return;
    }
    UpdateSupplierAvailable();
    NetManagerCenter::GetInstance().UpdateNetSupplierInfo(netSupplier_, netSupplierInfo_);
}

void DevInterfaceState::UpdateLinkInfo()
{
    if (ifCfg_ == nullptr || ifCfg_->mode_ != STATIC) {
        return;
    }
    if (linkInfo_ == nullptr) {
        linkInfo_ = new (std::nothrow) NetLinkInfo();
        if (linkInfo_ == nullptr) {
            NETMGR_EXT_LOG_E("linkInfo_ is nullptr");
            return;
        }
    }
    std::list<INetAddr>().swap(linkInfo_->netAddrList_);
    std::list<Route>().swap(linkInfo_->routeList_);
    std::list<INetAddr>().swap(linkInfo_->dnsList_);
    linkInfo_->ifaceName_ = devName_;
    linkInfo_->netAddrList_.push_back(ifCfg_->ipStatic_.ipAddr_);
    Route route;
    route.iface_ = devName_;
    route.destination_ = ifCfg_->ipStatic_.route_;
    route.gateway_ = ifCfg_->ipStatic_.gateway_;
    linkInfo_->routeList_.push_back(route);
    const auto &routeLocal =
        CreateLocalRoute(devName_, ifCfg_->ipStatic_.ipAddr_.address_, ifCfg_->ipStatic_.netMask_.address_);
    linkInfo_->routeList_.push_back(routeLocal);
    for (auto dnsServer : ifCfg_->ipStatic_.dnsServers_) {
        linkInfo_->dnsList_.push_back(dnsServer);
    }
}
void DevInterfaceState::UpdateLinkInfo(const INetAddr &ipAddr, const INetAddr &netMask, const INetAddr &gateWay,
                                       const INetAddr &route, const INetAddr &dns1, const INetAddr &dns2)
{
    if (linkInfo_ == nullptr) {
        linkInfo_ = new (std::nothrow) NetLinkInfo();
        if (linkInfo_ == nullptr) {
            NETMGR_EXT_LOG_E("NetLinkInfo new failed");
        }
    }
    std::list<INetAddr>().swap(linkInfo_->netAddrList_);
    std::list<Route>().swap(linkInfo_->routeList_);
    std::list<INetAddr>().swap(linkInfo_->dnsList_);
    linkInfo_->ifaceName_ = devName_;
    linkInfo_->netAddrList_.push_back(ipAddr);
    Route routeStc;
    routeStc.iface_ = devName_;
    routeStc.destination_ = route;
    routeStc.gateway_ = gateWay;
    linkInfo_->routeList_.push_back(routeStc);
    Route routeLocal = CreateLocalRoute(devName_, ipAddr.address_, netMask.address_);
    linkInfo_->routeList_.push_back(routeLocal);
    linkInfo_->dnsList_.push_back(dns1);
    linkInfo_->dnsList_.push_back(dns2);
}

void DevInterfaceState::UpdateSupplierAvailable()
{
    netSupplierInfo_->isAvailable_ = linkUp_;
    connLinkState_ = linkUp_ ? LINK_AVAILABLE : LINK_UNAVAILABLE;
}

Route DevInterfaceState::CreateLocalRoute(const std::string &iface, const std::string &ipAddr,
                                          const std::string &maskAddr)
{
    NETMGR_EXT_LOG_D("create local route ipAddr=%{public}s maskAddr=%{public}s.",
                     CommonUtils::ToAnonymousIp(ipAddr).c_str(), maskAddr.c_str());
    Route localRoute;
    int prefixLength = CommonUtils::GetMaskLength(maskAddr);
    uint32_t ipInt = CommonUtils::ConvertIpv4Address(ipAddr);
    uint32_t maskInt = CommonUtils::ConvertIpv4Address(maskAddr);
    std::string strLocalRoute = CommonUtils::ConvertIpv4Address(ipInt & maskInt);
    localRoute.iface_ = iface;
    localRoute.destination_.type_ = INetAddr::IPV4;
    localRoute.destination_.address_ = strLocalRoute;
    localRoute.destination_.prefixlen_ = prefixLength;
    localRoute.gateway_.address_ = DEFAULT_ROUTE_ADDR;
    return localRoute;
}

void DevInterfaceState::GetDumpInfo(std::string &info)
{
    const std::string TAB = "  ";
    std::list<std::string> dumpInfo = {
        "DevName: " + devName_,
        "ConnLinkState: " + std::to_string(connLinkState_),
        "LinkUp: " + std::to_string(linkUp_),
        "DHCPReqState: " + std::to_string(dhcpReqState_),
    };
    std::string data = "DevInterfaceState: ";
    std::for_each(dumpInfo.begin(), dumpInfo.end(),
                  [&data, &TAB](const auto &msg) { data.append(TAB + TAB + msg + "\n"); });
    if (linkInfo_ != nullptr) {
        data.append(linkInfo_->ToString(TAB));
    }
    if (netSupplierInfo_ != nullptr) {
        data.append(netSupplierInfo_->ToString(TAB));
    }
    if (ifCfg_ != nullptr) {
        data.append(TAB + TAB + "InterfaceConfig: \n" + TAB + TAB + TAB + "Mode: " + std::to_string(ifCfg_->mode_) +
                    "\n");
        data.append("\nConfig: \n");
        data.append(TAB + TAB + "IpAddr:" + ifCfg_->ipStatic_.ipAddr_.ToString(TAB) + "\n" + TAB + TAB +
                    "Route: " + ifCfg_->ipStatic_.route_.ToString(TAB) + "\n" + TAB + TAB +
                    "GateWay: " + ifCfg_->ipStatic_.gateway_.ToString(TAB) + "\n" + TAB + TAB +
                    "NetMask: " + ifCfg_->ipStatic_.netMask_.ToString(TAB) + "\n" + TAB + TAB + "DNSServers: \n");
        std::for_each(ifCfg_->ipStatic_.dnsServers_.begin(), ifCfg_->ipStatic_.dnsServers_.end(),
                      [&data, &TAB](const auto &server) { data.append(TAB + TAB + server.ToString(TAB)); });
        data.append(TAB + TAB + "Domain: " + ifCfg_->ipStatic_.domain_ + "\n" + TAB + TAB + "NetCaps: {");
        std::for_each(netCaps_.begin(), netCaps_.end(),
                      [&data, &TAB](const auto &cap) { data.append(std::to_string(cap) + ", "); });
        data.append("}\n");
    }
    data.append(TAB + TAB + "BearerType :" + std::to_string(bearerType_) + "\n");
    info.append(data);
}
} // namespace NetManagerStandard
} // namespace OHOS
