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

#ifndef MOCK_NETSYS_NATIVE_CLIENT_H
#define MOCK_NETSYS_NATIVE_CLIENT_H

#include <string>
#include <vector>
#include <unordered_set>
#include <cstring>
#include <netdb.h>
#include <linux/if.h>

#include "netsys_controller_callback.h"
#include "netsys_controller_define.h"

namespace OHOS {
namespace NetManagerStandard {
constexpr const char *MOCK_NETWORKCREATEPHYSICAL_API = "NetworkCreatePhysical";
constexpr const char *MOCK_NETWORKDESTROY_API = "NetworkDestroy";
constexpr const char *MOCK_NETWORKADDINTERFACE_API = "NetworkAddInterface";
constexpr const char *MOCK_NETWORKREMOVEINTERFACE_API = "NetworkRemoveInterface";
constexpr const char *MOCK_NETWORKADDROUTE_API = "NetworkAddRoute";
constexpr const char *MOCK_NETWORKREMOVEROUTE_API = "NetworkRemoveRoute";
constexpr const char *MOCK_SETINTERFACEDOWN_API = "SetInterfaceDown";
constexpr const char *MOCK_SETINTERFACEUP_API = "SetInterfaceUp";
constexpr const char *MOCK_INTERFACECLEARADDRS_API = "ClearInterfaceAddrs";
constexpr const char *MOCK_INTERFACEGETMTU_API = "GetInterfaceMtu";
constexpr const char *MOCK_INTERFACESETMTU_API = "SetInterfaceMtu";
constexpr const char *MOCK_INTERFACEADDADDRESS_API = "AddInterfaceAddress";
constexpr const char *MOCK_INTERFACEDELADDRESS_API = "DelInterfaceAddress";
constexpr const char *MOCK_SETRESOLVERCONFIG_API = "SetResolverConfig";
constexpr const char *MOCK_GETRESOLVERICONFIG_API = "GetResolverConfig";
constexpr const char *MOCK_CREATENETWORKCACHE_API = "CreateNetworkCache";
constexpr const char *MOCK_DESTROYNETWORKCACHE_API = "DestroyNetworkCache";
constexpr const char *MOCK_GETCELLULARRXBYTES_API = "GetCellularRxBytes";
constexpr const char *MOCK_GETCELLULARTXBYTES_API = "GetCellularTxBytes";
constexpr const char *MOCK_GETALLRXBYTES_API = "GetAllRxBytes";
constexpr const char *MOCK_GETALLTXBYTES_API = "GetAllTxBytes";
constexpr const char *MOCK_GETUIDRXBYTES_API = "GetUidRxBytes";
constexpr const char *MOCK_GETUIDTXBYTES_API = "GetUidTxBytes";
constexpr const char *MOCK_GETUIDONIFACERXBYTES_API = "GetUidOnIfaceRxBytes";
constexpr const char *MOCK_GETUIDONIFACETXBYTES_API = "GetUidOnIfaceTxBytes";
constexpr const char *MOCK_GETIFACERXBYTES_API = "GetIfaceRxBytes";
constexpr const char *MOCK_GETIFACETXBYTES_API = "GetIfaceTxBytes";
constexpr const char *MOCK_INTERFACEGETLIST_API = "InterfaceGetList";
constexpr const char *MOCK_UIDGETLIST_API = "UidGetList";
constexpr const char *MOCK_GETIFACERXPACKETS_API = "GetIfaceRxPackets";
constexpr const char *MOCK_GETIFACETXPACKETS_API = "GetIfaceTxPackets";
constexpr const char *MOCK_SETDEFAULTNETWORK_API = "SetDefaultNetWork";
constexpr const char *MOCK_CLEARDEFAULTNETWORK_API = "ClearDefaultNetWorkNetId";
constexpr const char *MOCK_BINDSOCKET_API = "BindSocket";
constexpr const char *MOCK_SHAREDNSSET_API = "ShareDnsSet";
constexpr const char *MOCK_REGISTERNETSYSNOTIFYCALLBACK_API = "RegisterNetsysNotifyCallback";
constexpr const char *MOCK_BINDNETWORKSERVICEVPN_API = "BindNetworkServiceVpn";
constexpr const char *MOCK_ENABLEVIRTUALNETIFACECARD_API = "EnableVirtualNetIfaceCard";
constexpr const char *MOCK_SETIPADDRESS_API = "SetIpAddress";
constexpr const char *MOCK_SETBLOCKING_API = "SetBlocking";
constexpr const char *MOCK_STARTDHCPCLIENT_API = "StartDhcpClient";
constexpr const char *MOCK_STOPDHCPCLIENT_API = "StopDhcpClient";
constexpr const char *MOCK_REGISTERNOTIFYCALLBACK_API = "RegisterNotifyCallback";
constexpr const char *MOCK_STARTDHCPSERVICE_API = "StartDhcpService";
constexpr const char *MOCK_STOPDHCPSERVICE_API = "StopDhcpService";

class MockNetsysNativeClient {
public:
    MockNetsysNativeClient();
    ~MockNetsysNativeClient();
    void Init();

    void RegisterMockApi();
    bool CheckMockApi(const std::string &api);

    /**
     * @brief Create a physical network
     *
     * @param netId
     * @param permission Permission to create a physical network
     * @return Return the return value of the netsys interface call
     */
    int32_t NetworkCreatePhysical(int32_t netId, int32_t permission);

    /**
     * @brief Destroy the network
     *
     * @param netId
     * @return Return the return value of the netsys interface call
     */
    int32_t NetworkDestroy(int32_t netId);

    /**
     * @brief Add network port device
     *
     * @param netId
     * @param iface Network port device name
     * @return Return the return value of the netsys interface call
     */
    int32_t NetworkAddInterface(int32_t netId, const std::string &iface);

    /**
     * @brief Delete network port device
     *
     * @param netId
     * @param iface Network port device name
     * @return Return the return value of the netsys interface call
     */
    int32_t NetworkRemoveInterface(int32_t netId, const std::string &iface);

    /**
     * @brief Add route
     *
     * @param netId
     * @param ifName Network port device name
     * @param destination Target host ip
     * @param nextHop Next hop address
     * @return Return the return value of the netsys interface call
     */
    int32_t NetworkAddRoute(int32_t netId, const std::string &ifName, const std::string &destination,
        const std::string &nextHop);

    /**
     * @brief Remove route
     *
     * @param netId
     * @param ifName Network port device name
     * @param destination Target host ip
     * @param nextHop Next hop address
     * @return Return the return value of the netsys interface call
     */
    int32_t NetworkRemoveRoute(int32_t netId, const std::string &ifName, const std::string &destination,
        const std::string &nextHop);

    /**
     * @brief Turn off the device
     *
     * @param iface Network port device name
     * @return Return the result of this action
     */
    int32_t SetInterfaceDown(const std::string &iface);

    /**
     * @brief Turn on the device
     *
     * @param iface Network port device name
     * @return Return the result of this action
     */
    int32_t SetInterfaceUp(const std::string &iface);

    /**
     * @brief Clear the network interface ip address
     *
     * @param ifName Network port device name
     */
    void ClearInterfaceAddrs(const std::string &ifName);

    /**
     * @brief Obtain mtu from the network interface device
     *
     * @param ifName Network port device name
     * @return Return the return value of the netsys interface call
     */
    int32_t GetInterfaceMtu(const std::string &ifName);

    /**
     * @brief Set mtu to network interface device
     *
     * @param ifName Network port device name
     * @param mtu
     * @return Return the return value of the netsys interface call
     */
    int32_t SetInterfaceMtu(const std::string &ifName, int32_t mtu);

    /**
     * @brief Add ip address
     *
     * @param ifName Network port device name
     * @param ipAddr    ip address
     * @param prefixLength  subnet mask
     * @return Return the return value of the netsys interface call
     */
    int32_t AddInterfaceAddress(const std::string &ifName, const std::string &ipAddr, int32_t prefixLength);

    /**
     * @brief Delete ip address
     *
     * @param ifName Network port device name
     * @param ipAddr ip address
     * @param prefixLength subnet mask
     * @return Return the return value of the netsys interface call
     */
    int32_t DelInterfaceAddress(const std::string &ifName, const std::string &ipAddr, int32_t prefixLength);

    /**
     * @brief Set dns
     *
     * @param netId
     * @param baseTimeoutMsec
     * @param retryCount
     * @param servers
     * @param domains
     * @return Return the return value of the netsys interface call
     */
    int32_t SetResolverConfig(uint16_t netId, uint16_t baseTimeoutMsec, uint8_t retryCount,
        const std::vector<std::string> &servers, const std::vector<std::string> &domains);
    /**
     * @brief Get dns server param info
     *
     * @param netId
     * @param servers
     * @param domains
     * @param baseTimeoutMsec
     * @param retryCount
     * @return Return the return value of the netsys interface call
     */
    int32_t GetResolverConfig(uint16_t netId, std::vector<std::string> &servers,
        std::vector<std::string> &domains, uint16_t &baseTimeoutMsec, uint8_t &retryCount);

    /**
     * @brief Create dns cache before set dns
     *
     * @param netId
     * @return Return the return value for status of call
     */
    int32_t CreateNetworkCache(uint16_t netId);

    /**
     * @brief Destroy dns cache
     *
     * @param netId
     * @return Return the return value of the netsys interface call
     */
    int32_t DestroyNetworkCache(uint16_t netId);

    /**
     * @brief Obtains the bytes received over the cellular network.
     *
     * @return The number of received bytes.
     */
    int64_t GetCellularRxBytes();

    /**
     * @brief Obtains the bytes sent over the cellular network.
     *
     * @return The number of sent bytes.
     */
    int64_t GetCellularTxBytes();

    /**
     * @brief Obtains the bytes received through all NICs.
     *
     * @return The number of received bytes.
     */
    int64_t GetAllRxBytes();

    /**
     * @brief Obtains the bytes sent through all NICs.
     *
     * @return The number of sent bytes.
     */
    int64_t GetAllTxBytes();

    /**
     * @brief Obtains the bytes received through a specified UID.
     *
     * @param uid app id.
     * @return The number of received bytes.
     */
    int64_t GetUidRxBytes(uint32_t uid);

    /**
     * @brief Obtains the bytes sent through a specified UID.
     *
     * @param uid app id.
     * @return The number of sent bytes.
     */
    int64_t GetUidTxBytes(uint32_t uid);

    /**
     * @brief Obtains the bytes received through a specified UID on Iface.
     *
     * @param uid app id.
     * @param iface The name of the interface.
     * @return The number of received bytes.
     */
    int64_t GetUidOnIfaceRxBytes(uint32_t uid, const std::string &interfaceName);

    /**
     * @brief Obtains the bytes sent through a specified UID on Iface.
     *
     * @param uid app id.
     * @param iface The name of the interface.
     * @return The number of sent bytes.
     */
    int64_t GetUidOnIfaceTxBytes(uint32_t uid, const std::string &interfaceName);

    /**
     * @brief Obtains the bytes received through a specified NIC.
     *
     * @param iface The name of the interface.
     * @return The number of received bytes.
     */
    int64_t GetIfaceRxBytes(const std::string &interfaceName);

    /**
     * @brief Obtains the bytes sent through a specified NIC.
     *
     * @param iface The name of the interface.
     * @return The number of sent bytes.
     */
    int64_t GetIfaceTxBytes(const std::string &interfaceName);

    /**
     * @brief Obtains the NIC list.
     *
     * @return The list of interface.
     */
    std::vector<std::string> InterfaceGetList();

    /**
     * @brief Obtains the uid list.
     *
     * @return The list of uid.
     */
    std::vector<std::string> UidGetList();

    /**
     * @brief Obtains the packets received through a specified NIC.
     *
     * @param iface The name of the interface.
     * @return The number of received packets.
     */
    int64_t GetIfaceRxPackets(const std::string &interfaceName);

    /**
     * @brief Obtains the packets sent through a specified NIC.
     *
     * @param iface The name of the interface.
     * @return The number of sent packets.
     */
    int64_t GetIfaceTxPackets(const std::string &interfaceName);

    /**
     * @brief  set default network.
     *
     * @return Return the return value of the netsys interface call
     */
    int32_t  SetDefaultNetWork(int32_t   netId);

    /**
     * @brief clear default network netId.
     *
     * @return Return the return value of the netsys interface call
     */
    int32_t  ClearDefaultNetWorkNetId();

    /**
     * @brief Obtains the NIC list.
     *
     * @param socket_fd
     * @param netId
     * @return Return the return value of the netsys interface call
     */
    int32_t BindSocket(int32_t socket_fd, uint32_t netId);

    /**
     * Set tether dns.
     *
     * @param netId network id
     * @param dnsAddr the list of dns address
     * @return Return the return value of the netsys interface call.
     */
    int32_t ShareDnsSet(uint16_t netId);

    /**
     * @brief Set net callbackfuction.
     *
     * @param callback callbackfuction class
     * @return Return the return value of the netsys interface call.
     */
    int32_t RegisterNetsysNotifyCallback(const NetsysNotifyCallback &callback);

    /**
     * @brief protect tradition network to connect VPN.
     *
     * @param socketFd socket file description
     * @return Return the return value of the netsys interface call.
     */
    int32_t BindNetworkServiceVpn(int32_t socketFd);

    /**
     * @brief enable virtual network iterface card.
     *
     * @param socketFd socket file description
     * @param ifRequest interface request
     * @return Return the return value of the netsys interface call.
     */
    int32_t EnableVirtualNetIfaceCard(int32_t socketFd, struct ifreq &ifRequest, int32_t &ifaceFd);

    /**
     * @brief Set ip address.
     *
     * @param socketFd socket file description
     * @param ipAddress ip address
     * @param prefixLen the mask of ip address
     * @param ifRequest interface request
     * @return Return the return value of the netsys interface call.
     */
    int32_t SetIpAddress(int32_t socketFd, const std::string &ipAddress, int32_t prefixLen, struct ifreq &ifRequest);

    /**
     * @brief Set network blocking.
     *
     * @param ifaceFd interface file description
     * @param isBlock network blocking
     * @return Return the return value of the netsys interface call.
     */
    int32_t SetBlocking(int32_t ifaceFd, bool isBlock);
    /**
    * @brief Start Dhcp Client.
    *
    * @param iface interface file description
    * @param bIpv6 network blocking
    * @return.
    */
    int32_t StartDhcpClient(const std::string &iface, bool bIpv6);
    /**
     * @brief Stop Dhcp Client.
     *
     * @param iface interface file description
     * @param bIpv6 network blocking
     * @return .
     */
    int32_t StopDhcpClient(const std::string &iface, bool bIpv6);
    /**
    * @brief Register Notify Callback
    *
    * @param callback
    * @return .
    */
    int32_t RegisterCallback(sptr<NetsysControllerCallback> callback);
    /**
     * @brief start dhcpservice.
     *
     * @param iface interface name
     * @param ipv4addr ipv4 addr
     * @return Return the return value of the netsys interface call.
     */
    int32_t StartDhcpService(const std::string &iface, const std::string &ipv4addr);

    /**
     * @brief stop dhcpservice.
     *
     * @param iface interface name
     * @return Return the return value of the netsys interface call.
     */
    int32_t StopDhcpService(const std::string &iface);
private:
    int64_t GetIfaceBytes(const std::string &interfaceName, const std::string &filename);
    int64_t GetAllBytes(const std::string &filename);
    int32_t AddRoute(const std::string &ip, const std::string &mask, const std::string &gateWay,
        const std::string &devName);

private:
    std::unordered_set<std::string> mockApi_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // MOCK_NETSYS_NATIVE_CLIENT_H
