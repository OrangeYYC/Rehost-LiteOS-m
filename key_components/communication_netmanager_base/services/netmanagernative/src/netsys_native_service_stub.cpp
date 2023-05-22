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

#include <cstdlib>
#include <net/route.h>
#include <netdb.h>
#include <unistd.h>

#include "ipc_skeleton.h"
#include "netmanager_base_common_utils.h"
#include "netnative_log_wrapper.h"
#include "securec.h"

#include "netsys_native_service_stub.h"

using namespace OHOS::NetManagerStandard::CommonUtils;
namespace OHOS {
namespace NetsysNative {
static constexpr int32_t MAX_FLAG_NUM = 64;
static constexpr int32_t MAX_DNS_CONFIG_SIZE = 4;
static constexpr int32_t NETMANAGER_ERR_PERMISSION_DENIED = 201;
static constexpr uint32_t UIDS_LIST_MAX_SIZE = 1024;

NetsysNativeServiceStub::NetsysNativeServiceStub()
{
    opToInterfaceMap_[NETSYS_SET_RESOLVER_CONFIG] = &NetsysNativeServiceStub::CmdSetResolverConfig;
    opToInterfaceMap_[NETSYS_GET_RESOLVER_CONFIG] = &NetsysNativeServiceStub::CmdGetResolverConfig;
    opToInterfaceMap_[NETSYS_CREATE_NETWORK_CACHE] = &NetsysNativeServiceStub::CmdCreateNetworkCache;
    opToInterfaceMap_[NETSYS_DESTROY_NETWORK_CACHE] = &NetsysNativeServiceStub::CmdDestroyNetworkCache;
    opToInterfaceMap_[NETSYS_GET_ADDR_INFO] = &NetsysNativeServiceStub::CmdGetAddrInfo;
    opToInterfaceMap_[NETSYS_INTERFACE_SET_MTU] = &NetsysNativeServiceStub::CmdSetInterfaceMtu;
    opToInterfaceMap_[NETSYS_INTERFACE_GET_MTU] = &NetsysNativeServiceStub::CmdGetInterfaceMtu;
    opToInterfaceMap_[NETSYS_REGISTER_NOTIFY_CALLBACK] = &NetsysNativeServiceStub::CmdRegisterNotifyCallback;
    opToInterfaceMap_[NETSYS_UNREGISTER_NOTIFY_CALLBACK] = &NetsysNativeServiceStub::CmdUnRegisterNotifyCallback;
    opToInterfaceMap_[NETSYS_NETWORK_ADD_ROUTE] = &NetsysNativeServiceStub::CmdNetworkAddRoute;
    opToInterfaceMap_[NETSYS_NETWORK_REMOVE_ROUTE] = &NetsysNativeServiceStub::CmdNetworkRemoveRoute;
    opToInterfaceMap_[NETSYS_NETWORK_ADD_ROUTE_PARCEL] = &NetsysNativeServiceStub::CmdNetworkAddRouteParcel;
    opToInterfaceMap_[NETSYS_NETWORK_REMOVE_ROUTE_PARCEL] = &NetsysNativeServiceStub::CmdNetworkRemoveRouteParcel;
    opToInterfaceMap_[NETSYS_NETWORK_SET_DEFAULT] = &NetsysNativeServiceStub::CmdNetworkSetDefault;
    opToInterfaceMap_[NETSYS_NETWORK_GET_DEFAULT] = &NetsysNativeServiceStub::CmdNetworkGetDefault;
    opToInterfaceMap_[NETSYS_NETWORK_CLEAR_DEFAULT] = &NetsysNativeServiceStub::CmdNetworkClearDefault;
    opToInterfaceMap_[NETSYS_GET_PROC_SYS_NET] = &NetsysNativeServiceStub::CmdGetProcSysNet;
    opToInterfaceMap_[NETSYS_SET_PROC_SYS_NET] = &NetsysNativeServiceStub::CmdSetProcSysNet;
    opToInterfaceMap_[NETSYS_NETWORK_CREATE_PHYSICAL] = &NetsysNativeServiceStub::CmdNetworkCreatePhysical;
    opToInterfaceMap_[NETSYS_INTERFACE_ADD_ADDRESS] = &NetsysNativeServiceStub::CmdAddInterfaceAddress;
    opToInterfaceMap_[NETSYS_INTERFACE_DEL_ADDRESS] = &NetsysNativeServiceStub::CmdDelInterfaceAddress;
    opToInterfaceMap_[NETSYS_INTERFACE_SET_IP_ADDRESS] = &NetsysNativeServiceStub::CmdInterfaceSetIpAddress;
    opToInterfaceMap_[NETSYS_INTERFACE_SET_IFF_UP] = &NetsysNativeServiceStub::CmdInterfaceSetIffUp;
    opToInterfaceMap_[NETSYS_NETWORK_ADD_INTERFACE] = &NetsysNativeServiceStub::CmdNetworkAddInterface;
    opToInterfaceMap_[NETSYS_NETWORK_REMOVE_INTERFACE] = &NetsysNativeServiceStub::CmdNetworkRemoveInterface;
    opToInterfaceMap_[NETSYS_NETWORK_DESTROY] = &NetsysNativeServiceStub::CmdNetworkDestroy;
    opToInterfaceMap_[NETSYS_GET_FWMARK_FOR_NETWORK] = &NetsysNativeServiceStub::CmdGetFwmarkForNetwork;
    opToInterfaceMap_[NETSYS_INTERFACE_SET_CONFIG] = &NetsysNativeServiceStub::CmdSetInterfaceConfig;
    opToInterfaceMap_[NETSYS_INTERFACE_GET_CONFIG] = &NetsysNativeServiceStub::CmdGetInterfaceConfig;
    opToInterfaceMap_[NETSYS_INTERFACE_GET_LIST] = &NetsysNativeServiceStub::CmdInterfaceGetList;
    opToInterfaceMap_[NETSYS_START_DHCP_CLIENT] = &NetsysNativeServiceStub::CmdStartDhcpClient;
    opToInterfaceMap_[NETSYS_STOP_DHCP_CLIENT] = &NetsysNativeServiceStub::CmdStopDhcpClient;
    opToInterfaceMap_[NETSYS_START_DHCP_SERVICE] = &NetsysNativeServiceStub::CmdStartDhcpService;
    opToInterfaceMap_[NETSYS_STOP_DHCP_SERVICE] = &NetsysNativeServiceStub::CmdStopDhcpService;
    opToInterfaceMap_[NETSYS_IPENABLE_FORWARDING] = &NetsysNativeServiceStub::CmdIpEnableForwarding;
    opToInterfaceMap_[NETSYS_IPDISABLE_FORWARDING] = &NetsysNativeServiceStub::CmdIpDisableForwarding;
    opToInterfaceMap_[NETSYS_ENABLE_NAT] = &NetsysNativeServiceStub::CmdEnableNat;
    opToInterfaceMap_[NETSYS_DISABLE_NAT] = &NetsysNativeServiceStub::CmdDisableNat;
    opToInterfaceMap_[NETSYS_IPFWD_ADD_INTERFACE_FORWARD] = &NetsysNativeServiceStub::CmdIpfwdAddInterfaceForward;
    opToInterfaceMap_[NETSYS_IPFWD_REMOVE_INTERFACE_FORWARD] = &NetsysNativeServiceStub::CmdIpfwdRemoveInterfaceForward;
    opToInterfaceMap_[NETSYS_TETHER_DNS_SET] = &NetsysNativeServiceStub::CmdShareDnsSet;
    opToInterfaceMap_[NETSYS_START_DNS_PROXY_LISTEN] = &NetsysNativeServiceStub::CmdStartDnsProxyListen;
    opToInterfaceMap_[NETSYS_STOP_DNS_PROXY_LISTEN] = &NetsysNativeServiceStub::CmdStopDnsProxyListen;
    opToInterfaceMap_[NETSYS_GET_SHARING_NETWORK_TRAFFIC] = &NetsysNativeServiceStub::CmdGetNetworkSharingTraffic;
    InitBandwidthOpToInterfaceMap();
    InitFirewallOpToInterfaceMap();
    uids_ = {UID_ROOT, UID_SHELL, UID_NET_MANAGER, UID_WIFI, UID_RADIO, UID_HIDUMPER_SERVICE,
             UID_SAMGR, UID_PARAM_WATCHER};
}

void NetsysNativeServiceStub::InitBandwidthOpToInterfaceMap()
{
    opToInterfaceMap_[NETSYS_BANDWIDTH_ENABLE_DATA_SAVER] = &NetsysNativeServiceStub::CmdBandwidthEnableDataSaver;
    opToInterfaceMap_[NETSYS_BANDWIDTH_SET_IFACE_QUOTA] = &NetsysNativeServiceStub::CmdBandwidthSetIfaceQuota;
    opToInterfaceMap_[NETSYS_BANDWIDTH_REMOVE_IFACE_QUOTA] = &NetsysNativeServiceStub::CmdBandwidthRemoveIfaceQuota;
    opToInterfaceMap_[NETSYS_BANDWIDTH_ADD_DENIED_LIST] = &NetsysNativeServiceStub::CmdBandwidthAddDeniedList;
    opToInterfaceMap_[NETSYS_BANDWIDTH_REMOVE_DENIED_LIST] = &NetsysNativeServiceStub::CmdBandwidthRemoveDeniedList;
    opToInterfaceMap_[NETSYS_BANDWIDTH_ADD_ALLOWED_LIST] = &NetsysNativeServiceStub::CmdBandwidthAddAllowedList;
    opToInterfaceMap_[NETSYS_BANDWIDTH_REMOVE_ALLOWED_LIST] = &NetsysNativeServiceStub::CmdBandwidthRemoveAllowedList;
}

void NetsysNativeServiceStub::InitFirewallOpToInterfaceMap()
{
    opToInterfaceMap_[NETSYS_FIREWALL_SET_UID_ALLOWED_LIST_CHAIN] =
        &NetsysNativeServiceStub::CmdFirewallSetUidsAllowedListChain;
    opToInterfaceMap_[NETSYS_FIREWALL_SET_UID_DENIED_LIST_CHAIN] =
        &NetsysNativeServiceStub::CmdFirewallSetUidsDeniedListChain;
    opToInterfaceMap_[NETSYS_FIREWALL_ENABLE_CHAIN] = &NetsysNativeServiceStub::CmdFirewallEnableChain;
    opToInterfaceMap_[NETSYS_FIREWALL_SET_UID_RULE] = &NetsysNativeServiceStub::CmdFirewallSetUidRule;
}

int32_t NetsysNativeServiceStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                                 MessageOption &option)
{
    NETNATIVE_LOG_D("Begin to call procedure with code %{public}u", code);
    auto interfaceIndex = opToInterfaceMap_.find(code);
    if (interfaceIndex == opToInterfaceMap_.end() || !interfaceIndex->second) {
        NETNATIVE_LOGE("Cannot response request %d: unknown tranction", code);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    auto uid = IPCSkeleton::GetCallingUid();
    if (std::find(uids_.begin(), uids_.end(), uid) == uids_.end()) {
        NETNATIVE_LOGE("This uid connot use netsys");
        if (!reply.WriteInt32(NETMANAGER_ERR_PERMISSION_DENIED)) {
            return IPC_STUB_WRITE_PARCEL_ERR;
        }
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }

    const std::u16string descriptor = NetsysNativeServiceStub::GetDescriptor();
    const std::u16string remoteDescriptor = data.ReadInterfaceToken();
    if (descriptor != remoteDescriptor) {
        NETNATIVE_LOGE("Check remote descriptor failed");
        return IPC_STUB_INVALID_DATA_ERR;
    }
    return (this->*(interfaceIndex->second))(data, reply);
}

int32_t NetsysNativeServiceStub::CmdSetResolverConfig(MessageParcel &data, MessageParcel &reply)
{
    uint16_t netId = 0;
    uint16_t baseTimeoutMsec = 0;
    uint8_t retryCount = 0;
    std::vector<std::string> servers;
    std::vector<std::string> domains;
    if (!data.ReadUint16(netId)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.ReadUint16(baseTimeoutMsec)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.ReadUint8(retryCount)) {
        return ERR_FLATTEN_OBJECT;
    }
    int32_t vServerSize;
    if (!data.ReadInt32(vServerSize)) {
        return ERR_FLATTEN_OBJECT;
    }
    vServerSize = (vServerSize > MAX_DNS_CONFIG_SIZE) ? MAX_DNS_CONFIG_SIZE : vServerSize;
    std::string s;
    for (int32_t i = 0; i < vServerSize; ++i) {
        std::string().swap(s);
        if (!data.ReadString(s)) {
            return ERR_FLATTEN_OBJECT;
        }
        servers.push_back(s);
    }
    int32_t vDomainSize;
    if (!data.ReadInt32(vDomainSize)) {
        return ERR_FLATTEN_OBJECT;
    }
    vDomainSize = (vDomainSize > MAX_DNS_CONFIG_SIZE) ? MAX_DNS_CONFIG_SIZE : vDomainSize;
    for (int32_t i = 0; i < vDomainSize; ++i) {
        std::string().swap(s);
        if (!data.ReadString(s)) {
            return ERR_FLATTEN_OBJECT;
        }
        domains.push_back(s);
    }

    int32_t result = SetResolverConfig(netId, baseTimeoutMsec, retryCount, servers, domains);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("SetResolverConfig has received result %{public}d", result);

    return ERR_NONE;
}

int32_t NetsysNativeServiceStub::CmdGetResolverConfig(MessageParcel &data, MessageParcel &reply)
{
    uint16_t baseTimeoutMsec;
    uint8_t retryCount;
    uint16_t netId = 0;
    std::vector<std::string> servers;
    std::vector<std::string> domains;

    data.ReadUint16(netId);
    int32_t result = GetResolverConfig(netId, servers, domains, baseTimeoutMsec, retryCount);
    reply.WriteInt32(result);
    reply.WriteUint16(baseTimeoutMsec);
    reply.WriteUint8(retryCount);
    auto vServerSize = static_cast<int32_t>(servers.size());
    vServerSize = (vServerSize > MAX_DNS_CONFIG_SIZE) ? MAX_DNS_CONFIG_SIZE : vServerSize;
    reply.WriteInt32(vServerSize);
    int32_t index = 0;
    for (auto &server : servers) {
        if (++index > MAX_DNS_CONFIG_SIZE) {
            break;
        }
        reply.WriteString(server);
    }
    auto vDomainsSize = static_cast<int32_t>(domains.size());
    vDomainsSize = (vDomainsSize > MAX_DNS_CONFIG_SIZE) ? MAX_DNS_CONFIG_SIZE : vDomainsSize;
    reply.WriteInt32(vDomainsSize);
    std::vector<std::string>::iterator iterDomains;
    index = 0;
    for (iterDomains = domains.begin(); iterDomains != domains.end(); ++iterDomains) {
        if (++index > MAX_DNS_CONFIG_SIZE) {
            break;
        }
        reply.WriteString(*iterDomains);
    }
    NETNATIVE_LOG_D("GetResolverConfig has recved result %{public}d", result);
    return ERR_NONE;
}

int32_t NetsysNativeServiceStub::CmdCreateNetworkCache(MessageParcel &data, MessageParcel &reply)
{
    uint16_t netid = data.ReadUint16();
    NETNATIVE_LOGI("CreateNetworkCache  netid %{public}d", netid);
    int32_t result = CreateNetworkCache(netid);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("CreateNetworkCache has recved result %{public}d", result);

    return ERR_NONE;
}

int32_t NetsysNativeServiceStub::CmdDestroyNetworkCache(MessageParcel &data, MessageParcel &reply)
{
    uint16_t netId = data.ReadUint16();
    int32_t result = DestroyNetworkCache(netId);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("DestroyNetworkCache has recved result %{public}d", result);

    return ERR_NONE;
}

int32_t NetsysNativeServiceStub::NetsysFreeAddrinfo(struct addrinfo *aihead)
{
    struct addrinfo *ai, *ainext;
    for (ai = aihead; ai != nullptr; ai = ainext) {
        if (ai->ai_addr != nullptr)
            free(ai->ai_addr);
        if (ai->ai_canonname != nullptr)
            free(ai->ai_canonname);
        ainext = ai->ai_next;
        free(ai);
    }
    return ERR_NONE;
}

int32_t NetsysNativeServiceStub::CmdGetAddrInfo(MessageParcel &data, MessageParcel &reply)
{
    std::string hostName;
    std::string serverName;
    AddrInfo hints = {};
    uint16_t netId;
    if (!data.ReadString(hostName)) {
        return IPC_STUB_INVALID_DATA_ERR;
    }

    if (!data.ReadString(serverName)) {
        return IPC_STUB_INVALID_DATA_ERR;
    }

    auto p = data.ReadRawData(sizeof(AddrInfo));
    if (p == nullptr) {
        return IPC_STUB_INVALID_DATA_ERR;
    }
    if (memcpy_s(&hints, sizeof(AddrInfo), p, sizeof(AddrInfo)) != EOK) {
        return IPC_STUB_INVALID_DATA_ERR;
    }

    if (!data.ReadUint16(netId)) {
        return IPC_STUB_INVALID_DATA_ERR;
    }

    std::vector<AddrInfo> retInfo;
    auto ret = GetAddrInfo(hostName, serverName, hints, netId, retInfo);
    if (retInfo.size() > MAX_RESULTS) {
        return IPC_STUB_INVALID_DATA_ERR;
    }

    if (!reply.WriteInt32(ret)) {
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    if (ret != ERR_NONE) {
        return ERR_NONE;
    }

    if (!reply.WriteUint32(static_cast<uint32_t>(retInfo.size()))) {
        return IPC_STUB_WRITE_PARCEL_ERR;
    }

    for (const auto &info : retInfo) {
        if (!reply.WriteRawData(&info, sizeof(AddrInfo))) {
            return IPC_STUB_WRITE_PARCEL_ERR;
        }
    }
    return ERR_NONE;
}

int32_t NetsysNativeServiceStub::CmdSetInterfaceMtu(MessageParcel &data, MessageParcel &reply)
{
    std::string ifName = data.ReadString();
    int32_t mtu = data.ReadInt32();
    int32_t result = SetInterfaceMtu(ifName, mtu);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("SetInterfaceMtu has recved result %{public}d", result);

    return ERR_NONE;
}

int32_t NetsysNativeServiceStub::CmdGetInterfaceMtu(MessageParcel &data, MessageParcel &reply)
{
    std::string ifName = data.ReadString();
    int32_t result = GetInterfaceMtu(ifName);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("GetInterfaceMtu has recved result %{public}d", result);

    return ERR_NONE;
}

int32_t NetsysNativeServiceStub::CmdRegisterNotifyCallback(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd RegisterNotifyCallback");
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETNATIVE_LOGE("Callback ptr is nullptr.");
        return -1;
    }

    sptr<INotifyCallback> callback = iface_cast<INotifyCallback>(remote);
    int32_t result = RegisterNotifyCallback(callback);
    reply.WriteInt32(result);
    return ERR_NONE;
}

int32_t NetsysNativeServiceStub::CmdUnRegisterNotifyCallback(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd UnRegisterNotifyCallback");
    sptr<IRemoteObject> remote = data.ReadRemoteObject();
    if (remote == nullptr) {
        NETNATIVE_LOGE("Callback ptr is nullptr.");
        return -1;
    }

    sptr<INotifyCallback> callback = iface_cast<INotifyCallback>(remote);
    int32_t result = UnRegisterNotifyCallback(callback);
    reply.WriteInt32(result);
    return ERR_NONE;
}

int32_t NetsysNativeServiceStub::CmdNetworkAddRoute(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = data.ReadInt32();
    std::string ifName = data.ReadString();
    std::string destination = data.ReadString();
    std::string nextHop = data.ReadString();

    NETNATIVE_LOGI("netId[%{public}d}, ifName[%{public}s], destination[%{public}s}, nextHop[%{public}s]", netId,
                   ifName.c_str(), ToAnonymousIp(destination).c_str(), ToAnonymousIp(nextHop).c_str());
    int32_t result = NetworkAddRoute(netId, ifName, destination, nextHop);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("NetworkAddRoute has recved result %{public}d", result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdNetworkRemoveRoute(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = data.ReadInt32();
    std::string interfaceName = data.ReadString();
    std::string destination = data.ReadString();
    std::string nextHop = data.ReadString();

    NETNATIVE_LOGI("netId[%{public}d}, ifName[%{public}s], destination[%{public}s}, nextHop[%{public}s]", netId,
                   interfaceName.c_str(), ToAnonymousIp(destination).c_str(), ToAnonymousIp(nextHop).c_str());
    int32_t result = NetworkRemoveRoute(netId, interfaceName, destination, nextHop);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("NetworkRemoveRoute has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdNetworkAddRouteParcel(MessageParcel &data, MessageParcel &reply)
{
    RouteInfoParcel routeInfo = {};
    int32_t netId = data.ReadInt32();
    routeInfo.ifName = data.ReadString();
    routeInfo.destination = data.ReadString();
    routeInfo.nextHop = data.ReadString();
    int32_t result = NetworkAddRouteParcel(netId, routeInfo);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("NetworkAddRouteParcel has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdNetworkRemoveRouteParcel(MessageParcel &data, MessageParcel &reply)
{
    RouteInfoParcel routeInfo = {};
    int32_t netId = data.ReadInt32();
    routeInfo.ifName = data.ReadString();
    routeInfo.destination = data.ReadString();
    routeInfo.nextHop = data.ReadString();

    int32_t result = NetworkRemoveRouteParcel(netId, routeInfo);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("NetworkRemoveRouteParcel has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdNetworkSetDefault(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = data.ReadInt32();

    int32_t result = NetworkSetDefault(netId);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("NetworkSetDefault has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdNetworkGetDefault(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = NetworkGetDefault();
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("NetworkGetDefault has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdNetworkClearDefault(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = NetworkClearDefault();
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("NetworkClearDefault has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdGetProcSysNet(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd GetProcSysNet");
    int32_t family = data.ReadInt32();
    int32_t which = data.ReadInt32();
    std::string ifname = data.ReadString();
    std::string parameter = data.ReadString();
    std::string value;
    int32_t result = GetProcSysNet(family, which, ifname, parameter, value);
    reply.WriteInt32(result);
    std::string valueRsl = value;
    reply.WriteString(valueRsl);
    return result;
}

int32_t NetsysNativeServiceStub::CmdSetProcSysNet(MessageParcel &data, MessageParcel &reply)
{
    int32_t family = data.ReadInt32();
    int32_t which = data.ReadInt32();
    std::string ifname = data.ReadString();
    std::string parameter = data.ReadString();
    std::string value = data.ReadString();
    int32_t result = SetProcSysNet(family, which, ifname, parameter, value);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("SetProcSysNet has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdNetworkCreatePhysical(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = data.ReadInt32();
    int32_t permission = data.ReadInt32();

    int32_t result = NetworkCreatePhysical(netId, permission);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("NetworkCreatePhysical has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdAddInterfaceAddress(MessageParcel &data, MessageParcel &reply)
{
    std::string interfaceName = data.ReadString();
    std::string ipAddr = data.ReadString();
    int32_t prefixLength = data.ReadInt32();

    int32_t result = AddInterfaceAddress(interfaceName, ipAddr, prefixLength);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("AddInterfaceAddress has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdDelInterfaceAddress(MessageParcel &data, MessageParcel &reply)
{
    std::string interfaceName = data.ReadString();
    std::string ipAddr = data.ReadString();
    int32_t prefixLength = data.ReadInt32();

    int32_t result = DelInterfaceAddress(interfaceName, ipAddr, prefixLength);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("DelInterfaceAddress has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdInterfaceSetIpAddress(MessageParcel &data, MessageParcel &reply)
{
    std::string ifaceName = data.ReadString();
    std::string ipAddress = data.ReadString();

    int32_t result = InterfaceSetIpAddress(ifaceName, ipAddress);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("InterfaceSetIpAddress has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdInterfaceSetIffUp(MessageParcel &data, MessageParcel &reply)
{
    std::string ifaceName = data.ReadString();

    int32_t result = InterfaceSetIffUp(ifaceName);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("InterfaceSetIffUp has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdNetworkAddInterface(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = data.ReadInt32();
    std::string iface = data.ReadString();

    int32_t result = NetworkAddInterface(netId, iface);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("NetworkAddInterface has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdNetworkRemoveInterface(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = data.ReadInt32();
    std::string iface = data.ReadString();
    int32_t result = NetworkRemoveInterface(netId, iface);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("NetworkRemoveRouteParcel has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdNetworkDestroy(MessageParcel &data, MessageParcel &reply)
{
    int32_t netId = data.ReadInt32();
    int32_t result = NetworkDestroy(netId);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("NetworkDestroy has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdGetFwmarkForNetwork(MessageParcel &data, MessageParcel &reply)
{
    MarkMaskParcel markMaskParcel = {};
    int32_t netId = data.ReadInt32();
    markMaskParcel.mark = data.ReadInt32();
    markMaskParcel.mask = data.ReadInt32();
    int32_t result = GetFwmarkForNetwork(netId, markMaskParcel);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("GetFwmarkForNetwork has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdSetInterfaceConfig(MessageParcel &data, MessageParcel &reply)
{
    InterfaceConfigurationParcel cfg = {};
    cfg.ifName = data.ReadString();
    cfg.hwAddr = data.ReadString();
    cfg.ipv4Addr = data.ReadString();
    cfg.prefixLength = data.ReadInt32();
    int32_t vSize = data.ReadInt32();
    vSize = (vSize > MAX_FLAG_NUM) ? MAX_FLAG_NUM : vSize;
    std::vector<std::string> vFlags;
    for (int i = 0; i < vSize; i++) {
        vFlags.emplace_back(data.ReadString());
    }
    cfg.flags.assign(vFlags.begin(), vFlags.end());
    int32_t result = SetInterfaceConfig(cfg);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("SetInterfaceConfig has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdGetInterfaceConfig(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd GetInterfaceConfig");
    InterfaceConfigurationParcel cfg = {};
    cfg.ifName = data.ReadString();
    int32_t result = GetInterfaceConfig(cfg);
    reply.WriteInt32(result);
    reply.WriteString(cfg.ifName);
    reply.WriteString(cfg.hwAddr);
    reply.WriteString(cfg.ipv4Addr);
    reply.WriteInt32(cfg.prefixLength);
    int32_t vsize = static_cast<int32_t>(cfg.flags.size());
    vsize = vsize > MAX_DNS_CONFIG_SIZE ? MAX_DNS_CONFIG_SIZE : vsize;
    reply.WriteInt32(vsize);
    std::vector<std::string>::iterator iter;
    int32_t index = 0;
    for (iter = cfg.flags.begin(); iter != cfg.flags.end(); ++iter) {
        if (++index > MAX_DNS_CONFIG_SIZE) {
            break;
        }
        reply.WriteString(*iter);
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdInterfaceGetList(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd InterfaceGetList");
    std::vector<std::string> ifaces;
    int32_t result = InterfaceGetList(ifaces);
    reply.WriteInt32(result);
    auto vsize = static_cast<int32_t>(ifaces.size());
    reply.WriteInt32(vsize);
    std::vector<std::string>::iterator iter;
    for (iter = ifaces.begin(); iter != ifaces.end(); ++iter) {
        reply.WriteString(*iter);
    }
    return result;
}

int32_t NetsysNativeServiceStub::CmdStartDhcpClient(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdStartDhcpClient");
    std::string iface = data.ReadString();
    bool bIpv6 = data.ReadBool();
    int32_t result = StartDhcpClient(iface, bIpv6);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdStopDhcpClient(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdStopDhcpClient");
    std::string iface = data.ReadString();
    bool bIpv6 = data.ReadBool();
    int32_t result = StopDhcpClient(iface, bIpv6);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdStartDhcpService(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdStartDhcpService");
    std::string iface = data.ReadString();
    std::string ipv4addr = data.ReadString();
    int32_t result = StartDhcpService(iface, ipv4addr);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdStopDhcpService(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdStopDhcpService");
    std::string iface = data.ReadString();
    int32_t result = StopDhcpService(iface);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdIpEnableForwarding(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdIpEnableForwarding");
    const auto &requester = data.ReadString();
    int32_t result = IpEnableForwarding(requester);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdIpDisableForwarding(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdIpDisableForwarding");
    const auto &requester = data.ReadString();
    int32_t result = IpDisableForwarding(requester);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdEnableNat(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdEnableNat");
    const auto &downstreamIface = data.ReadString();
    const auto &upstreamIface = data.ReadString();
    int32_t result = EnableNat(downstreamIface, upstreamIface);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdDisableNat(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdDisableNat");
    const auto &downstreamIface = data.ReadString();
    const auto &upstreamIface = data.ReadString();
    int32_t result = DisableNat(downstreamIface, upstreamIface);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdIpfwdAddInterfaceForward(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdIpfwdAddInterfaceForward");
    const auto &fromIface = data.ReadString();
    const auto &toIface = data.ReadString();
    int32_t result = IpfwdAddInterfaceForward(fromIface, toIface);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdIpfwdRemoveInterfaceForward(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdIpfwdRemoveInterfaceForward");
    const auto &fromIface = data.ReadString();
    const auto &toIface = data.ReadString();
    int32_t result = IpfwdRemoveInterfaceForward(fromIface, toIface);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdBandwidthEnableDataSaver(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdBandwidthEnableDataSaver");
    bool enable = data.ReadBool();
    int32_t result = BandwidthEnableDataSaver(enable);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdBandwidthSetIfaceQuota(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdBandwidthSetIfaceQuota");
    std::string ifName = data.ReadString();
    int64_t bytes = data.ReadInt64();
    int32_t result = BandwidthSetIfaceQuota(ifName, bytes);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdBandwidthRemoveIfaceQuota(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdBandwidthRemoveIfaceQuota");
    std::string ifName = data.ReadString();
    int32_t result = BandwidthRemoveIfaceQuota(ifName);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdBandwidthAddDeniedList(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdBandwidthAddDeniedList");
    uint32_t uid = data.ReadUint32();
    int32_t result = BandwidthAddDeniedList(uid);
    reply.WriteInt32(result);
    return result;
}
int32_t NetsysNativeServiceStub::CmdBandwidthRemoveDeniedList(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdBandwidthRemoveDeniedList");
    uint32_t uid = data.ReadUint32();
    int32_t result = BandwidthRemoveDeniedList(uid);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdBandwidthAddAllowedList(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdBandwidthAddAllowedList");
    uint32_t uid = data.ReadUint32();
    int32_t result = BandwidthAddAllowedList(uid);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdBandwidthRemoveAllowedList(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdBandwidthRemoveAllowedList");
    uint32_t uid = data.ReadUint32();
    int32_t result = BandwidthRemoveAllowedList(uid);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdFirewallSetUidsAllowedListChain(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdFirewallSetUidsAllowedListChain");
    uint32_t chain = data.ReadUint32();
    std::vector<uint32_t> uids;
    uint32_t uidSize = data.ReadUint32();
    uidSize = (uidSize > UIDS_LIST_MAX_SIZE) ? UIDS_LIST_MAX_SIZE : uidSize;
    for (uint32_t i = 0; i < uidSize; i++) {
        uint32_t uid = data.ReadUint32();
        uids.push_back(uid);
    }
    int32_t result = FirewallSetUidsAllowedListChain(chain, uids);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdFirewallSetUidsDeniedListChain(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdFirewallSetUidsDeniedListChain");
    uint32_t chain = data.ReadUint32();
    std::vector<uint32_t> uids;
    uint32_t uidSize = data.ReadUint32();
    uidSize = (uidSize > UIDS_LIST_MAX_SIZE) ? UIDS_LIST_MAX_SIZE : uidSize;
    for (uint32_t i = 0; i < uidSize; i++) {
        uint32_t uid = data.ReadUint32();
        uids.push_back(uid);
    }
    int32_t result = FirewallSetUidsDeniedListChain(chain, uids);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdFirewallEnableChain(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdFirewallEnableChain");
    uint32_t chain = data.ReadUint32();
    bool enable = data.ReadBool();
    int32_t result = FirewallEnableChain(chain, enable);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdFirewallSetUidRule(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd CmdFirewallSetUidRule");
    uint32_t chain = (unsigned)data.ReadUint32();
    uint32_t uid = (unsigned)data.ReadInt32();
    uint32_t firewallRule = (unsigned)data.ReadInt32();
    int32_t result = FirewallSetUidRule(chain, uid, firewallRule);
    reply.WriteInt32(result);
    return result;
}

int32_t NetsysNativeServiceStub::CmdShareDnsSet(MessageParcel &data, MessageParcel &reply)
{
    uint16_t netId = 0;
    data.ReadUint16(netId);
    int32_t result = ShareDnsSet(netId);
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("ShareDnsSet has received result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdStartDnsProxyListen(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = StartDnsProxyListen();
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("StartDnsProxyListen has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdStopDnsProxyListen(MessageParcel &data, MessageParcel &reply)
{
    int32_t result = StopDnsProxyListen();
    reply.WriteInt32(result);
    NETNATIVE_LOG_D("StopDnsProxyListen has recved result %{public}d", result);

    return result;
}

int32_t NetsysNativeServiceStub::CmdGetNetworkSharingTraffic(MessageParcel &data, MessageParcel &reply)
{
    NETNATIVE_LOG_D("Begin to dispatch cmd GetNetworkSharingTraffic");
    std::string downIface = data.ReadString();
    std::string upIface = data.ReadString();
    NetworkSharingTraffic traffic;
    int32_t result = GetNetworkSharingTraffic(downIface, upIface, traffic);
    reply.WriteInt32(result);
    reply.WriteInt64(traffic.receive);
    reply.WriteInt64(traffic.send);
    reply.WriteInt64(traffic.all);

    return result;
}
} // namespace NetsysNative
} // namespace OHOS
