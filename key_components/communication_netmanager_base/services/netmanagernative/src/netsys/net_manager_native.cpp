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

#include <net/if.h>

#include "interface_manager.h"
#include "net_manager_constants.h"
#include "netmanager_base_common_utils.h"
#include "netnative_log_wrapper.h"
#include "network_permission.h"
#include "route_manager.h"
#include "traffic_manager.h"

#include "net_manager_native.h"

using namespace OHOS::NetManagerStandard::CommonUtils;
std::vector<uint32_t> OHOS::nmd::NetManagerNative::interfaceIdex_;

namespace OHOS {
namespace nmd {
NetManagerNative::NetManagerNative()
    : bandwidthManager_(std::make_shared<BandwidthManager>()),
      connManager_(std::make_shared<ConnManager>()),
      firewallManager_(std::make_shared<FirewallManager>()),
      routeManager_(std::make_shared<RouteManager>()),
      interfaceManager_(std::make_shared<InterfaceManager>()),
      sharingManager_(std::make_shared<SharingManager>()),
      dnsManager_(std::make_shared<DnsManager>())
{
}

void NetManagerNative::GetOriginInterfaceIndex()
{
    std::vector<std::string> ifNameList = InterfaceManager::GetInterfaceNames();
    interfaceIdex_.clear();
    for (auto iter = ifNameList.begin(); iter != ifNameList.end(); ++iter) {
        uint32_t infIndex = if_nametoindex((*iter).c_str());
        interfaceIdex_.push_back(infIndex);
    }
}

void NetManagerNative::UpdateInterfaceIndex(uint32_t infIndex)
{
    interfaceIdex_.push_back(infIndex);
}

std::vector<uint32_t> NetManagerNative::GetCurrentInterfaceIndex()
{
    return interfaceIdex_;
}

void NetManagerNative::Init()
{
    GetOriginInterfaceIndex();
}

int32_t NetManagerNative::NetworkReinitRoute()
{
    return connManager_->ReinitRoute();
}

int32_t NetManagerNative::NetworkCreatePhysical(int32_t netId, int32_t permission)
{
    return connManager_->CreatePhysicalNetwork(static_cast<uint16_t>(netId),
                                               static_cast<NetworkPermission>(permission));
}

int32_t NetManagerNative::NetworkDestroy(int32_t netId)
{
    return connManager_->DestroyNetwork(netId);
}

int32_t NetManagerNative::NetworkAddInterface(int32_t netId, std::string interfaceName)
{
    return connManager_->AddInterfaceToNetwork(netId, interfaceName);
}

int32_t NetManagerNative::NetworkRemoveInterface(int32_t netId, std::string interfaceName)
{
    return connManager_->RemoveInterfaceFromNetwork(netId, interfaceName);
}

int32_t NetManagerNative::AddInterfaceAddress(std::string ifName, std::string addrString, int32_t prefixLength)
{
    return interfaceManager_->AddAddress(ifName.c_str(), addrString.c_str(), prefixLength);
}

int32_t NetManagerNative::DelInterfaceAddress(std::string ifName, std::string addrString, int32_t prefixLength)
{
    return interfaceManager_->DelAddress(ifName.c_str(), addrString.c_str(), prefixLength);
}

int32_t NetManagerNative::NetworkAddRoute(int32_t netId, std::string interfaceName, std::string destination,
                                          std::string nextHop)
{
    return connManager_->AddRoute(netId, interfaceName, destination, nextHop);
}

int32_t NetManagerNative::NetworkRemoveRoute(int32_t netId, std::string interfaceName, std::string destination,
                                             std::string nextHop)
{
    return connManager_->RemoveRoute(netId, interfaceName, destination, nextHop);
}

int32_t NetManagerNative::NetworkGetDefault()
{
    return connManager_->GetDefaultNetwork();
}

int32_t NetManagerNative::NetworkSetDefault(int32_t netId)
{
    dnsManager_->SetDefaultNetwork(netId);
    return connManager_->SetDefaultNetwork(netId);
}

int32_t NetManagerNative::NetworkClearDefault()
{
    return connManager_->ClearDefaultNetwork();
}

int32_t NetManagerNative::NetworkSetPermissionForNetwork(int32_t netId, NetworkPermission permission)
{
    return connManager_->SetPermissionForNetwork(netId, permission);
}

std::vector<std::string> NetManagerNative::InterfaceGetList()
{
    return InterfaceManager::GetInterfaceNames();
}

nmd::InterfaceConfigurationParcel NetManagerNative::GetInterfaceConfig(std::string interfaceName)
{
    return InterfaceManager::GetIfaceConfig(interfaceName.c_str());
}

void NetManagerNative::SetInterfaceConfig(nmd::InterfaceConfigurationParcel parcel)
{
    InterfaceManager::SetIfaceConfig(parcel);
}

void NetManagerNative::ClearInterfaceAddrs(const std::string ifName) {}

int32_t NetManagerNative::GetInterfaceMtu(std::string ifName)
{
    return InterfaceManager::GetMtu(ifName.c_str());
}

int32_t NetManagerNative::SetInterfaceMtu(std::string ifName, int32_t mtuValue)
{
    std::string mtu = std::to_string(mtuValue);
    return InterfaceManager::SetMtu(ifName.c_str(), mtu.c_str());
}

int32_t NetManagerNative::InterfaceSetIpAddress(const std::string &ifaceName, const std::string &ipAddress)
{
    return InterfaceManager::SetIpAddress(ifaceName.c_str(), ipAddress.c_str());
}

int32_t NetManagerNative::InterfaceSetIffUp(std::string ifaceName)
{
    return InterfaceManager::SetIffUp(ifaceName.c_str());
}

nmd::MarkMaskParcel NetManagerNative::GetFwmarkForNetwork(int32_t netId)
{
    nmd::MarkMaskParcel mark;
    mark.mark = connManager_->GetFwmarkForNetwork(netId);
    mark.mask = 0XFFFF;
    return mark;
}

int32_t NetManagerNative::NetworkAddRouteParcel(int32_t netId, RouteInfoParcel parcel)
{
    return connManager_->AddRoute(netId, parcel.ifName, parcel.destination, parcel.nextHop);
}

int32_t NetManagerNative::NetworkRemoveRouteParcel(int32_t netId, RouteInfoParcel parcel)
{
    return connManager_->RemoveRoute(netId, parcel.ifName, parcel.destination, parcel.nextHop);
}

int32_t NetManagerNative::SetProcSysNet(int32_t family, int32_t which, const std::string ifname,
                                        const std::string parameter, const std::string value)
{
    return 0;
}

int32_t NetManagerNative::GetProcSysNet(int32_t family, int32_t which, const std::string ifname,
                                        const std::string parameter, std::string *value)
{
    return 0;
}

int64_t NetManagerNative::GetCellularRxBytes()
{
    return 0;
}

int64_t NetManagerNative::GetCellularTxBytes()
{
    return 0;
}

int64_t NetManagerNative::GetAllRxBytes()
{
    return nmd::TrafficManager::GetAllRxTraffic();
}

int64_t NetManagerNative::GetAllTxBytes()
{
    return nmd::TrafficManager::GetAllTxTraffic();
}

int64_t NetManagerNative::GetUidTxBytes(int32_t uid)
{
    return 0;
}

int64_t NetManagerNative::GetUidRxBytes(int32_t uid)
{
    return 0;
}

int64_t NetManagerNative::GetIfaceRxBytes(std::string interfaceName)
{
    nmd::TrafficStatsParcel interfaceTraffic = nmd::TrafficManager::GetInterfaceTraffic(interfaceName);
    return interfaceTraffic.rxBytes;
}

int64_t NetManagerNative::GetIfaceTxBytes(std::string interfaceName)
{
    nmd::TrafficStatsParcel interfaceTraffic = nmd::TrafficManager::GetInterfaceTraffic(interfaceName);
    return interfaceTraffic.txBytes;
}

int32_t NetManagerNative::IpEnableForwarding(const std::string &requester)
{
    return sharingManager_->IpEnableForwarding(requester);
}

int32_t NetManagerNative::IpDisableForwarding(const std::string &requester)
{
    return sharingManager_->IpDisableForwarding(requester);
}

int32_t NetManagerNative::EnableNat(const std::string &downstreamIface, const std::string &upstreamIface)
{
    return sharingManager_->EnableNat(downstreamIface, upstreamIface);
}

int32_t NetManagerNative::DisableNat(const std::string &downstreamIface, const std::string &upstreamIface)
{
    return sharingManager_->DisableNat(downstreamIface, upstreamIface);
}

int32_t NetManagerNative::IpfwdAddInterfaceForward(const std::string &fromIface, const std::string &toIface)
{
    return sharingManager_->IpfwdAddInterfaceForward(fromIface, toIface);
}

int32_t NetManagerNative::IpfwdRemoveInterfaceForward(const std::string &fromIface, const std::string &toIface)
{
    return sharingManager_->IpfwdRemoveInterfaceForward(fromIface, toIface);
}

int32_t NetManagerNative::DnsSetResolverConfig(uint16_t netId, uint16_t baseTimeoutMsec, uint8_t retryCount,
                                               const std::vector<std::string> &servers,
                                               const std::vector<std::string> &domains)
{
    return dnsManager_->SetResolverConfig(netId, baseTimeoutMsec, retryCount, servers, domains);
}

int32_t NetManagerNative::DnsGetResolverConfig(uint16_t netId, std::vector<std::string> &servers,
                                               std::vector<std::string> &domains, uint16_t &baseTimeoutMsec,
                                               uint8_t &retryCount)
{
    return dnsManager_->GetResolverConfig(netId, servers, domains, baseTimeoutMsec, retryCount);
}

int32_t NetManagerNative::DnsCreateNetworkCache(uint16_t netId)
{
    return dnsManager_->CreateNetworkCache(netId);
}

int32_t NetManagerNative::DnsDestroyNetworkCache(uint16_t netId)
{
    return dnsManager_->DestroyNetworkCache(netId);
}

int32_t NetManagerNative::BandwidthEnableDataSaver(bool enable)
{
    return bandwidthManager_->EnableDataSaver(enable);
}

int32_t NetManagerNative::BandwidthSetIfaceQuota(const std::string &ifName, int64_t bytes)
{
    return bandwidthManager_->SetIfaceQuota(ifName, bytes);
}

int32_t NetManagerNative::BandwidthRemoveIfaceQuota(const std::string &ifName)
{
    return bandwidthManager_->RemoveIfaceQuota(ifName);
}

int32_t NetManagerNative::BandwidthAddDeniedList(uint32_t uid)
{
    return bandwidthManager_->AddDeniedList(uid);
}

int32_t NetManagerNative::BandwidthRemoveDeniedList(uint32_t uid)
{
    return bandwidthManager_->RemoveDeniedList(uid);
}

int32_t NetManagerNative::BandwidthAddAllowedList(uint32_t uid)
{
    return bandwidthManager_->AddAllowedList(uid);
}

int32_t NetManagerNative::BandwidthRemoveAllowedList(uint32_t uid)
{
    return bandwidthManager_->RemoveAllowedList(uid);
}

int32_t NetManagerNative::FirewallSetUidsAllowedListChain(uint32_t chain, const std::vector<uint32_t> &uids)
{
    auto chainType = static_cast<NetManagerStandard::ChainType>(chain);
    return firewallManager_->SetUidsAllowedListChain(chainType, uids);
}

int32_t NetManagerNative::FirewallSetUidsDeniedListChain(uint32_t chain, const std::vector<uint32_t> &uids)
{
    auto chainType = static_cast<NetManagerStandard::ChainType>(chain);
    return firewallManager_->SetUidsDeniedListChain(chainType, uids);
}

int32_t NetManagerNative::FirewallEnableChain(uint32_t chain, bool enable)
{
    auto chainType = static_cast<NetManagerStandard::ChainType>(chain);
    return firewallManager_->EnableChain(chainType, enable);
}

int32_t NetManagerNative::FirewallSetUidRule(uint32_t chain, uint32_t uid, uint32_t firewallRule)
{
    auto chainType = static_cast<NetManagerStandard::ChainType>(chain);
    auto rule = static_cast<NetManagerStandard::FirewallRule>(firewallRule);
    return firewallManager_->SetUidRule(chainType, uid, rule);
}

void NetManagerNative::ShareDnsSet(uint16_t netId)
{
    dnsManager_->ShareDnsSet(netId);
}

void NetManagerNative::StartDnsProxyListen()
{
    dnsManager_->StartDnsProxyListen();
}

void NetManagerNative::StopDnsProxyListen()
{
    dnsManager_->StopDnsProxyListen();
}

int32_t NetManagerNative::DnsGetAddrInfo(const std::string &hostName, const std::string &serverName,
                                         const AddrInfo &hints, uint16_t netId, std::vector<AddrInfo> &res)
{
    return dnsManager_->GetAddrInfo(hostName, serverName, hints, netId, res);
}

void NetManagerNative::GetDumpInfo(std::string &infos)
{
    connManager_->GetDumpInfos(infos);
    dnsManager_->GetDumpInfo(infos);
}
} // namespace nmd
} // namespace OHOS
