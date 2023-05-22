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
#include "netsys_controller.h"

#include "net_conn_constants.h"
#include "net_conn_types.h"
#include "net_mgr_log_wrapper.h"
#include "netmanager_base_common_utils.h"
#include "netsys_controller_service_impl.h"

using namespace OHOS::NetManagerStandard::CommonUtils;
namespace OHOS {
namespace NetManagerStandard {
void NetsysController::Init()
{
    NETMGR_LOG_I("netsys Init");
    if (initFlag_) {
        NETMGR_LOG_I("netsys initialization is complete");
        return;
    }
    netsysService_ = std::make_unique<NetsysControllerServiceImpl>().release();
    netsysService_->Init();
    initFlag_ = true;
}

NetsysController &NetsysController::GetInstance()
{
    static NetsysController singleInstance_;
    static std::mutex mutex_;
    if (!singleInstance_.initFlag_) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!singleInstance_.initFlag_) {
            singleInstance_.Init();
        }
    }
    return singleInstance_;
}

int32_t NetsysController::NetworkCreatePhysical(int32_t netId, int32_t permission)
{
    NETMGR_LOG_D("Create Physical network: netId[%{public}d], permission[%{public}d]", netId, permission);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->NetworkCreatePhysical(netId, permission);
}

int32_t NetsysController::NetworkDestroy(int32_t netId)
{
    NETMGR_LOG_D("Destroy network: netId[%{public}d]", netId);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->NetworkDestroy(netId);
}

int32_t NetsysController::NetworkAddInterface(int32_t netId, const std::string &iface)
{
    NETMGR_LOG_D("Add network interface: netId[%{public}d], iface[%{public}s]", netId, iface.c_str());
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->NetworkAddInterface(netId, iface);
}

int32_t NetsysController::NetworkRemoveInterface(int32_t netId, const std::string &iface)
{
    NETMGR_LOG_D("Remove network interface: netId[%{public}d], iface[%{public}s]", netId, iface.c_str());
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->NetworkRemoveInterface(netId, iface);
}

int32_t NetsysController::NetworkAddRoute(int32_t netId, const std::string &ifName, const std::string &destination,
                                          const std::string &nextHop)
{
    NETMGR_LOG_D("Add Route: netId[%{public}d], ifName[%{public}s], destination[%{public}s], nextHop[%{public}s]",
                 netId, ifName.c_str(), ToAnonymousIp(destination).c_str(), ToAnonymousIp(nextHop).c_str());
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->NetworkAddRoute(netId, ifName, destination, nextHop);
}

int32_t NetsysController::NetworkRemoveRoute(int32_t netId, const std::string &ifName, const std::string &destination,
                                             const std::string &nextHop)
{
    NETMGR_LOG_D("Remove Route: netId[%{public}d], ifName[%{public}s], destination[%{public}s], nextHop[%{public}s]",
                 netId, ifName.c_str(), ToAnonymousIp(destination).c_str(), ToAnonymousIp(nextHop).c_str());
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->NetworkRemoveRoute(netId, ifName, destination, nextHop);
}

int32_t NetsysController::GetInterfaceConfig(OHOS::nmd::InterfaceConfigurationParcel &cfg)
{
    NETMGR_LOG_D("get interface config");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->GetInterfaceConfig(cfg);
}

int32_t NetsysController::SetInterfaceConfig(const OHOS::nmd::InterfaceConfigurationParcel &cfg)
{
    NETMGR_LOG_D("set interface config");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->SetInterfaceConfig(cfg);
}

int32_t NetsysController::SetInterfaceDown(const std::string &iface)
{
    NETMGR_LOG_D("Set interface down: iface[%{public}s]", iface.c_str());
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->SetInterfaceDown(iface);
}

int32_t NetsysController::SetInterfaceUp(const std::string &iface)
{
    NETMGR_LOG_D("Set interface up: iface[%{public}s]", iface.c_str());
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->SetInterfaceUp(iface);
}

void NetsysController::ClearInterfaceAddrs(const std::string &ifName)
{
    NETMGR_LOG_D("Clear addrs: ifName[%{public}s]", ifName.c_str());
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return;
    }
    return netsysService_->ClearInterfaceAddrs(ifName);
}

int32_t NetsysController::GetInterfaceMtu(const std::string &ifName)
{
    NETMGR_LOG_D("Get mtu: ifName[%{public}s]", ifName.c_str());
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->GetInterfaceMtu(ifName);
}

int32_t NetsysController::SetInterfaceMtu(const std::string &ifName, int32_t mtu)
{
    NETMGR_LOG_D("Set mtu: ifName[%{public}s], mtu[%{public}d]", ifName.c_str(), mtu);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->SetInterfaceMtu(ifName, mtu);
}

int32_t NetsysController::AddInterfaceAddress(const std::string &ifName, const std::string &ipAddr,
                                              int32_t prefixLength)
{
    NETMGR_LOG_D("Add address: ifName[%{public}s], ipAddr[%{public}s], prefixLength[%{public}d]", ifName.c_str(),
                 ToAnonymousIp(ipAddr).c_str(), prefixLength);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->AddInterfaceAddress(ifName, ipAddr, prefixLength);
}

int32_t NetsysController::DelInterfaceAddress(const std::string &ifName, const std::string &ipAddr,
                                              int32_t prefixLength)
{
    NETMGR_LOG_D("Delete address: ifName[%{public}s], ipAddr[%{public}s], prefixLength[%{public}d]", ifName.c_str(),
                 ToAnonymousIp(ipAddr).c_str(), prefixLength);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->DelInterfaceAddress(ifName, ipAddr, prefixLength);
}

int32_t NetsysController::InterfaceSetIpAddress(const std::string &ifaceName, const std::string &ipAddress)
{
    NETMGR_LOG_D("Set Ip Address: ifName[%{public}s]", ifaceName.c_str());
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->InterfaceSetIpAddress(ifaceName, ipAddress);
}

int32_t NetsysController::InterfaceSetIffUp(const std::string &ifaceName)
{
    NETMGR_LOG_D("Set Iff Up: ifName[%{public}s]", ifaceName.c_str());
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->InterfaceSetIffUp(ifaceName);
}

int32_t NetsysController::SetResolverConfig(uint16_t netId, uint16_t baseTimeoutMsec, uint8_t retryCount,
                                            const std::vector<std::string> &servers,
                                            const std::vector<std::string> &domains)
{
    NETMGR_LOG_D("Set resolver config: netId[%{public}d]", netId);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->SetResolverConfig(netId, baseTimeoutMsec, retryCount, servers, domains);
}

int32_t NetsysController::GetResolverConfig(uint16_t netId, std::vector<std::string> &servers,
                                            std::vector<std::string> &domains, uint16_t &baseTimeoutMsec,
                                            uint8_t &retryCount)
{
    NETMGR_LOG_D("Get resolver config: netId[%{public}d]", netId);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->GetResolverConfig(netId, servers, domains, baseTimeoutMsec, retryCount);
}

int32_t NetsysController::CreateNetworkCache(uint16_t netId)
{
    NETMGR_LOG_D("create dns cache: netId[%{public}d]", netId);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->CreateNetworkCache(netId);
}

int32_t NetsysController::DestroyNetworkCache(uint16_t netId)
{
    NETMGR_LOG_D("Destroy dns cache: netId[%{public}d]", netId);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->DestroyNetworkCache(netId);
}

int32_t NetsysController::GetAddrInfo(const std::string &hostName, const std::string &serverName, const AddrInfo &hints,
                                      uint16_t netId, std::vector<AddrInfo> &res)
{
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NET_CONN_ERR_SERVICE_UPDATE_NET_LINK_INFO_FAIL;
    }
    return netsysService_->GetAddrInfo(hostName, serverName, hints, netId, res);
}

int32_t NetsysController::GetNetworkSharingTraffic(const std::string &downIface, const std::string &upIface,
                                                   nmd::NetworkSharingTraffic &traffic)
{
    NETMGR_LOG_D("NetsysController GetNetworkSharingTraffic");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->GetNetworkSharingTraffic(downIface, upIface, traffic);
}

int64_t NetsysController::GetCellularRxBytes()
{
    NETMGR_LOG_D("NetsysController GetCellularRxBytes");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->GetCellularRxBytes();
}

int64_t NetsysController::GetCellularTxBytes()
{
    NETMGR_LOG_D("NetsysController GetCellularTxBytes");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->GetCellularTxBytes();
}

int64_t NetsysController::GetAllRxBytes()
{
    NETMGR_LOG_D("NetsysController GetAllRxBytes");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->GetAllRxBytes();
}

int64_t NetsysController::GetAllTxBytes()
{
    NETMGR_LOG_D("NetsysController GetAllTxBytes");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->GetAllTxBytes();
}

int64_t NetsysController::GetUidRxBytes(uint32_t uid)
{
    NETMGR_LOG_D("NetsysController GetUidRxBytes");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->GetUidRxBytes(uid);
}

int64_t NetsysController::GetUidTxBytes(uint32_t uid)
{
    NETMGR_LOG_D("NetsysController GetUidTxBytes");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->GetUidTxBytes(uid);
}

int64_t NetsysController::GetUidOnIfaceRxBytes(uint32_t uid, const std::string &interfaceName)
{
    NETMGR_LOG_D("NetsysController GetUidOnIfaceRxBytes");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->GetUidOnIfaceRxBytes(uid, interfaceName);
}

int64_t NetsysController::GetUidOnIfaceTxBytes(uint32_t uid, const std::string &interfaceName)
{
    NETMGR_LOG_D("NetsysController GetUidOnIfaceTxBytes");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->GetUidOnIfaceTxBytes(uid, interfaceName);
}

int64_t NetsysController::GetIfaceRxBytes(const std::string &interfaceName)
{
    NETMGR_LOG_D("NetsysController GetIfaceRxBytes");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->GetIfaceRxBytes(interfaceName);
}

int64_t NetsysController::GetIfaceTxBytes(const std::string &interfaceName)
{
    NETMGR_LOG_D("NetsysController GetIfaceTxBytes");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->GetIfaceTxBytes(interfaceName);
}

std::vector<std::string> NetsysController::InterfaceGetList()
{
    NETMGR_LOG_D("NetsysController InterfaceGetList");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return {};
    }
    return netsysService_->InterfaceGetList();
}

std::vector<std::string> NetsysController::UidGetList()
{
    NETMGR_LOG_D("NetsysController UidGetList");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return {};
    }
    return netsysService_->UidGetList();
}

int64_t NetsysController::GetIfaceRxPackets(const std::string &interfaceName)
{
    NETMGR_LOG_D("NetsysController GetIfaceRxPackets");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->GetIfaceRxPackets(interfaceName);
}

int64_t NetsysController::GetIfaceTxPackets(const std::string &interfaceName)
{
    NETMGR_LOG_D("NetsysController GetIfaceTxPackets");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->GetIfaceTxPackets(interfaceName);
}

int32_t NetsysController::SetDefaultNetWork(int32_t netId)
{
    NETMGR_LOG_D("Set DefaultNetWork: netId[%{public}d]", netId);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->SetDefaultNetWork(netId);
}

int32_t NetsysController::ClearDefaultNetWorkNetId()
{
    NETMGR_LOG_D("ClearDefaultNetWorkNetId");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->ClearDefaultNetWorkNetId();
}

int32_t NetsysController::BindSocket(int32_t socketFd, uint32_t netId)
{
    NETMGR_LOG_D("NetsysController::BindSocket: netId = [%{public}u]", netId);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->BindSocket(socketFd, netId);
}

int32_t NetsysController::IpEnableForwarding(const std::string &requestor)
{
    NETMGR_LOG_D("IpEnableForwarding: requestor[%{public}s]", requestor.c_str());
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->IpEnableForwarding(requestor);
}

int32_t NetsysController::IpDisableForwarding(const std::string &requestor)
{
    NETMGR_LOG_D("IpDisableForwarding: requestor[%{public}s]", requestor.c_str());
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->IpDisableForwarding(requestor);
}

int32_t NetsysController::EnableNat(const std::string &downstreamIface, const std::string &upstreamIface)
{
    NETMGR_LOG_D("MockNetsysNativeClient EnableNat: intIface[%{public}s] intIface[%{public}s]", downstreamIface.c_str(),
                 upstreamIface.c_str());
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->EnableNat(downstreamIface, upstreamIface);
}

int32_t NetsysController::DisableNat(const std::string &downstreamIface, const std::string &upstreamIface)
{
    NETMGR_LOG_D("MockNetsysNativeClient DisableNat: intIface[%{public}s] intIface[%{public}s]",
                 downstreamIface.c_str(), upstreamIface.c_str());
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->DisableNat(downstreamIface, upstreamIface);
}

int32_t NetsysController::IpfwdAddInterfaceForward(const std::string &fromIface, const std::string &toIface)
{
    NETMGR_LOG_D("IpfwdAddInterfaceForward: fromIface[%{public}s], toIface[%{public}s]", fromIface.c_str(),
                 toIface.c_str());
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->IpfwdAddInterfaceForward(fromIface, toIface);
}

int32_t NetsysController::IpfwdRemoveInterfaceForward(const std::string &fromIface, const std::string &toIface)
{
    NETMGR_LOG_D("IpfwdRemoveInterfaceForward: fromIface[%{public}s], toIface[%{public}s]", fromIface.c_str(),
                 toIface.c_str());
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->IpfwdRemoveInterfaceForward(fromIface, toIface);
}

int32_t NetsysController::ShareDnsSet(uint16_t netId)
{
    NETMGR_LOG_D("ShareDnsSet: netId[%{public}d]", netId);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->ShareDnsSet(netId);
}

int32_t NetsysController::StartDnsProxyListen()
{
    NETMGR_LOG_D("NetsysController::StartDnsProxyListen");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->StartDnsProxyListen();
}

int32_t NetsysController::StopDnsProxyListen()
{
    NETMGR_LOG_D("NetsysController::StopDnsProxyListen");
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->StopDnsProxyListen();
}

int32_t NetsysController::RegisterNetsysNotifyCallback(const NetsysNotifyCallback &callback)
{
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->RegisterNetsysNotifyCallback(callback);
}

int32_t NetsysController::BindNetworkServiceVpn(int32_t socketFd)
{
    NETMGR_LOG_D("NetsysController::BindNetworkServiceVpn: socketFd[%{public}d]", socketFd);
    if (socketFd <= 0) {
        NETMGR_LOG_E("socketFd is null");
        return NETSYS_ERR_VPN;
    }
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->BindNetworkServiceVpn(socketFd);
}

int32_t NetsysController::EnableVirtualNetIfaceCard(int32_t socketFd, struct ifreq &ifRequest, int32_t &ifaceFd)
{
    NETMGR_LOG_D("NetsysController::EnableVirtualNetIfaceCard: socketFd[%{public}d]", socketFd);
    if (socketFd <= 0) {
        NETMGR_LOG_E("socketFd is null");
        return NETSYS_ERR_VPN;
    }
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->EnableVirtualNetIfaceCard(socketFd, ifRequest, ifaceFd);
}

int32_t NetsysController::SetIpAddress(int32_t socketFd, const std::string &ipAddress, int32_t prefixLen,
                                       struct ifreq &ifRequest)
{
    NETMGR_LOG_D("NetsysController::set addr");
    if ((socketFd <= 0) || (ipAddress.length() == 0) || (ipAddress.length() > MAX_IPV4_ADDRESS_LEN) ||
        (prefixLen <= 0) || (prefixLen > MAX_IPV4_ADDRESS_LEN)) {
        NETMGR_LOG_E(
            "The paramemters of SetIpAddress is failed, socketFd[%{public}d], "
            "ipAddress[%{public}s], prefixLen[%{public}d].",
            socketFd, ToAnonymousIp(ipAddress).c_str(), prefixLen);
        return NETSYS_ERR_VPN;
    }
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->SetIpAddress(socketFd, ipAddress, prefixLen, ifRequest);
}

int32_t NetsysController::SetBlocking(int32_t ifaceFd, bool isBlock)
{
    NETMGR_LOG_D("NetsysController::SetBlocking: ifaceFd[%{public}d], isBlock[%{public}d]", ifaceFd, isBlock);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->SetBlocking(ifaceFd, isBlock);
}

int32_t NetsysController::StartDhcpClient(const std::string &iface, bool bIpv6)
{
    NETMGR_LOG_D("NetsysController::StartDhcpClient: iface[%{public}s], bIpv6[%{public}d]", iface.c_str(), bIpv6);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->StartDhcpClient(iface, bIpv6);
}

int32_t NetsysController::StopDhcpClient(const std::string &iface, bool bIpv6)
{
    NETMGR_LOG_D("NetsysController::SetBlocking: iface[%{public}s], bIpv6[%{public}d]", iface.c_str(), bIpv6);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->StopDhcpClient(iface, bIpv6);
}

int32_t NetsysController::RegisterCallback(sptr<NetsysControllerCallback> callback)
{
    NETMGR_LOG_D("NetsysController::RegisterCallback");
    return netsysService_->RegisterCallback(callback);
}

int32_t NetsysController::StartDhcpService(const std::string &iface, const std::string &ipv4addr)
{
    NETMGR_LOG_D("NetsysController::StartDhcpService: iface[%{public}s], ipv4addr[%{public}s]", iface.c_str(),
                 ToAnonymousIp(ipv4addr).c_str());
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->StartDhcpService(iface, ipv4addr);
}

int32_t NetsysController::StopDhcpService(const std::string &iface)
{
    NETMGR_LOG_D("NetsysController::StopDhcpService: ifaceFd[%{public}s]", iface.c_str());
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->StopDhcpService(iface);
}

int32_t NetsysController::BandwidthEnableDataSaver(bool enable)
{
    NETMGR_LOG_D("NetsysController::BandwidthEnableDataSaver: enable=%{public}d", enable);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->BandwidthEnableDataSaver(enable);
}

int32_t NetsysController::BandwidthSetIfaceQuota(const std::string &ifName, int64_t bytes)
{
    NETMGR_LOG_D("NetsysController::BandwidthSetIfaceQuota: ifName=%{public}s", ifName.c_str());
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->BandwidthSetIfaceQuota(ifName, bytes);
}

int32_t NetsysController::BandwidthRemoveIfaceQuota(const std::string &ifName)
{
    NETMGR_LOG_D("NetsysController::BandwidthRemoveIfaceQuota: ifName=%{public}s", ifName.c_str());
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->BandwidthRemoveIfaceQuota(ifName);
}

int32_t NetsysController::BandwidthAddDeniedList(uint32_t uid)
{
    NETMGR_LOG_D("NetsysController::BandwidthAddDeniedList: uid=%{public}d", uid);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->BandwidthAddDeniedList(uid);
}

int32_t NetsysController::BandwidthRemoveDeniedList(uint32_t uid)
{
    NETMGR_LOG_D("NetsysController::BandwidthRemoveDeniedList: uid=%{public}d", uid);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->BandwidthRemoveDeniedList(uid);
}

int32_t NetsysController::BandwidthAddAllowedList(uint32_t uid)
{
    NETMGR_LOG_D("NetsysController::BandwidthAddAllowedList: uid=%{public}d", uid);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->BandwidthAddAllowedList(uid);
}

int32_t NetsysController::BandwidthRemoveAllowedList(uint32_t uid)
{
    NETMGR_LOG_D("NetsysController::BandwidthRemoveAllowedList: uid=%{public}d", uid);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->BandwidthRemoveAllowedList(uid);
}

int32_t NetsysController::FirewallSetUidsAllowedListChain(uint32_t chain, const std::vector<uint32_t> &uids)
{
    NETMGR_LOG_D("NetsysController::FirewallSetUidsAllowedListChain: chain=%{public}d", chain);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->FirewallSetUidsAllowedListChain(chain, uids);
}

int32_t NetsysController::FirewallSetUidsDeniedListChain(uint32_t chain, const std::vector<uint32_t> &uids)
{
    NETMGR_LOG_D("NetsysController::FirewallSetUidsDeniedListChain: chain=%{public}d", chain);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->FirewallSetUidsDeniedListChain(chain, uids);
}

int32_t NetsysController::FirewallEnableChain(uint32_t chain, bool enable)
{
    NETMGR_LOG_D("NetsysController::FirewallEnableChain: chain=%{public}d, enable=%{public}d", chain, enable);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->FirewallEnableChain(chain, enable);
}

int32_t NetsysController::FirewallSetUidRule(uint32_t chain, uint32_t uid, uint32_t firewallRule)
{
    NETMGR_LOG_D("NetsysController::FirewallSetUidRule: chain=%{public}d,uid=%{public}d,firewallRule=%{public}d", chain,
                 uid, firewallRule);
    if (netsysService_ == nullptr) {
        NETMGR_LOG_E("netsysService_ is null");
        return NETSYS_NETSYSSERVICE_NULL;
    }
    return netsysService_->FirewallSetUidRule(chain, uid, firewallRule);
}

void NetsysController::FreeAddrInfo(addrinfo *aihead)
{
    addrinfo *tmpNext = nullptr;
    for (addrinfo *tmp = aihead; tmp != nullptr;) {
        if (tmp->ai_addr != nullptr) {
            free(tmp->ai_addr);
        }
        if (tmp->ai_canonname != nullptr) {
            free(tmp->ai_canonname);
        }
        tmpNext = tmp->ai_next;
        free(tmp);
        tmp = tmpNext;
    }
}
} // namespace NetManagerStandard
} // namespace OHOS
