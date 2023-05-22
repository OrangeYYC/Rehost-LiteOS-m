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

#include "netsys_native_service_proxy.h"

#include <securec.h>

#include "netnative_log_wrapper.h"
#include "netsys_addr_info_parcel.h"

namespace OHOS {
namespace NetsysNative {
static constexpr uint32_t UIDS_LIST_MAX_SIZE = 1024;
static constexpr int32_t MAX_DNS_CONFIG_SIZE = 4;
static constexpr int32_t MAX_INTERFACE_CONFIG_SIZE = 16;

namespace {
bool WriteNatDataToMessage(MessageParcel &data, const std::string &downstreamIface, const std::string &upstreamIface)
{
    if (!data.WriteInterfaceToken(NetsysNativeServiceProxy::GetDescriptor())) {
        NETNATIVE_LOGI("WriteInterfaceToken failed");
        return false;
    }
    if (!data.WriteString(downstreamIface)) {
        return false;
    }
    if (!data.WriteString(upstreamIface)) {
        return false;
    }
    return true;
}
} // namespace

bool NetsysNativeServiceProxy::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(NetsysNativeServiceProxy::GetDescriptor())) {
        NETNATIVE_LOGI("WriteInterfaceToken failed");
        return false;
    }
    return true;
}

int32_t NetsysNativeServiceProxy::SetResolverConfig(uint16_t netId, uint16_t baseTimeoutMsec, uint8_t retryCount,
                                                    const std::vector<std::string> &servers,
                                                    const std::vector<std::string> &domains)
{
    NETNATIVE_LOGI("Begin to SetResolverConfig %{public}d", retryCount);
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteUint16(netId)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteUint16(baseTimeoutMsec)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteUint8(retryCount)) {
        return ERR_FLATTEN_OBJECT;
    }

    auto vServerSize1 = static_cast<int32_t>(servers.size());
    if (!data.WriteInt32(vServerSize1)) {
        return ERR_FLATTEN_OBJECT;
    }
    std::vector<std::string> vServers;
    vServers.assign(servers.begin(), servers.end());
    NETNATIVE_LOGI("PROXY: SetResolverConfig Write Servers  String_SIZE: %{public}d",
                   static_cast<int32_t>(vServers.size()));
    for (auto &vServer : vServers) {
        data.WriteString(vServer);
    }

    int vDomainSize1 = static_cast<int>(domains.size());
    if (!data.WriteInt32(vDomainSize1)) {
        return ERR_FLATTEN_OBJECT;
    }

    std::vector<std::string> vDomains;
    vDomains.assign(domains.begin(), domains.end());
    NETNATIVE_LOGI("PROXY: SetResolverConfig Write Domains String_SIZE: %{public}d",
                   static_cast<int32_t>(vDomains.size()));
    for (auto &vDomain : vDomains) {
        data.WriteString(vDomain);
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_SET_RESOLVER_CONFIG, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::GetResolverConfig(uint16_t netId, std::vector<std::string> &servers,
                                                    std::vector<std::string> &domains, uint16_t &baseTimeoutMsec,
                                                    uint8_t &retryCount)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteUint16(netId)) {
        return ERR_FLATTEN_OBJECT;
    }
    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_GET_RESOLVER_CONFIG, data, reply, option);
    int result = reply.ReadInt32();
    if (result != ERR_NONE) {
        NETNATIVE_LOGE("Fail to GetResolverConfig ret= %{public}d", result);
        return result;
    }

    reply.ReadUint16(baseTimeoutMsec);
    reply.ReadUint8(retryCount);
    int32_t vServerSize = reply.ReadInt32();
    vServerSize = vServerSize > MAX_DNS_CONFIG_SIZE ? MAX_DNS_CONFIG_SIZE : vServerSize;
    std::vector<std::string> vecString;
    for (int i = 0; i < vServerSize; i++) {
        vecString.push_back(reply.ReadString());
    }
    if (vServerSize > 0) {
        servers.assign(vecString.begin(), vecString.end());
    }
    int32_t vDomainSize = reply.ReadInt32();
    vDomainSize = vDomainSize > MAX_DNS_CONFIG_SIZE ? MAX_DNS_CONFIG_SIZE : vDomainSize;
    std::vector<std::string> vecDomain;
    for (int i = 0; i < vDomainSize; i++) {
        vecDomain.push_back(reply.ReadString());
    }
    if (vDomainSize > 0) {
        domains.assign(vecDomain.begin(), vecDomain.end());
    }
    NETNATIVE_LOGI("Begin to GetResolverConfig %{public}d", result);
    return result;
}

int32_t NetsysNativeServiceProxy::CreateNetworkCache(uint16_t netId)
{
    NETNATIVE_LOGI("Begin to CreateNetworkCache");
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteUint16(netId)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_CREATE_NETWORK_CACHE, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::DestroyNetworkCache(uint16_t netId)
{
    NETNATIVE_LOGI("Begin to DestroyNetworkCache");
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteUint16(netId)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_DESTROY_NETWORK_CACHE, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::GetAddrInfo(const std::string &hostName, const std::string &serverName,
                                              const AddrInfo &hints, uint16_t netId, std::vector<AddrInfo> &res)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data) || !data.WriteString(hostName) || !data.WriteString(serverName) ||
        !data.WriteRawData(&hints, sizeof(AddrInfo)) || !data.WriteUint16(netId)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_GET_ADDR_INFO, data, reply, option);

    int32_t ret;
    uint32_t addrSize;
    if (!reply.ReadInt32(ret) || ret != ERR_NONE || !reply.ReadUint32(addrSize) || addrSize > MAX_RESULTS) {
        return ERR_INVALID_DATA;
    }

    std::vector<AddrInfo> infos;
    for (uint32_t i = 0; i < addrSize; ++i) {
        auto p = reply.ReadRawData(sizeof(AddrInfo));
        if (p == nullptr) {
            return ERR_INVALID_DATA;
        }

        AddrInfo info = {};
        if (memcpy_s(&info, sizeof(AddrInfo), p, sizeof(AddrInfo)) != EOK) {
            return ERR_INVALID_DATA;
        }

        infos.emplace_back(info);
    }

    res = infos;
    return ret;
}

int32_t NetsysNativeServiceProxy::SetInterfaceMtu(const std::string &interfaceName, int32_t mtu)
{
    NETNATIVE_LOGI("Begin to SetInterfaceMtu");
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(interfaceName)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(mtu)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_INTERFACE_SET_MTU, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::GetInterfaceMtu(const std::string &interfaceName)
{
    NETNATIVE_LOGI("Begin to GetInterfaceMtu");
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(interfaceName)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_INTERFACE_GET_MTU, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::RegisterNotifyCallback(sptr<INotifyCallback> &callback)
{
    NETNATIVE_LOGI("Begin to RegisterNotifyCallback");
    MessageParcel data;
    if (callback == nullptr) {
        NETNATIVE_LOGE("The parameter of callback is nullptr");
        return ERR_NULL_OBJECT;
    }

    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    data.WriteRemoteObject(callback->AsObject().GetRefPtr());

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_REGISTER_NOTIFY_CALLBACK, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::UnRegisterNotifyCallback(sptr<INotifyCallback> &callback)
{
    NETNATIVE_LOGI("Begin to UnRegisterNotifyCallback");
    MessageParcel data;
    if (callback == nullptr) {
        NETNATIVE_LOGE("The parameter of callback is nullptr");
        return ERR_NULL_OBJECT;
    }

    if (!WriteInterfaceToken(data)) {
        NETNATIVE_LOGE("WriteInterfaceToken fail");
        return ERR_FLATTEN_OBJECT;
    }

    data.WriteRemoteObject(callback->AsObject().GetRefPtr());

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_UNREGISTER_NOTIFY_CALLBACK, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::NetworkAddRoute(int32_t netId, const std::string &interfaceName,
                                                  const std::string &destination, const std::string &nextHop)
{
    NETNATIVE_LOGI("Begin to NetworkAddRoute");
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(netId)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(interfaceName)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(destination)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(nextHop)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_NETWORK_ADD_ROUTE, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::NetworkRemoveRoute(int32_t netId, const std::string &interfaceName,
                                                     const std::string &destination, const std::string &nextHop)
{
    NETNATIVE_LOGI("Begin to NetworkRemoveRoute");
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(netId)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(interfaceName)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(destination)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(nextHop)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_NETWORK_REMOVE_ROUTE, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::NetworkAddRouteParcel(int32_t netId, const RouteInfoParcel &routeInfo)
{
    NETNATIVE_LOGI("Begin to NetworkAddRouteParcel");
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(netId)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(routeInfo.ifName)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(routeInfo.destination)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(routeInfo.nextHop)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_NETWORK_ADD_ROUTE_PARCEL, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::NetworkRemoveRouteParcel(int32_t netId, const RouteInfoParcel &routeInfo)
{
    NETNATIVE_LOGI("Begin to NetworkRemoveRouteParcel");
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(netId)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(routeInfo.ifName)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(routeInfo.destination)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(routeInfo.nextHop)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_NETWORK_REMOVE_ROUTE_PARCEL, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::NetworkSetDefault(int32_t netId)
{
    NETNATIVE_LOGI("Begin to NetworkSetDefault");
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(netId)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_NETWORK_SET_DEFAULT, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::NetworkGetDefault()
{
    NETNATIVE_LOGI("Begin to NetworkGetDefault");
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_NETWORK_GET_DEFAULT, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::NetworkClearDefault()
{
    NETNATIVE_LOGI("Begin to NetworkClearDefault");
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_NETWORK_CLEAR_DEFAULT, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::GetProcSysNet(int32_t family, int32_t which, const std::string &ifname,
                                                const std::string &parameter, std::string &value)
{
    NETNATIVE_LOGI("Begin to GetSysProcNet");
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }

    if (!data.WriteInt32(family)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(which)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (data.WriteString(ifname)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(parameter)) {
        return ERR_FLATTEN_OBJECT;
    }
    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_GET_PROC_SYS_NET, data, reply, option);
    int32_t ret = reply.ReadInt32();
    if (ret != ERR_NONE) {
        NETNATIVE_LOGE("Fail to GetProcSysNet ret= %{public}d", ret);
        return ret;
    }
    std::string valueRsl = reply.ReadString();
    NETNATIVE_LOGE("NETSYS_GET_PROC_SYS_NET value %{public}s", valueRsl.c_str());
    value = valueRsl;
    return ret;
}

int32_t NetsysNativeServiceProxy::SetProcSysNet(int32_t family, int32_t which, const std::string &ifname,
                                                const std::string &parameter, std::string &value)
{
    NETNATIVE_LOGI("Begin to SetSysProcNet");
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(family)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(which)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (data.WriteString(ifname)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(parameter)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(value)) {
        return ERR_FLATTEN_OBJECT;
    }
    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_SET_PROC_SYS_NET, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::NetworkCreatePhysical(int32_t netId, int32_t permission)
{
    NETNATIVE_LOGI("Begin to NetworkCreatePhysical");
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(netId)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(permission)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_NETWORK_CREATE_PHYSICAL, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::AddInterfaceAddress(const std::string &interfaceName, const std::string &addrString,
                                                      int32_t prefixLength)
{
    NETNATIVE_LOGI("Begin to AddInterfaceAddress");
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(interfaceName)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(addrString)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(prefixLength)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_INTERFACE_ADD_ADDRESS, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::DelInterfaceAddress(const std::string &interfaceName, const std::string &addrString,
                                                      int32_t prefixLength)
{
    NETNATIVE_LOGI("Begin to DelInterfaceAddress");
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(interfaceName)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(addrString)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(prefixLength)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_INTERFACE_DEL_ADDRESS, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::InterfaceSetIpAddress(const std::string &ifaceName, const std::string &ipAddress)
{
    NETNATIVE_LOGI("Begin to InterfaceSetIpAddress");
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(ifaceName) || !data.WriteString(ipAddress)) {
        return ERR_FLATTEN_OBJECT;
    }
    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_INTERFACE_SET_IP_ADDRESS, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::InterfaceSetIffUp(const std::string &ifaceName)
{
    NETNATIVE_LOGI("Begin to InterfaceSetIffUp");
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(ifaceName)) {
        return ERR_FLATTEN_OBJECT;
    }
    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_INTERFACE_SET_IFF_UP, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::NetworkAddInterface(int32_t netId, const std::string &iface)
{
    NETNATIVE_LOGI("Begin to NetworkAddInterface");
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(netId)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(iface)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_NETWORK_ADD_INTERFACE, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::NetworkRemoveInterface(int32_t netId, const std::string &iface)
{
    NETNATIVE_LOGI("Begin to NetworkRemoveInterface");
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(netId)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(iface)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_NETWORK_REMOVE_INTERFACE, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::NetworkDestroy(int32_t netId)
{
    NETNATIVE_LOGI("Begin to NetworkDestroy");
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(netId)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_NETWORK_DESTROY, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::GetFwmarkForNetwork(int32_t netId, MarkMaskParcel &markMaskParcel)
{
    NETNATIVE_LOGI("Begin to GetFwmarkForNetwork");
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(netId)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(markMaskParcel.mark)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(markMaskParcel.mask)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_GET_FWMARK_FOR_NETWORK, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::SetInterfaceConfig(const InterfaceConfigurationParcel &cfg)
{
    NETNATIVE_LOGI("Begin to SetInterfaceConfig");
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(cfg.ifName)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(cfg.hwAddr)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(cfg.ipv4Addr)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(cfg.prefixLength)) {
        return ERR_FLATTEN_OBJECT;
    }
    int32_t vsize = static_cast<int32_t>(cfg.flags.size());
    if (!data.WriteInt32(vsize)) {
        return ERR_FLATTEN_OBJECT;
    }
    std::vector<std::string> vCflags;
    vCflags.assign(cfg.flags.begin(), cfg.flags.end());
    NETNATIVE_LOGI("PROXY: SetInterfaceConfig Write flags String_SIZE: %{public}d",
                   static_cast<int32_t>(vCflags.size()));
    for (std::vector<std::string>::iterator it = vCflags.begin(); it != vCflags.end(); ++it) {
        data.WriteString(*it);
    }
    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_INTERFACE_SET_CONFIG, data, reply, option);

    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::GetInterfaceConfig(InterfaceConfigurationParcel &cfg)
{
    NETNATIVE_LOGI("Begin to GetInterfaceConfig");
    MessageParcel data;
    int32_t ret;
    int32_t vSize;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(cfg.ifName)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_INTERFACE_GET_CONFIG, data, reply, option);
    ret = reply.ReadInt32();
    if (ret != ERR_NONE) {
        NETNATIVE_LOGE("Fail to GetInterfaceConfig ret= %{public}d", ret);
        return ret;
    }
    reply.ReadString(cfg.ifName);
    reply.ReadString(cfg.hwAddr);
    reply.ReadString(cfg.ipv4Addr);
    reply.ReadInt32(cfg.prefixLength);
    vSize = reply.ReadInt32();
    vSize = vSize > MAX_INTERFACE_CONFIG_SIZE ? MAX_INTERFACE_CONFIG_SIZE : vSize;
    std::vector<std::string> vecString;
    for (int i = 0; i < vSize; i++) {
        vecString.push_back(reply.ReadString());
    }
    if (vSize > 0) {
        cfg.flags.assign(vecString.begin(), vecString.end());
    }
    NETNATIVE_LOGI("End to GetInterfaceConfig, ret =%{public}d", ret);
    return ret;
}

int32_t NetsysNativeServiceProxy::InterfaceGetList(std::vector<std::string> &ifaces)
{
    NETNATIVE_LOGI("NetsysNativeServiceProxy Begin to InterfaceGetList");
    MessageParcel data;
    int32_t ret;
    int32_t vSize;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_INTERFACE_GET_LIST, data, reply, option);
    ret = reply.ReadInt32();
    if (ret != ERR_NONE) {
        NETNATIVE_LOGE("Fail to InterfaceGetList ret= %{public}d", ret);
        return ret;
    }
    vSize = reply.ReadInt32();
    std::vector<std::string> vecString;
    for (int i = 0; i < vSize; i++) {
        vecString.push_back(reply.ReadString());
    }
    if (vSize > 0) {
        ifaces.assign(vecString.begin(), vecString.end());
    }
    NETNATIVE_LOGI("NetsysNativeServiceProxy End to InterfaceGetList, ret =%{public}d", ret);
    return ret;
}

int32_t NetsysNativeServiceProxy::StartDhcpClient(const std::string &iface, bool bIpv6)
{
    NETNATIVE_LOGI("Begin to StartDhcpClient");
    MessageParcel data;
    int32_t ret;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(iface)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteBool(bIpv6)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_START_DHCP_CLIENT, data, reply, option);

    ret = reply.ReadInt32();
    NETNATIVE_LOGI("End to StartDhcpClient, ret =%{public}d", ret);
    return ret;
}

int32_t NetsysNativeServiceProxy::StopDhcpClient(const std::string &iface, bool bIpv6)
{
    NETNATIVE_LOGI("Begin to StopDhcpClient");
    MessageParcel data;
    int32_t ret;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(iface)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteBool(bIpv6)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    ret = Remote()->SendRequest(INetsysService::NETSYS_STOP_DHCP_CLIENT, data, reply, option);
    NETNATIVE_LOGI("SendRequest, ret =%{public}d", ret);
    ret = reply.ReadInt32();
    NETNATIVE_LOGI("End to StopDhcpClient, ret =%{public}d", ret);
    return ret;
}

int32_t NetsysNativeServiceProxy::StartDhcpService(const std::string &iface, const std::string &ipv4addr)
{
    NETNATIVE_LOGI("Begin to StartDhcpService");
    MessageParcel data;

    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(iface)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(ipv4addr)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_START_DHCP_SERVICE, data, reply, option);
    int32_t ret = reply.ReadInt32();
    NETNATIVE_LOGI("End to StartDhcpService, ret =%{public}d", ret);
    return ret;
}

int32_t NetsysNativeServiceProxy::StopDhcpService(const std::string &iface)
{
    NETNATIVE_LOGI("Begin to StopDhcpService");
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(iface)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_STOP_DHCP_SERVICE, data, reply, option);
    int32_t ret = reply.ReadInt32();
    NETNATIVE_LOGI("End to StopDhcpService, ret =%{public}d", ret);
    return ret;
}

int32_t NetsysNativeServiceProxy::IpEnableForwarding(const std::string &requestor)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data) || !data.WriteString(requestor)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_IPENABLE_FORWARDING, data, reply, option);

    int32_t ret = reply.ReadInt32();
    NETNATIVE_LOGI("End to IpEnableForwarding, ret =%{public}d", ret);
    return ret;
}

int32_t NetsysNativeServiceProxy::IpDisableForwarding(const std::string &requestor)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data) || !data.WriteString(requestor)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_IPDISABLE_FORWARDING, data, reply, option);

    int32_t ret = reply.ReadInt32();
    NETNATIVE_LOGI("End to IpDisableForwarding, ret =%{public}d", ret);
    return ret;
}

int32_t NetsysNativeServiceProxy::EnableNat(const std::string &downstreamIface, const std::string &upstreamIface)
{
    MessageParcel data;
    if (!WriteNatDataToMessage(data, downstreamIface, upstreamIface)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_ENABLE_NAT, data, reply, option);

    int32_t ret = reply.ReadInt32();
    NETNATIVE_LOGI("End to EnableNat, ret =%{public}d", ret);
    return ret;
}

int32_t NetsysNativeServiceProxy::DisableNat(const std::string &downstreamIface, const std::string &upstreamIface)
{
    MessageParcel data;
    if (!WriteNatDataToMessage(data, downstreamIface, upstreamIface)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_DISABLE_NAT, data, reply, option);

    int32_t ret = reply.ReadInt32();
    NETNATIVE_LOGI("End to DisableNat, ret =%{public}d", ret);
    return ret;
}

int32_t NetsysNativeServiceProxy::IpfwdAddInterfaceForward(const std::string &fromIface, const std::string &toIface)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data) || !data.WriteString(fromIface) || !data.WriteString(toIface)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_IPFWD_ADD_INTERFACE_FORWARD, data, reply, option);

    int32_t ret = reply.ReadInt32();
    NETNATIVE_LOGI("End to IpfwdAddInterfaceForward, ret =%{public}d", ret);
    return ret;
}

int32_t NetsysNativeServiceProxy::IpfwdRemoveInterfaceForward(const std::string &fromIface, const std::string &toIface)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data) || !data.WriteString(fromIface) || !data.WriteString(toIface)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_IPFWD_REMOVE_INTERFACE_FORWARD, data, reply, option);

    int32_t ret = reply.ReadInt32();
    NETNATIVE_LOGI("End to IpfwdRemoveInterfaceForward, ret =%{public}d", ret);
    return ret;
}

int32_t NetsysNativeServiceProxy::BandwidthEnableDataSaver(bool enable)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETNATIVE_LOGE("WriteInterfaceToken failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteBool(enable)) {
        NETNATIVE_LOGE("WriteBool failed");
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    auto remote = Remote();
    if (remote) {
        if (ERR_NONE != remote->SendRequest(INetsysService::NETSYS_BANDWIDTH_ENABLE_DATA_SAVER, data, reply, option)) {
            NETNATIVE_LOGE("proxy SendRequest failed");
            return ERR_FLATTEN_OBJECT;
        }
    }
    int32_t ret = reply.ReadInt32();
    return ret;
}

int32_t NetsysNativeServiceProxy::BandwidthSetIfaceQuota(const std::string &ifName, int64_t bytes)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETNATIVE_LOGE("WriteInterfaceToken failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(ifName)) {
        NETNATIVE_LOGE("WriteString failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteInt64(bytes)) {
        NETNATIVE_LOGE("WriteInt64 failed");
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    auto remote = Remote();
    if (remote) {
        if (ERR_NONE != remote->SendRequest(INetsysService::NETSYS_BANDWIDTH_SET_IFACE_QUOTA, data, reply, option)) {
            NETNATIVE_LOGE("proxy SendRequest failed");
            return ERR_FLATTEN_OBJECT;
        }
    }
    int32_t ret = reply.ReadInt32();
    return ret;
}

int32_t NetsysNativeServiceProxy::BandwidthRemoveIfaceQuota(const std::string &ifName)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETNATIVE_LOGE("WriteInterfaceToken failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(ifName)) {
        NETNATIVE_LOGE("WriteString failed");
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    auto remote = Remote();
    if (remote) {
        if (ERR_NONE != remote->SendRequest(INetsysService::NETSYS_BANDWIDTH_REMOVE_IFACE_QUOTA, data, reply, option)) {
            NETNATIVE_LOGE("proxy SendRequest failed");
            return ERR_FLATTEN_OBJECT;
        }
    }
    int32_t ret = reply.ReadInt32();
    return ret;
}

int32_t NetsysNativeServiceProxy::BandwidthAddDeniedList(uint32_t uid)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETNATIVE_LOGE("WriteInterfaceToken failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteUint32(uid)) {
        NETNATIVE_LOGE("WriteUint32 failed");
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    auto remote = Remote();
    if (remote) {
        if (ERR_NONE != remote->SendRequest(INetsysService::NETSYS_BANDWIDTH_ADD_DENIED_LIST, data, reply, option)) {
            NETNATIVE_LOGE("proxy SendRequest failed");
            return ERR_FLATTEN_OBJECT;
        }
    }
    int32_t ret = reply.ReadInt32();
    return ret;
}

int32_t NetsysNativeServiceProxy::BandwidthRemoveDeniedList(uint32_t uid)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETNATIVE_LOGE("WriteInterfaceToken failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteUint32(uid)) {
        NETNATIVE_LOGE("WriteUint32 failed");
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    auto remote = Remote();
    if (remote) {
        if (ERR_NONE != remote->SendRequest(INetsysService::NETSYS_BANDWIDTH_REMOVE_DENIED_LIST, data, reply, option)) {
            NETNATIVE_LOGE("proxy SendRequest failed");
            return ERR_FLATTEN_OBJECT;
        }
    }
    int32_t ret = reply.ReadInt32();
    return ret;
}

int32_t NetsysNativeServiceProxy::BandwidthAddAllowedList(uint32_t uid)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETNATIVE_LOGE("WriteInterfaceToken failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteUint32(uid)) {
        NETNATIVE_LOGE("WriteUint32 failed");
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    auto remote = Remote();
    if (remote) {
        if (ERR_NONE != remote->SendRequest(INetsysService::NETSYS_BANDWIDTH_ADD_ALLOWED_LIST, data, reply, option)) {
            NETNATIVE_LOGE("proxy SendRequest failed");
            return ERR_FLATTEN_OBJECT;
        }
    }
    int32_t ret = reply.ReadInt32();
    return ret;
}

int32_t NetsysNativeServiceProxy::BandwidthRemoveAllowedList(uint32_t uid)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETNATIVE_LOGE("WriteInterfaceToken failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteUint32(uid)) {
        NETNATIVE_LOGE("WriteUint32 failed");
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    auto remote = Remote();
    if (remote) {
        if (ERR_NONE !=
            remote->SendRequest(INetsysService::NETSYS_BANDWIDTH_REMOVE_ALLOWED_LIST, data, reply, option)) {
            NETNATIVE_LOGE("proxy SendRequest failed");
            return ERR_FLATTEN_OBJECT;
        }
    }
    int32_t ret = reply.ReadInt32();
    return ret;
}

int32_t NetsysNativeServiceProxy::FirewallSetUidsAllowedListChain(uint32_t chain, const std::vector<uint32_t> &uids)
{
    MessageParcel data;
    uint32_t uidSize = uids.size();
    if (uidSize > UIDS_LIST_MAX_SIZE) {
        NETNATIVE_LOGE("Uids size err [%{public}d]", uidSize);
        return ERR_INVALID_DATA;
    }
    if (!WriteInterfaceToken(data)) {
        NETNATIVE_LOGE("WriteInterfaceToken failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteUint32(chain)) {
        NETNATIVE_LOGE("WriteUint32 failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteUint32(uidSize)) {
        NETNATIVE_LOGE("WriteUint32 failed");
        return ERR_FLATTEN_OBJECT;
    }
    std::vector<uint32_t> vUids;
    vUids.assign(uids.begin(), uids.end());
    std::for_each(vUids.begin(), vUids.end(), [&data](uint32_t uid) { data.WriteUint32(uid); });

    MessageParcel reply;
    MessageOption option;
    auto remote = Remote();
    if (remote) {
        if (ERR_NONE !=
            remote->SendRequest(INetsysService::NETSYS_FIREWALL_SET_UID_ALLOWED_LIST_CHAIN, data, reply, option)) {
            NETNATIVE_LOGE("proxy SendRequest failed");
            return ERR_FLATTEN_OBJECT;
        }
    }
    int32_t ret = reply.ReadInt32();
    return ret;
}

int32_t NetsysNativeServiceProxy::FirewallSetUidsDeniedListChain(uint32_t chain, const std::vector<uint32_t> &uids)
{
    MessageParcel data;
    uint32_t uidSize = uids.size();
    if (uidSize > UIDS_LIST_MAX_SIZE) {
        NETNATIVE_LOGE("Uids size err [%{public}d]", uidSize);
        return ERR_INVALID_DATA;
    }
    if (!WriteInterfaceToken(data)) {
        NETNATIVE_LOGE("WriteInterfaceToken failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteUint32(chain)) {
        NETNATIVE_LOGE("WriteUint32 Error");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteUint32(uidSize)) {
        NETNATIVE_LOGE("WriteUint32 Error");
        return ERR_FLATTEN_OBJECT;
    }
    std::vector<uint32_t> vUids;
    vUids.assign(uids.begin(), uids.end());
    std::for_each(vUids.begin(), vUids.end(), [&data](uint32_t uid) { data.WriteUint32(uid); });

    MessageParcel reply;
    MessageOption option;
    auto remote = Remote();
    if (remote) {
        if (ERR_NONE !=
            remote->SendRequest(INetsysService::NETSYS_FIREWALL_SET_UID_DENIED_LIST_CHAIN, data, reply, option)) {
            NETNATIVE_LOGE("proxy SendRequest failed");
            return ERR_FLATTEN_OBJECT;
        }
    }
    int32_t ret = reply.ReadInt32();
    return ret;
}

int32_t NetsysNativeServiceProxy::FirewallEnableChain(uint32_t chain, bool enable)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETNATIVE_LOGE("WriteInterfaceToken failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteUint32(chain)) {
        NETNATIVE_LOGE("WriteUint32 Error");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteBool(enable)) {
        NETNATIVE_LOGE("WriteBool Error");
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    auto remote = Remote();
    if (remote) {
        if (ERR_NONE != remote->SendRequest(INetsysService::NETSYS_FIREWALL_ENABLE_CHAIN, data, reply, option)) {
            NETNATIVE_LOGE("proxy SendRequest failed");
            return ERR_FLATTEN_OBJECT;
        }
    }
    int32_t ret = reply.ReadInt32();
    return ret;
}

int32_t NetsysNativeServiceProxy::FirewallSetUidRule(uint32_t chain, uint32_t uid, uint32_t firewallRule)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        NETNATIVE_LOGE("WriteInterfaceToken failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteUint32(chain)) {
        NETNATIVE_LOGE("WriteUint32 failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteUint32(uid)) {
        NETNATIVE_LOGE("WriteUint32 failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteUint32(firewallRule)) {
        NETNATIVE_LOGE("WriteUint32 failed");
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    auto remote = Remote();
    if (remote) {
        if (ERR_NONE != remote->SendRequest(INetsysService::NETSYS_FIREWALL_SET_UID_RULE, data, reply, option)) {
            NETNATIVE_LOGE("proxy SendRequest failed");
            return ERR_FLATTEN_OBJECT;
        }
    }
    int32_t ret = reply.ReadInt32();
    return ret;
}

int32_t NetsysNativeServiceProxy::ShareDnsSet(uint16_t netId)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteUint16(netId)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t error = Remote()->SendRequest(INetsysService::NETSYS_TETHER_DNS_SET, data, reply, option);
    if (error != ERR_NONE) {
        NETNATIVE_LOGE("proxy SendRequest failed, error code: [%{public}d]", error);
        return error;
    }
    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::StartDnsProxyListen()
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t error = Remote()->SendRequest(INetsysService::NETSYS_START_DNS_PROXY_LISTEN, data, reply, option);
    if (error != ERR_NONE) {
        NETNATIVE_LOGE("proxy SendRequest failed, error code: [%{public}d]", error);
        return error;
    }
    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::StopDnsProxyListen()
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    int32_t error = Remote()->SendRequest(INetsysService::NETSYS_STOP_DNS_PROXY_LISTEN, data, reply, option);
    if (error != ERR_NONE) {
        NETNATIVE_LOGE("proxy SendRequest failed, error code: [%{public}d]", error);
        return error;
    }
    return reply.ReadInt32();
}

int32_t NetsysNativeServiceProxy::GetNetworkSharingTraffic(const std::string &downIface, const std::string &upIface,
                                                           NetworkSharingTraffic &traffic)
{
    MessageParcel data;
    if (!WriteInterfaceToken(data)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(downIface)) {
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteString(upIface)) {
        return ERR_FLATTEN_OBJECT;
    }

    MessageParcel reply;
    MessageOption option;
    Remote()->SendRequest(INetsysService::NETSYS_GET_SHARING_NETWORK_TRAFFIC, data, reply, option);

    int32_t ret = reply.ReadInt32();
    if (ret != ERR_NONE) {
        NETNATIVE_LOGE("Fail to GetNetworkSharingTraffic ret= %{public}d", ret);
        return ret;
    }

    traffic.receive = reply.ReadInt64();
    traffic.send = reply.ReadInt64();
    traffic.all = reply.ReadInt64();
    NETNATIVE_LOGI("NetsysNativeServiceProxy GetNetworkSharingTraffic ret=%{public}d", ret);
    return ret;
}
} // namespace NetsysNative
} // namespace OHOS
