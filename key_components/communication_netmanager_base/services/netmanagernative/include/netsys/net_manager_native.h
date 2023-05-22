/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef INCLUDE_NET_MANAGER_NATIVE_H
#define INCLUDE_NET_MANAGER_NATIVE_H

#include <memory>
#include <string>
#include <vector>

#include "bandwidth_manager.h"
#include "conn_manager.h"
#include "dns_manager.h"
#include "firewall_manager.h"
#include "interface_manager.h"
#include "interface_type.h"
#include "route_manager.h"
#include "route_type.h"
#include "sharing_manager.h"

namespace OHOS {
namespace nmd {
class NetManagerNative {
public:
    NetManagerNative();
    ~NetManagerNative() = default;

    static void GetOriginInterfaceIndex();
    static std::vector<uint32_t> GetCurrentInterfaceIndex();
    static void UpdateInterfaceIndex(uint32_t infIndex);

    void Init();

    int32_t NetworkReinitRoute();
    int32_t NetworkCreatePhysical(int32_t netId, int32_t permission);
    int32_t NetworkDestroy(int32_t netId);
    int32_t NetworkAddInterface(int32_t netId, std::string iface);
    int32_t NetworkRemoveInterface(int32_t netId, std::string iface);

    MarkMaskParcel GetFwmarkForNetwork(int32_t netId);
    int32_t NetworkAddRoute(int32_t netId, std::string ifName, std::string destination, std::string nextHop);
    int32_t NetworkRemoveRoute(int32_t netId, std::string ifName, std::string destination, std::string nextHop);
    int32_t NetworkGetDefault();
    int32_t NetworkSetDefault(int32_t netId);
    int32_t NetworkClearDefault();
    int32_t NetworkSetPermissionForNetwork(int32_t netId, NetworkPermission permission);
    std::vector<std::string> InterfaceGetList();

    int32_t SetProcSysNet(int32_t family, int32_t which, const std::string ifname, const std::string parameter,
                          const std::string value);
    int32_t GetProcSysNet(int32_t family, int32_t which, const std::string ifname, const std::string parameter,
                          std::string *value);

    nmd::InterfaceConfigurationParcel GetInterfaceConfig(std::string ifName);
    void SetInterfaceConfig(InterfaceConfigurationParcel cfg);
    void ClearInterfaceAddrs(const std::string ifName);
    int32_t GetInterfaceMtu(std::string ifName);
    int32_t SetInterfaceMtu(std::string ifName, int32_t mtuValue);
    int32_t AddInterfaceAddress(std::string ifName, std::string addrString, int32_t prefixLength);
    int32_t DelInterfaceAddress(std::string ifName, std::string addrString, int32_t prefixLength);
    int32_t InterfaceSetIpAddress(const std::string &ifaceName, const std::string &ipAddress);
    int32_t InterfaceSetIffUp(std::string ifaceName);
    int32_t NetworkAddRouteParcel(int32_t netId, RouteInfoParcel routeInfo);
    int32_t NetworkRemoveRouteParcel(int32_t netId, RouteInfoParcel routeInfo);

    int64_t GetCellularRxBytes();
    int64_t GetCellularTxBytes();
    int64_t GetAllRxBytes();
    int64_t GetAllTxBytes();
    int64_t GetUidTxBytes(int32_t uid);
    int64_t GetUidRxBytes(int32_t uid);
    int64_t GetIfaceRxBytes(std::string interfaceName);
    int64_t GetIfaceTxBytes(std::string interfaceName);
    int32_t IpEnableForwarding(const std::string &requester);
    int32_t IpDisableForwarding(const std::string &requester);
    int32_t EnableNat(const std::string &downstreamIface, const std::string &upstreamIface);
    int32_t DisableNat(const std::string &downstreamIface, const std::string &upsteramIface);
    int32_t IpfwdAddInterfaceForward(const std::string &fromIface, const std::string &toIface);
    int32_t IpfwdRemoveInterfaceForward(const std::string &fromIface, const std::string &toIface);

    int32_t DnsSetResolverConfig(uint16_t netId, uint16_t baseTimeoutMsec, uint8_t retryCount,
                                 const std::vector<std::string> &servers, const std::vector<std::string> &domains);
    int32_t DnsGetResolverConfig(uint16_t netId, std::vector<std::string> &servers, std::vector<std::string> &domains,
                                 uint16_t &baseTimeoutMsec, uint8_t &retryCount);
    int32_t DnsCreateNetworkCache(uint16_t netid);
    int32_t DnsDestroyNetworkCache(uint16_t netId);
    int32_t BandwidthEnableDataSaver(bool enable);
    int32_t BandwidthSetIfaceQuota(const std::string &ifName, int64_t bytes);
    int32_t BandwidthRemoveIfaceQuota(const std::string &ifName);
    int32_t BandwidthAddDeniedList(uint32_t uid);
    int32_t BandwidthRemoveDeniedList(uint32_t uid);
    int32_t BandwidthAddAllowedList(uint32_t uid);
    int32_t BandwidthRemoveAllowedList(uint32_t uid);

    int32_t FirewallSetUidsAllowedListChain(uint32_t chain, const std::vector<uint32_t> &uids);
    int32_t FirewallSetUidsDeniedListChain(uint32_t chain, const std::vector<uint32_t> &uids);
    int32_t FirewallEnableChain(uint32_t chain, bool enable);
    int32_t FirewallSetUidRule(uint32_t chain, uint32_t uid, uint32_t firewallRule);
    void ShareDnsSet(uint16_t netId);
    void StartDnsProxyListen();
    void StopDnsProxyListen();
    void GetDumpInfo(std::string &infos);
    int32_t DnsGetAddrInfo(const std::string &hostName, const std::string &serverName, const AddrInfo &hints,
                           uint16_t netId, std::vector<AddrInfo> &res);

private:
    std::shared_ptr<BandwidthManager> bandwidthManager_ = nullptr;
    std::shared_ptr<ConnManager> connManager_ = nullptr;
    std::shared_ptr<FirewallManager> firewallManager_ = nullptr;
    std::shared_ptr<RouteManager> routeManager_ = nullptr;
    std::shared_ptr<InterfaceManager> interfaceManager_ = nullptr;
    std::shared_ptr<SharingManager> sharingManager_ = nullptr;
    std::shared_ptr<DnsManager> dnsManager_ = nullptr;
    static std::vector<uint32_t> interfaceIdex_;
};
} // namespace nmd
} // namespace OHOS
#endif // !INCLUDE_NET_MANAGER_NATIVE_H
