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
#ifndef I_NETSYS_SERVICE_H
#define I_NETSYS_SERVICE_H

#include <netdb.h>
#include <string>

#include "dns_config_client.h"
#include "i_notify_callback.h"
#include "interface_type.h"
#include "iremote_broker.h"
#include "route_type.h"
#include "network_sharing.h"

namespace OHOS {
namespace NetsysNative {
using namespace nmd;
class INetsysService : public IRemoteBroker {
public:
    enum {
        NETSYS_SET_RESOLVER_CONFIG_PARCEL,
        NETSYS_SET_RESOLVER_CONFIG,
        NETSYS_GET_RESOLVER_CONFIG,
        NETSYS_CREATE_NETWORK_CACHE,
        NETSYS_FLUSH_NETWORK_CACHE,
        NETSYS_DESTROY_NETWORK_CACHE,
        NETSYS_GET_ADDR_INFO,
        NETSYS_INTERFACE_SET_MTU,
        NETSYS_INTERFACE_GET_MTU,
        NETSYS_REGISTER_NOTIFY_CALLBACK,
        NETSYS_UNREGISTER_NOTIFY_CALLBACK,
        NETSYS_NETWORK_ADD_ROUTE,
        NETSYS_NETWORK_REMOVE_ROUTE,
        NETSYS_NETWORK_ADD_ROUTE_PARCEL,
        NETSYS_NETWORK_REMOVE_ROUTE_PARCEL,
        NETSYS_NETWORK_SET_DEFAULT,
        NETSYS_NETWORK_GET_DEFAULT,
        NETSYS_NETWORK_CLEAR_DEFAULT,
        NETSYS_GET_PROC_SYS_NET,
        NETSYS_SET_PROC_SYS_NET,
        NETSYS_NETWORK_CREATE_PHYSICAL,
        NETSYS_INTERFACE_ADD_ADDRESS,
        NETSYS_INTERFACE_DEL_ADDRESS,
        NETSYS_INTERFACE_SET_IP_ADDRESS,
        NETSYS_INTERFACE_SET_IFF_UP,
        NETSYS_NETWORK_ADD_INTERFACE,
        NETSYS_NETWORK_REMOVE_INTERFACE,
        NETSYS_NETWORK_DESTROY,
        NETSYS_GET_FWMARK_FOR_NETWORK,
        NETSYS_INTERFACE_SET_CONFIG,
        NETSYS_INTERFACE_GET_CONFIG,
        NETSYS_INTERFACE_GET_LIST,
        NETSYS_START_DHCP_CLIENT,
        NETSYS_STOP_DHCP_CLIENT,
        NETSYS_START_DHCP_SERVICE,
        NETSYS_STOP_DHCP_SERVICE,
        NETSYS_IPENABLE_FORWARDING,
        NETSYS_IPDISABLE_FORWARDING,
        NETSYS_ENABLE_NAT,
        NETSYS_DISABLE_NAT,
        NETSYS_IPFWD_ADD_INTERFACE_FORWARD,
        NETSYS_IPFWD_REMOVE_INTERFACE_FORWARD,
        NETSYS_BANDWIDTH_ENABLE_DATA_SAVER,
        NETSYS_BANDWIDTH_SET_IFACE_QUOTA,
        NETSYS_BANDWIDTH_REMOVE_IFACE_QUOTA,
        NETSYS_BANDWIDTH_ADD_DENIED_LIST,
        NETSYS_BANDWIDTH_REMOVE_DENIED_LIST,
        NETSYS_BANDWIDTH_ADD_ALLOWED_LIST,
        NETSYS_BANDWIDTH_REMOVE_ALLOWED_LIST,
        NETSYS_FIREWALL_SET_UID_ALLOWED_LIST_CHAIN,
        NETSYS_FIREWALL_SET_UID_DENIED_LIST_CHAIN,
        NETSYS_FIREWALL_ENABLE_CHAIN,
        NETSYS_FIREWALL_SET_UID_RULE,
        NETSYS_TETHER_DNS_SET,
        NETSYS_START_DNS_PROXY_LISTEN,
        NETSYS_STOP_DNS_PROXY_LISTEN,
        NETSYS_GET_SHARING_NETWORK_TRAFFIC,
    };

    virtual int32_t SetResolverConfig(uint16_t netId, uint16_t baseTimeoutMsec, uint8_t retryCount,
                                      const std::vector<std::string> &servers,
                                      const std::vector<std::string> &domains) = 0;
    virtual int32_t GetResolverConfig(uint16_t netId, std::vector<std::string> &servers,
                                      std::vector<std::string> &domains, uint16_t &baseTimeoutMsec,
                                      uint8_t &retryCount) = 0;
    virtual int32_t CreateNetworkCache(uint16_t netId) = 0;
    virtual int32_t DestroyNetworkCache(uint16_t netId) = 0;
    virtual int32_t GetAddrInfo(const std::string &hostName, const std::string &serverName, const AddrInfo &hints,
                                uint16_t netId, std::vector<AddrInfo> &res) = 0;
    virtual int32_t SetInterfaceMtu(const std::string &interfaceName, int mtu) = 0;
    virtual int32_t GetInterfaceMtu(const std::string &interfaceName) = 0;

    virtual int32_t RegisterNotifyCallback(sptr<INotifyCallback> &callback) = 0;
    virtual int32_t UnRegisterNotifyCallback(sptr<INotifyCallback> &callback) = 0;

    virtual int32_t NetworkAddRoute(int32_t netId, const std::string &interfaceName, const std::string &destination,
                                    const std::string &nextHop) = 0;
    virtual int32_t NetworkRemoveRoute(int32_t netId, const std::string &interfaceName, const std::string &destination,
                                       const std::string &nextHop) = 0;
    virtual int32_t NetworkAddRouteParcel(int32_t netId, const RouteInfoParcel &routeInfo) = 0;
    virtual int32_t NetworkRemoveRouteParcel(int32_t netId, const RouteInfoParcel &routeInfo) = 0;
    virtual int32_t NetworkSetDefault(int32_t netId) = 0;
    virtual int32_t NetworkGetDefault() = 0;
    virtual int32_t NetworkClearDefault() = 0;
    virtual int32_t GetProcSysNet(int32_t family, int32_t which, const std::string &ifname,
                                  const std::string &parameter, std::string &value) = 0;
    virtual int32_t SetProcSysNet(int32_t family, int32_t which, const std::string &ifname,
                                  const std::string &parameter, std::string &value) = 0;
    virtual int32_t NetworkCreatePhysical(int32_t netId, int32_t permission) = 0;
    virtual int32_t AddInterfaceAddress(const std::string &interfaceName, const std::string &addrString,
                                        int32_t prefixLength) = 0;
    virtual int32_t DelInterfaceAddress(const std::string &interfaceName, const std::string &addrString,
                                        int32_t prefixLength) = 0;
    virtual int32_t InterfaceSetIpAddress(const std::string &ifaceName, const std::string &ipAddress) = 0;
    virtual int32_t InterfaceSetIffUp(const std::string &ifaceName) = 0;
    virtual int32_t NetworkAddInterface(int32_t netId, const std::string &iface) = 0;
    virtual int32_t NetworkRemoveInterface(int32_t netId, const std::string &iface) = 0;
    virtual int32_t NetworkDestroy(int32_t netId) = 0;
    virtual int32_t GetFwmarkForNetwork(int32_t netId, MarkMaskParcel &markMaskParcel) = 0;
    virtual int32_t SetInterfaceConfig(const InterfaceConfigurationParcel &cfg) = 0;
    virtual int32_t GetInterfaceConfig(InterfaceConfigurationParcel &cfg) = 0;
    virtual int32_t InterfaceGetList(std::vector<std::string> &ifaces) = 0;
    virtual int32_t StartDhcpClient(const std::string &iface, bool bIpv6) = 0;
    virtual int32_t StopDhcpClient(const std::string &iface, bool bIpv6) = 0;
    virtual int32_t StartDhcpService(const std::string &iface, const std::string &ipv4addr) = 0;
    virtual int32_t StopDhcpService(const std::string &iface) = 0;
    virtual int32_t IpEnableForwarding(const std::string &requestor) = 0;
    virtual int32_t IpDisableForwarding(const std::string &requestor) = 0;
    virtual int32_t EnableNat(const std::string &downstreamIface, const std::string &upstreamIface) = 0;
    virtual int32_t DisableNat(const std::string &downstreamIface, const std::string &upstreamIface) = 0;
    virtual int32_t IpfwdAddInterfaceForward(const std::string &fromIface, const std::string &toIface) = 0;
    virtual int32_t IpfwdRemoveInterfaceForward(const std::string &fromIface, const std::string &toIface) = 0;
    virtual int32_t BandwidthAddAllowedList(uint32_t uid) = 0;
    virtual int32_t BandwidthRemoveAllowedList(uint32_t uid) = 0;
    virtual int32_t BandwidthEnableDataSaver(bool enable) = 0;
    virtual int32_t BandwidthSetIfaceQuota(const std::string &ifName, int64_t bytes) = 0;
    virtual int32_t BandwidthAddDeniedList(uint32_t uid) = 0;
    virtual int32_t BandwidthRemoveDeniedList(uint32_t uid) = 0;
    virtual int32_t BandwidthRemoveIfaceQuota(const std::string &ifName) = 0;
    virtual int32_t FirewallSetUidsAllowedListChain(uint32_t chain, const std::vector<uint32_t> &uids) = 0;
    virtual int32_t FirewallSetUidsDeniedListChain(uint32_t chain, const std::vector<uint32_t> &uids) = 0;
    virtual int32_t FirewallEnableChain(uint32_t chain, bool enable) = 0;
    virtual int32_t FirewallSetUidRule(uint32_t chain, uint32_t uid, uint32_t firewallRule) = 0;
    virtual int32_t ShareDnsSet(uint16_t netId) = 0;
    virtual int32_t StartDnsProxyListen() = 0;
    virtual int32_t StopDnsProxyListen() = 0;
    virtual int32_t GetNetworkSharingTraffic(const std::string &downIface, const std::string &upIface,
                                             NetworkSharingTraffic &traffic) = 0;
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.NetsysNative.INetsysService")
};
} // namespace NetsysNative
} // namespace OHOS
#endif // I_NETSYS_SERVICE_H
