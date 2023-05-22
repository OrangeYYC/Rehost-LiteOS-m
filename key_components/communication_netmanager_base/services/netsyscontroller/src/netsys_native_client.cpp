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

#include <arpa/inet.h>
#include <cstring>
#include <fcntl.h>
#include <linux/if_tun.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>

#include "iservice_registry.h"
#include "system_ability_definition.h"

#include "net_conn_constants.h"
#include "net_conn_types.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "netmanager_base_common_utils.h"
#include "netsys_native_client.h"
#include "netsys_native_service_proxy.h"

using namespace OHOS::NetManagerStandard::CommonUtils;
namespace OHOS {
namespace NetManagerStandard {
static constexpr const char *DEV_NET_TUN_PATH = "/dev/net/tun";
static constexpr const char *IF_CFG_UP = "up";
static constexpr const char *IF_CFG_DOWN = "down";
static constexpr const char *NETSYS_ROUTE_INIT_DIR_PATH = "/data/service/el1/public/netmanager/route";
static constexpr uint32_t WAIT_FOR_SERVICE_TIME_S = 1;
static constexpr uint32_t MAX_GET_SERVICE_COUNT = 30;

NetsysNativeClient::NativeNotifyCallback::NativeNotifyCallback(NetsysNativeClient &netsysNativeClient)
    : netsysNativeClient_(netsysNativeClient)
{
}

int32_t NetsysNativeClient::NativeNotifyCallback::OnInterfaceAddressUpdated(const std::string &addr,
                                                                            const std::string &ifName, int flags,
                                                                            int scope)
{
    std::lock_guard lock(netsysNativeClient_.cbObjMutex_);
    for (auto &cb : netsysNativeClient_.cbObjects_) {
        cb->OnInterfaceAddressUpdated(addr, ifName, flags, scope);
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::NativeNotifyCallback::OnInterfaceAddressRemoved(const std::string &addr,
                                                                            const std::string &ifName, int flags,
                                                                            int scope)
{
    std::lock_guard lock(netsysNativeClient_.cbObjMutex_);
    for (auto &cb : netsysNativeClient_.cbObjects_) {
        cb->OnInterfaceAddressRemoved(addr, ifName, flags, scope);
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::NativeNotifyCallback::OnInterfaceAdded(const std::string &ifName)
{
    std::lock_guard lock(netsysNativeClient_.cbObjMutex_);
    for (auto &cb : netsysNativeClient_.cbObjects_) {
        cb->OnInterfaceAdded(ifName);
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::NativeNotifyCallback::OnInterfaceRemoved(const std::string &ifName)
{
    std::lock_guard lock(netsysNativeClient_.cbObjMutex_);
    for (auto &cb : netsysNativeClient_.cbObjects_) {
        cb->OnInterfaceRemoved(ifName);
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::NativeNotifyCallback::OnInterfaceChanged(const std::string &ifName, bool up)
{
    std::lock_guard lock(netsysNativeClient_.cbObjMutex_);
    for (auto &cb : netsysNativeClient_.cbObjects_) {
        cb->OnInterfaceChanged(ifName, up);
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::NativeNotifyCallback::OnInterfaceLinkStateChanged(const std::string &ifName, bool up)
{
    std::lock_guard lock(netsysNativeClient_.cbObjMutex_);
    for (auto &cb : netsysNativeClient_.cbObjects_) {
        cb->OnInterfaceLinkStateChanged(ifName, up);
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::NativeNotifyCallback::OnRouteChanged(bool updated, const std::string &route,
                                                                 const std::string &gateway, const std::string &ifName)
{
    std::lock_guard lock(netsysNativeClient_.cbObjMutex_);
    for (auto &cb : netsysNativeClient_.cbObjects_) {
        cb->OnRouteChanged(updated, route, gateway, ifName);
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::NativeNotifyCallback::OnDhcpSuccess(sptr<OHOS::NetsysNative::DhcpResultParcel> &dhcpResult)
{
    NETMGR_LOG_I("NetsysNativeClient::NativeNotifyCallback::OnDhcpSuccess");
    netsysNativeClient_.ProcessDhcpResult(dhcpResult);
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::NativeNotifyCallback::OnBandwidthReachedLimit(const std::string &limitName,
                                                                          const std::string &iface)
{
    NETMGR_LOG_I("NetsysNativeClient::NativeNotifyCallback::OnBandwidthReachedLimit");
    netsysNativeClient_.ProcessBandwidthReachedLimit(limitName, iface);
    return NETMANAGER_SUCCESS;
}

NetsysNativeClient::NetsysNativeClient()
{
    std::thread([this]() {
        uint32_t count = 0;
        while (GetProxy() == nullptr && count < MAX_GET_SERVICE_COUNT) {
            std::this_thread::sleep_for(std::chrono::seconds(WAIT_FOR_SERVICE_TIME_S));
            count++;
        }
        auto proxy = GetProxy();
        NETMGR_LOG_W("Get proxy %{public}s, count: %{public}u", proxy == nullptr ? "failed" : "success", count);
        if (proxy != nullptr) {
            nativeNotifyCallback_ = new (std::nothrow) NativeNotifyCallback(*this);
            proxy->RegisterNotifyCallback(nativeNotifyCallback_);
        }
    }).detach();
}

int32_t NetsysNativeClient::NetworkCreatePhysical(int32_t netId, int32_t permission)
{
    NETMGR_LOG_I("Create Physical network: netId[%{public}d], permission[%{public}d]", netId, permission);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->NetworkCreatePhysical(netId, permission);
}

int32_t NetsysNativeClient::NetworkDestroy(int32_t netId)
{
    NETMGR_LOG_I("Destroy network: netId[%{public}d]", netId);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->NetworkDestroy(netId);
}

int32_t NetsysNativeClient::NetworkAddInterface(int32_t netId, const std::string &iface)
{
    NETMGR_LOG_I("Add network interface: netId[%{public}d], iface[%{public}s]", netId, iface.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->NetworkAddInterface(netId, iface);
}

int32_t NetsysNativeClient::NetworkRemoveInterface(int32_t netId, const std::string &iface)
{
    NETMGR_LOG_I("Remove network interface: netId[%{public}d], iface[%{public}s]", netId, iface.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->NetworkRemoveInterface(netId, iface);
}

int32_t NetsysNativeClient::NetworkAddRoute(int32_t netId, const std::string &ifName, const std::string &destination,
                                            const std::string &nextHop)
{
    NETMGR_LOG_I("Add Route: netId[%{public}d], ifName[%{public}s], destination[%{public}s], nextHop[%{public}s]",
                 netId, ifName.c_str(), ToAnonymousIp(destination).c_str(), ToAnonymousIp(nextHop).c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->NetworkAddRoute(netId, ifName, destination, nextHop);
}

int32_t NetsysNativeClient::NetworkRemoveRoute(int32_t netId, const std::string &ifName, const std::string &destination,
                                               const std::string &nextHop)
{
    NETMGR_LOG_D("Remove Route: netId[%{public}d], ifName[%{public}s], destination[%{public}s], nextHop[%{public}s]",
                 netId, ifName.c_str(), ToAnonymousIp(destination).c_str(), ToAnonymousIp(nextHop).c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->NetworkRemoveRoute(netId, ifName, destination, nextHop);
}

int32_t NetsysNativeClient::GetInterfaceConfig(OHOS::nmd::InterfaceConfigurationParcel &cfg)
{
    NETMGR_LOG_D("Get interface config: ifName[%{public}s]", cfg.ifName.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetInterfaceConfig(cfg);
}

int32_t NetsysNativeClient::SetInterfaceConfig(const OHOS::nmd::InterfaceConfigurationParcel &cfg)
{
    NETMGR_LOG_D("Set interface config: ifName[%{public}s]", cfg.ifName.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetInterfaceConfig(cfg);
}

int32_t NetsysNativeClient::SetInterfaceDown(const std::string &iface)
{
    NETMGR_LOG_D("Set interface down: iface[%{public}s]", iface.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    OHOS::nmd::InterfaceConfigurationParcel ifCfg;
    ifCfg.ifName = iface;
    proxy->GetInterfaceConfig(ifCfg);
    auto fit = std::find(ifCfg.flags.begin(), ifCfg.flags.end(), IF_CFG_UP);
    if (fit != ifCfg.flags.end()) {
        ifCfg.flags.erase(fit);
    }
    ifCfg.flags.emplace_back(IF_CFG_DOWN);
    return proxy->SetInterfaceConfig(ifCfg);
}

int32_t NetsysNativeClient::SetInterfaceUp(const std::string &iface)
{
    NETMGR_LOG_D("Set interface up: iface[%{public}s]", iface.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    OHOS::nmd::InterfaceConfigurationParcel ifCfg;
    ifCfg.ifName = iface;
    proxy->GetInterfaceConfig(ifCfg);
    auto fit = std::find(ifCfg.flags.begin(), ifCfg.flags.end(), IF_CFG_DOWN);
    if (fit != ifCfg.flags.end()) {
        ifCfg.flags.erase(fit);
    }
    ifCfg.flags.emplace_back(IF_CFG_UP);
    return proxy->SetInterfaceConfig(ifCfg);
}

void NetsysNativeClient::ClearInterfaceAddrs(const std::string &ifName)
{
    NETMGR_LOG_D("Clear addrs: ifName[%{public}s]", ifName.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return;
    }
}

int32_t NetsysNativeClient::GetInterfaceMtu(const std::string &ifName)
{
    NETMGR_LOG_D("Get mtu: ifName[%{public}s]", ifName.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetInterfaceMtu(ifName);
}

int32_t NetsysNativeClient::SetInterfaceMtu(const std::string &ifName, int32_t mtu)
{
    NETMGR_LOG_D("Set mtu: ifName[%{public}s], mtu[%{public}d]", ifName.c_str(), mtu);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetInterfaceMtu(ifName, mtu);
}

int32_t NetsysNativeClient::AddInterfaceAddress(const std::string &ifName, const std::string &ipAddr,
                                                int32_t prefixLength)
{
    NETMGR_LOG_D("Add address: ifName[%{public}s], ipAddr[%{public}s], prefixLength[%{public}d]", ifName.c_str(),
                 ToAnonymousIp(ipAddr).c_str(), prefixLength);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->AddInterfaceAddress(ifName, ipAddr, prefixLength);
}

int32_t NetsysNativeClient::DelInterfaceAddress(const std::string &ifName, const std::string &ipAddr,
                                                int32_t prefixLength)
{
    NETMGR_LOG_D("Delete address: ifName[%{public}s], ipAddr[%{public}s], prefixLength[%{public}d]", ifName.c_str(),
                 ToAnonymousIp(ipAddr).c_str(), prefixLength);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->DelInterfaceAddress(ifName, ipAddr, prefixLength);
}

int32_t NetsysNativeClient::InterfaceSetIpAddress(const std::string &ifaceName, const std::string &ipAddress)
{
    NETMGR_LOG_D("Set Ip Address: ifaceName[%{public}s], ipAddr[%{public}s]", ifaceName.c_str(),
                 ToAnonymousIp(ipAddress).c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return IPC_PROXY_ERR;
    }
    return proxy->InterfaceSetIpAddress(ifaceName, ipAddress);
}

int32_t NetsysNativeClient::InterfaceSetIffUp(const std::string &ifaceName)
{
    NETMGR_LOG_D("Set Iff Up: ifaceName[%{public}s]", ifaceName.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return IPC_PROXY_ERR;
    }
    return proxy->InterfaceSetIffUp(ifaceName);
}

int32_t NetsysNativeClient::SetResolverConfig(uint16_t netId, uint16_t baseTimeoutMsec, uint8_t retryCount,
                                              const std::vector<std::string> &servers,
                                              const std::vector<std::string> &domains)
{
    NETMGR_LOG_D("Set resolver config: netId[%{public}d]", netId);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->SetResolverConfig(netId, baseTimeoutMsec, retryCount, servers, domains);
}

int32_t NetsysNativeClient::GetResolverConfig(uint16_t netId, std::vector<std::string> &servers,
                                              std::vector<std::string> &domains, uint16_t &baseTimeoutMsec,
                                              uint8_t &retryCount)
{
    NETMGR_LOG_D("Get resolver config: netId[%{public}d]", netId);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetResolverConfig(netId, servers, domains, baseTimeoutMsec, retryCount);
}

int32_t NetsysNativeClient::CreateNetworkCache(uint16_t netId)
{
    NETMGR_LOG_D("create dns cache: netId[%{public}d]", netId);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->CreateNetworkCache(netId);
}

int32_t NetsysNativeClient::DestroyNetworkCache(uint16_t netId)
{
    NETMGR_LOG_D("Destroy dns cache: netId[%{public}d]", netId);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->DestroyNetworkCache(netId);
}

int32_t NetsysNativeClient::GetAddrInfo(const std::string &hostName, const std::string &serverName,
                                        const AddrInfo &hints, uint16_t netId, std::vector<AddrInfo> &res)
{
    if (netsysNativeService_ == nullptr) {
        NETMGR_LOG_E("GetAddrInfo netsysNativeService_ is null");
        return NET_CONN_ERR_SERVICE_UPDATE_NET_LINK_INFO_FAIL;
    }
    return netsysNativeService_->GetAddrInfo(hostName, serverName, hints, netId, res);
}

int32_t NetsysNativeClient::GetNetworkSharingTraffic(const std::string &downIface, const std::string &upIface,
                                                     nmd::NetworkSharingTraffic &traffic)
{
    NETMGR_LOG_D("NetsysNativeClient GetNetworkSharingTraffic");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->GetNetworkSharingTraffic(downIface, upIface, traffic);
}

int64_t NetsysNativeClient::GetCellularRxBytes()
{
    NETMGR_LOG_D("NetsysNativeClient GetCellularRxBytes");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int64_t NetsysNativeClient::GetCellularTxBytes()
{
    NETMGR_LOG_D("NetsysNativeClient GetCellularTxBytes");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int64_t NetsysNativeClient::GetAllRxBytes()
{
    NETMGR_LOG_D("NetsysNativeClient GetAllRxBytes");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int64_t NetsysNativeClient::GetAllTxBytes()
{
    NETMGR_LOG_D("NetsysNativeClient GetAllTxBytes");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int64_t NetsysNativeClient::GetUidRxBytes(uint32_t uid)
{
    NETMGR_LOG_D("NetsysNativeClient GetUidRxBytes uid is [%{public}u]", uid);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int64_t NetsysNativeClient::GetUidTxBytes(uint32_t uid)
{
    NETMGR_LOG_D("NetsysNativeClient GetUidTxBytes uid is [%{public}u]", uid);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int64_t NetsysNativeClient::GetUidOnIfaceRxBytes(uint32_t uid, const std::string &interfaceName)
{
    NETMGR_LOG_D("NetsysNativeClient GetUidOnIfaceRxBytes uid is [%{public}u] iface name is [%{public}s]", uid,
                 interfaceName.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int64_t NetsysNativeClient::GetUidOnIfaceTxBytes(uint32_t uid, const std::string &interfaceName)
{
    NETMGR_LOG_D("NetsysNativeClient GetUidOnIfaceTxBytes uid is [%{public}u] iface name is [%{public}s]", uid,
                 interfaceName.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int64_t NetsysNativeClient::GetIfaceRxBytes(const std::string &interfaceName)
{
    NETMGR_LOG_D("NetsysNativeClient GetIfaceRxBytes iface name is [%{public}s]", interfaceName.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int64_t NetsysNativeClient::GetIfaceTxBytes(const std::string &interfaceName)
{
    NETMGR_LOG_D("NetsysNativeClient GetIfaceTxBytes iface name is [%{public}s]", interfaceName.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

std::vector<std::string> NetsysNativeClient::InterfaceGetList()
{
    NETMGR_LOG_D("NetsysNativeClient InterfaceGetList");
    std::vector<std::string> ret;
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return ret;
    }
    proxy->InterfaceGetList(ret);
    return ret;
}

std::vector<std::string> NetsysNativeClient::UidGetList()
{
    NETMGR_LOG_D("NetsysNativeClient UidGetList");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return {};
    }
    return {};
}

int64_t NetsysNativeClient::GetIfaceRxPackets(const std::string &interfaceName)
{
    NETMGR_LOG_D("NetsysNativeClient GetIfaceRxPackets iface name is [%{public}s]", interfaceName.c_str());
    return NETMANAGER_SUCCESS;
}

int64_t NetsysNativeClient::GetIfaceTxPackets(const std::string &interfaceName)
{
    NETMGR_LOG_D("NetsysNativeClient GetIfaceTxPackets iface name is [%{public}s]", interfaceName.c_str());
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::SetDefaultNetWork(int32_t netId)
{
    NETMGR_LOG_D("NetsysNativeClient SetDefaultNetWork");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->NetworkSetDefault(netId);
}

int32_t NetsysNativeClient::ClearDefaultNetWorkNetId()
{
    NETMGR_LOG_D("NetsysNativeClient ClearDefaultNetWorkNetId");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::BindSocket(int32_t socket_fd, uint32_t netId)
{
    NETMGR_LOG_D("NetsysNativeClient::BindSocket: netId = [%{public}u]", netId);
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::IpEnableForwarding(const std::string &requestor)
{
    NETMGR_LOG_D("NetsysNativeClient IpEnableForwarding: requestor[%{public}s]", requestor.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->IpEnableForwarding(requestor);
}

int32_t NetsysNativeClient::IpDisableForwarding(const std::string &requestor)
{
    NETMGR_LOG_D("NetsysNativeClient IpDisableForwarding: requestor[%{public}s]", requestor.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->IpDisableForwarding(requestor);
}

int32_t NetsysNativeClient::EnableNat(const std::string &downstreamIface, const std::string &upstreamIface)
{
    NETMGR_LOG_D("NetsysNativeClient EnableNat: intIface[%{public}s] intIface[%{public}s]", downstreamIface.c_str(),
                 upstreamIface.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->EnableNat(downstreamIface, upstreamIface);
}

int32_t NetsysNativeClient::DisableNat(const std::string &downstreamIface, const std::string &upstreamIface)
{
    NETMGR_LOG_D("NetsysNativeClient DisableNat: intIface[%{public}s] intIface[%{public}s]", downstreamIface.c_str(),
                 upstreamIface.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->DisableNat(downstreamIface, upstreamIface);
}

int32_t NetsysNativeClient::IpfwdAddInterfaceForward(const std::string &fromIface, const std::string &toIface)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->IpfwdAddInterfaceForward(fromIface, toIface);
}

int32_t NetsysNativeClient::IpfwdRemoveInterfaceForward(const std::string &fromIface, const std::string &toIface)
{
    NETMGR_LOG_D("NetsysNativeClient IpfwdRemoveInterfaceForward: fromIface[%{public}s], toIface[%{public}s]",
                 fromIface.c_str(), toIface.c_str());
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->IpfwdRemoveInterfaceForward(fromIface, toIface);
}

int32_t NetsysNativeClient::ShareDnsSet(uint16_t netId)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->ShareDnsSet(netId);
}

int32_t NetsysNativeClient::StartDnsProxyListen()
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->StartDnsProxyListen();
}

int32_t NetsysNativeClient::StopDnsProxyListen()
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->StopDnsProxyListen();
}

int32_t NetsysNativeClient::RegisterNetsysNotifyCallback(const NetsysNotifyCallback &callback)
{
    (void)callback;
    NETMGR_LOG_D("NetsysNativeClient RegisterNetsysNotifyCallback");
    return NETMANAGER_SUCCESS;
}

sptr<OHOS::NetsysNative::INetsysService> NetsysNativeClient::GetProxy()
{
    std::lock_guard lock(mutex_);
    if (netsysNativeService_) {
        return netsysNativeService_;
    }

    NETMGR_LOG_D("Execute GetSystemAbilityManager");
    auto samgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (samgr == nullptr) {
        NETMGR_LOG_E("NetsysNativeClient samgr null");
        return nullptr;
    }

    auto remote = samgr->GetSystemAbility(OHOS::COMM_NETSYS_NATIVE_SYS_ABILITY_ID);
    if (remote == nullptr) {
        NETMGR_LOG_E("Get remote service failed");
        return nullptr;
    }

    deathRecipient_ = new (std::nothrow) NetNativeConnDeathRecipient(*this);
    if (deathRecipient_ == nullptr) {
        NETMGR_LOG_E("Recipient new failed!");
        return nullptr;
    }

    if ((remote->IsProxyObject()) && (!remote->AddDeathRecipient(deathRecipient_))) {
        NETMGR_LOG_E("add death recipient failed");
        return nullptr;
    }

    netsysNativeService_ = iface_cast<NetsysNative::INetsysService>(remote);
    if (netsysNativeService_ == nullptr) {
        NETMGR_LOG_E("Get remote service proxy failed");
        return nullptr;
    }

    return netsysNativeService_;
}

void NetsysNativeClient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    NETMGR_LOG_D("on remote died");
    if (remote == nullptr) {
        NETMGR_LOG_E("remote object is nullptr");
        return;
    }

    std::lock_guard lock(mutex_);
    if (netsysNativeService_ == nullptr) {
        NETMGR_LOG_E("netsysNativeService_ is nullptr");
        return;
    }

    sptr<IRemoteObject> local = netsysNativeService_->AsObject();
    if (local != remote.promote()) {
        NETMGR_LOG_E("proxy and stub is not same remote object");
        return;
    }
    local->RemoveDeathRecipient(deathRecipient_);

    if (access(NETSYS_ROUTE_INIT_DIR_PATH, F_OK) == 0) {
        NETMGR_LOG_D("NetConnService netsys restart, clear NETSYS_ROUTE_INIT_DIR_PATH");
        rmdir(NETSYS_ROUTE_INIT_DIR_PATH);
    }

    netsysNativeService_ = nullptr;
}

int32_t NetsysNativeClient::BindNetworkServiceVpn(int32_t socketFd)
{
    NETMGR_LOG_D("NetsysNativeClient::BindNetworkServiceVpn: socketFd[%{public}d]", socketFd);
    /* netsys provide default interface name */
    const char *defaultNetName = "wlan0";
    socklen_t defaultNetNameLen = strlen(defaultNetName);
    /* set socket by option. */
    int32_t ret = setsockopt(socketFd, SOL_SOCKET, SO_MARK, defaultNetName, defaultNetNameLen);
    if (ret < 0) {
        NETMGR_LOG_E("The SO_BINDTODEVICE of setsockopt failed.");
        return NETSYS_ERR_VPN;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::EnableVirtualNetIfaceCard(int32_t socketFd, struct ifreq &ifRequest, int32_t &ifaceFd)
{
    NETMGR_LOG_D("NetsysNativeClient::EnableVirtualNetIfaceCard: socketFd[%{public}d]", socketFd);
    int32_t ifaceFdTemp = 0;
    if ((ifaceFdTemp = open(DEV_NET_TUN_PATH, O_RDWR)) < 0) {
        NETMGR_LOG_E("VPN tunnel device open was failed.");
        return NETSYS_ERR_VPN;
    }

    /*
     * Flags:
     * IFF_TUN   - TUN device (no Ethernet headers)
     * IFF_TAP   - TAP device
     * IFF_NO_PI - Do not provide packet information
     **/
    ifRequest.ifr_flags = IFF_TUN | IFF_NO_PI;
    /**
     * Try to create the device. if it cannot assign the device interface name, kernel can
     * allocate the next device interface name. for example, there is tun0, kernel can
     * allocate tun1.
     **/
    if (ioctl(ifaceFdTemp, TUNSETIFF, &ifRequest) < 0) {
        NETMGR_LOG_E("The TUNSETIFF of ioctl failed, ifRequest.ifr_name[%{public}s]", ifRequest.ifr_name);
        return NETSYS_ERR_VPN;
    }

    /* Activate the device */
    ifRequest.ifr_flags = IFF_UP;
    if (ioctl(socketFd, SIOCSIFFLAGS, &ifRequest) < 0) {
        NETMGR_LOG_E("The SIOCSIFFLAGS of ioctl failed.");
        return NETSYS_ERR_VPN;
    }

    ifaceFd = ifaceFdTemp;
    return NETMANAGER_SUCCESS;
}

static inline in_addr_t *AsInAddr(sockaddr *sa)
{
    return &(reinterpret_cast<sockaddr_in *>(sa))->sin_addr.s_addr;
}

int32_t NetsysNativeClient::SetIpAddress(int32_t socketFd, const std::string &ipAddress, int32_t prefixLen,
                                         struct ifreq &ifRequest)
{
    NETMGR_LOG_D("NetsysNativeClient::SetIpAddress: socketFd[%{public}d]", socketFd);

    ifRequest.ifr_addr.sa_family = AF_INET;
    ifRequest.ifr_netmask.sa_family = AF_INET;

    /* inet_pton is IP ipAddress translation to binary network byte order. */
    if (inet_pton(AF_INET, ipAddress.c_str(), AsInAddr(&ifRequest.ifr_addr)) != 1) {
        NETMGR_LOG_E("inet_pton failed.");
        return NETSYS_ERR_VPN;
    }
    if (ioctl(socketFd, SIOCSIFADDR, &ifRequest) < 0) {
        NETMGR_LOG_E("The SIOCSIFADDR of ioctl failed.");
        return NETSYS_ERR_VPN;
    }
    in_addr_t addressPrefixLength = prefixLen ? (~0 << (MAX_IPV4_ADDRESS_LEN - prefixLen)) : 0;
    *AsInAddr(&ifRequest.ifr_netmask) = htonl(addressPrefixLength);
    if (ioctl(socketFd, SIOCSIFNETMASK, &ifRequest)) {
        NETMGR_LOG_E("The SIOCSIFNETMASK of ioctl failed.");
        return NETSYS_ERR_VPN;
    }

    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::SetBlocking(int32_t ifaceFd, bool isBlock)
{
    NETMGR_LOG_D("NetsysNativeClient::SetBlocking");
    int32_t blockingFlag = 0;
    blockingFlag = fcntl(ifaceFd, F_GETFL);
    if (blockingFlag < 0) {
        NETMGR_LOG_E("The blockingFlag of fcntl failed.");
        return NETSYS_ERR_VPN;
    }

    if (!isBlock) {
        blockingFlag = static_cast<int>(static_cast<uint32_t>(blockingFlag) | static_cast<uint32_t>(O_NONBLOCK));
    } else {
        blockingFlag = static_cast<int>(static_cast<uint32_t>(blockingFlag) | static_cast<uint32_t>(~O_NONBLOCK));
    }

    if (fcntl(ifaceFd, F_SETFL, blockingFlag) < 0) {
        NETMGR_LOG_E("The F_SETFL of fcntl failed.");
        return NETSYS_ERR_VPN;
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysNativeClient::StartDhcpClient(const std::string &iface, bool bIpv6)
{
    NETMGR_LOG_D("NetsysNativeClient::StartDhcpClient");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->StartDhcpClient(iface, bIpv6);
}

int32_t NetsysNativeClient::StopDhcpClient(const std::string &iface, bool bIpv6)
{
    NETMGR_LOG_D("NetsysNativeClient::StopDhcpClient");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->StopDhcpClient(iface, bIpv6);
}

int32_t NetsysNativeClient::RegisterCallback(const sptr<NetsysControllerCallback>& callback)
{
    NETMGR_LOG_D("NetsysNativeClient::RegisterCallback");
    if (callback == nullptr) {
        NETMGR_LOG_E("Callback is nullptr");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return IPC_PROXY_ERR;
    }
    std::lock_guard lock(cbObjMutex_);
    cbObjects_.push_back(callback);
    return NETMANAGER_SUCCESS;
}

void NetsysNativeClient::ProcessDhcpResult(sptr<OHOS::NetsysNative::DhcpResultParcel> &dhcpResult)
{
    NETMGR_LOG_I("NetsysNativeClient::ProcessDhcpResult");
    std::lock_guard lock(cbObjMutex_);
    NetsysControllerCallback::DhcpResult result;
    for (auto &cbObject : cbObjects_) {
        result.iface_ = dhcpResult->iface_;
        result.ipAddr_ = dhcpResult->ipAddr_;
        result.gateWay_ = dhcpResult->gateWay_;
        result.subNet_ = dhcpResult->subNet_;
        result.route1_ = dhcpResult->route1_;
        result.route2_ = dhcpResult->route2_;
        result.dns1_ = dhcpResult->dns1_;
        result.dns2_ = dhcpResult->dns2_;
        cbObject->OnDhcpSuccess(result);
    }
}

int32_t NetsysNativeClient::StartDhcpService(const std::string &iface, const std::string &ipv4addr)
{
    NETMGR_LOG_D("NetsysNativeClient StartDhcpService");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->StartDhcpService(iface, ipv4addr);
}

int32_t NetsysNativeClient::StopDhcpService(const std::string &iface)
{
    NETMGR_LOG_D("NetsysNativeClient StopDhcpService");
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->StopDhcpService(iface);
}

void NetsysNativeClient::ProcessBandwidthReachedLimit(const std::string &limitName, const std::string &iface)
{
    NETMGR_LOG_D("NetsysNativeClient ProcessBandwidthReachedLimit, limitName=%{public}s, iface=%{public}s",
                 limitName.c_str(), iface.c_str());
    std::lock_guard lock(cbObjMutex_);
    std::for_each(cbObjects_.begin(), cbObjects_.end(),
                  [limitName, iface](const sptr<NetsysControllerCallback> &callback) {
                      callback->OnBandwidthReachedLimit(limitName, iface);
                  });
}

int32_t NetsysNativeClient::BandwidthEnableDataSaver(bool enable)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->BandwidthEnableDataSaver(enable);
}

int32_t NetsysNativeClient::BandwidthSetIfaceQuota(const std::string &ifName, int64_t bytes)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->BandwidthSetIfaceQuota(ifName, bytes);
}

int32_t NetsysNativeClient::BandwidthRemoveIfaceQuota(const std::string &ifName)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->BandwidthRemoveIfaceQuota(ifName);
}

int32_t NetsysNativeClient::BandwidthAddDeniedList(uint32_t uid)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->BandwidthAddDeniedList(uid);
}

int32_t NetsysNativeClient::BandwidthRemoveDeniedList(uint32_t uid)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->BandwidthRemoveDeniedList(uid);
}

int32_t NetsysNativeClient::BandwidthAddAllowedList(uint32_t uid)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->BandwidthAddAllowedList(uid);
}

int32_t NetsysNativeClient::BandwidthRemoveAllowedList(uint32_t uid)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->BandwidthRemoveAllowedList(uid);
}

int32_t NetsysNativeClient::FirewallSetUidsAllowedListChain(uint32_t chain, const std::vector<uint32_t> &uids)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->FirewallSetUidsAllowedListChain(chain, uids);
}

int32_t NetsysNativeClient::FirewallSetUidsDeniedListChain(uint32_t chain, const std::vector<uint32_t> &uids)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->FirewallSetUidsDeniedListChain(chain, uids);
}

int32_t NetsysNativeClient::FirewallEnableChain(uint32_t chain, bool enable)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->FirewallEnableChain(chain, enable);
}

int32_t NetsysNativeClient::FirewallSetUidRule(uint32_t chain, uint32_t uid, uint32_t firewallRule)
{
    auto proxy = GetProxy();
    if (proxy == nullptr) {
        NETMGR_LOG_E("proxy is nullptr");
        return NETMANAGER_ERR_GET_PROXY_FAIL;
    }
    return proxy->FirewallSetUidRule(chain, uid, firewallRule);
}
} // namespace NetManagerStandard
} // namespace OHOS
