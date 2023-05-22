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

#include "common_event_support.h"

#include "broadcast_manager.h"
#include "event_report.h"
#include "netmanager_base_common_utils.h"
#include "network.h"
#include "netsys_controller.h"
#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "route_utils.h"
#include "securec.h"

using namespace OHOS::NetManagerStandard::CommonUtils;

namespace OHOS {
namespace NetManagerStandard {
namespace {
// hisysevent error messgae
constexpr const char *ERROR_MSG_CREATE_PHYSICAL_NETWORK_FAILED = "Create physical network failed, net id:";
constexpr const char *ERROR_MSG_ADD_NET_INTERFACE_FAILED = "Add network interface failed";
constexpr const char *ERROR_MSG_REMOVE_NET_INTERFACE_FAILED = "Remove network interface failed";
constexpr const char *ERROR_MSG_DELETE_NET_IP_ADDR_FAILED = "Delete network ip address failed";
constexpr const char *ERROR_MSG_ADD_NET_IP_ADDR_FAILED = "Add network ip address failed";
constexpr const char *ERROR_MSG_REMOVE_NET_ROUTES_FAILED = "Remove network routes failed";
constexpr const char *ERROR_MSG_ADD_NET_ROUTES_FAILED = "Add network routes failed";
constexpr const char *ERROR_MSG_UPDATE_NET_ROUTES_FAILED = "Update netlink routes failed,routes list is empty";
constexpr const char *ERROR_MSG_SET_NET_RESOLVER_FAILED = "Set network resolver config failed";
constexpr const char *ERROR_MSG_UPDATE_NET_DNSES_FAILED = "Update netlink dns failed,dns list is empty";
constexpr const char *ERROR_MSG_SET_NET_MTU_FAILED = "Set netlink interface mtu failed";
constexpr const char *ERROR_MSG_SET_DEFAULT_NETWORK_FAILED = "Set default network failed";
constexpr const char *ERROR_MSG_CLEAR_DEFAULT_NETWORK_FAILED = "Clear default network failed";
constexpr const char *LOCAL_ROUTE_NEXT_HOP = "0.0.0.0";
} // namespace

Network::Network(int32_t netId, uint32_t supplierId, const NetDetectionHandler &handler, NetBearType bearerType,
                 const std::shared_ptr<NetConnEventHandler> &eventHandler)
    : netId_(netId),
      supplierId_(supplierId),
      netCallback_(handler),
      netSupplierType_(bearerType),
      eventHandler_(eventHandler)
{
}

int32_t Network::GetNetId() const
{
    return netId_;
}

bool Network::operator==(const Network &network) const
{
    return netId_ == network.netId_;
}

bool Network::UpdateBasicNetwork(bool isAvailable_)
{
    NETMGR_LOG_D("Enter UpdateBasicNetwork");
    if (isAvailable_) {
        return CreateBasicNetwork();
    } else {
        return ReleaseBasicNetwork();
    }
}

bool Network::CreateBasicNetwork()
{
    NETMGR_LOG_D("Enter CreateBasicNetwork");
    if (!isPhyNetCreated_) {
        NETMGR_LOG_D("Create physical network");
        // Create a physical network
        if (NetsysController::GetInstance().NetworkCreatePhysical(netId_, 0) != NETMANAGER_SUCCESS) {
            std::string errMsg = std::string(ERROR_MSG_CREATE_PHYSICAL_NETWORK_FAILED).append(std::to_string(netId_));
            SendSupplierFaultHiSysEvent(FAULT_CREATE_PHYSICAL_NETWORK_FAILED, errMsg);
        }
        NetsysController::GetInstance().CreateNetworkCache(netId_);
        isPhyNetCreated_ = true;
    }
    return true;
}

bool Network::ReleaseBasicNetwork()
{
    NETMGR_LOG_D("Enter ReleaseBasicNetwork");
    if (isPhyNetCreated_) {
        NETMGR_LOG_D("Destroy physical network");
        StopNetDetection();
        for (const auto &inetAddr : netLinkInfo_.netAddrList_) {
            int32_t prefixLen = inetAddr.prefixlen_;
            if (prefixLen == 0) {
                prefixLen = Ipv4PrefixLen(inetAddr.netMask_);
            }
            NetsysController::GetInstance().DelInterfaceAddress(netLinkInfo_.ifaceName_, inetAddr.address_, prefixLen);
        }
        NetsysController::GetInstance().NetworkRemoveInterface(netId_, netLinkInfo_.ifaceName_);
        NetsysController::GetInstance().NetworkDestroy(netId_);
        NetsysController::GetInstance().DestroyNetworkCache(netId_);
        netLinkInfo_.Initialize();
        isPhyNetCreated_ = false;
    }
    return true;
}

bool Network::UpdateNetLinkInfo(const NetLinkInfo &netLinkInfo)
{
    NETMGR_LOG_D("update net link information process");
    UpdateInterfaces(netLinkInfo);
    UpdateIpAddrs(netLinkInfo);
    UpdateRoutes(netLinkInfo);
    UpdateDns(netLinkInfo);
    UpdateMtu(netLinkInfo);
    netLinkInfo_ = netLinkInfo;
    StartNetDetection(false);
    return true;
}

NetLinkInfo Network::GetNetLinkInfo() const
{
    return netLinkInfo_;
}

void Network::UpdateInterfaces(const NetLinkInfo &netLinkInfo)
{
    NETMGR_LOG_D("Network UpdateInterfaces in.");
    if (netLinkInfo.ifaceName_ == netLinkInfo_.ifaceName_) {
        NETMGR_LOG_D("Network UpdateInterfaces out. same with before.");
        return;
    }

    int32_t ret = NETMANAGER_SUCCESS;
    // Call netsys to add and remove interface
    if (!netLinkInfo.ifaceName_.empty()) {
        ret = NetsysController::GetInstance().NetworkAddInterface(netId_, netLinkInfo.ifaceName_);
        if (ret != NETMANAGER_SUCCESS) {
            SendSupplierFaultHiSysEvent(FAULT_UPDATE_NETLINK_INFO_FAILED, ERROR_MSG_ADD_NET_INTERFACE_FAILED);
        }
    }
    if (!netLinkInfo_.ifaceName_.empty()) {
        ret = NetsysController::GetInstance().NetworkRemoveInterface(netId_, netLinkInfo_.ifaceName_);
        if (ret != NETMANAGER_SUCCESS) {
            SendSupplierFaultHiSysEvent(FAULT_UPDATE_NETLINK_INFO_FAILED, ERROR_MSG_REMOVE_NET_INTERFACE_FAILED);
        }
    }
    netLinkInfo_.ifaceName_ = netLinkInfo.ifaceName_;
    NETMGR_LOG_D("Network UpdateInterfaces out.");
}

void Network::UpdateIpAddrs(const NetLinkInfo &netLinkInfo)
{
    // netLinkInfo_ represents the old, netLinkInfo represents the new
    // Update: remove old Ips first, then add the new Ips
    NETMGR_LOG_D("UpdateIpAddrs, old ip addrs: ...");
    for (const auto &inetAddr : netLinkInfo_.netAddrList_) {
        int32_t prefixLen = inetAddr.prefixlen_;
        if (prefixLen == 0) {
            prefixLen = Ipv4PrefixLen(inetAddr.netMask_);
        }
        int32_t ret =
            NetsysController::GetInstance().DelInterfaceAddress(netLinkInfo_.ifaceName_, inetAddr.address_, prefixLen);
        if (ret != NETMANAGER_SUCCESS) {
            SendSupplierFaultHiSysEvent(FAULT_UPDATE_NETLINK_INFO_FAILED, ERROR_MSG_DELETE_NET_IP_ADDR_FAILED);
        }
    }

    NETMGR_LOG_D("UpdateIpAddrs, new ip addrs: ...");
    for (auto it = netLinkInfo.netAddrList_.begin(); it != netLinkInfo.netAddrList_.end(); ++it) {
        const struct INetAddr &inetAddr = *it;
        int32_t prefixLen = inetAddr.prefixlen_;
        if (prefixLen == 0) {
            prefixLen = Ipv4PrefixLen(inetAddr.netMask_);
        }
        int32_t ret =
            NetsysController::GetInstance().AddInterfaceAddress(netLinkInfo.ifaceName_, inetAddr.address_, prefixLen);
        if (ret != NETMANAGER_SUCCESS) {
            SendSupplierFaultHiSysEvent(FAULT_UPDATE_NETLINK_INFO_FAILED, ERROR_MSG_ADD_NET_IP_ADDR_FAILED);
        }
    }
    NETMGR_LOG_D("Network UpdateIpAddrs out.");
}

void Network::UpdateRoutes(const NetLinkInfo &netLinkInfo)
{
    // netLinkInfo_ contains the old routes info, netLinkInfo contains the new routes info
    // Update: remove old routes first, then add the new routes
    NETMGR_LOG_D("UpdateRoutes, old routes: [%{public}s]", netLinkInfo_.ToStringRoute("").c_str());
    for (const auto &route : netLinkInfo_.routeList_) {
        std::string destAddress = route.destination_.address_ + "/" + std::to_string(route.destination_.prefixlen_);
        int32_t ret = NetsysController::GetInstance().NetworkRemoveRoute(netId_, route.iface_, destAddress,
                                                                         route.gateway_.address_);
        if (route.destination_.address_ != LOCAL_ROUTE_NEXT_HOP) {
            ret |= NetsysController::GetInstance().NetworkRemoveRoute(LOCAL_NET_ID, route.iface_, destAddress,
                                                                      LOCAL_ROUTE_NEXT_HOP);
        }
        if (ret != NETMANAGER_SUCCESS) {
            SendSupplierFaultHiSysEvent(FAULT_UPDATE_NETLINK_INFO_FAILED, ERROR_MSG_REMOVE_NET_ROUTES_FAILED);
        }
    }

    NETMGR_LOG_D("UpdateRoutes, new routes: [%{public}s]", netLinkInfo.ToStringRoute("").c_str());
    for (const auto &route : netLinkInfo.routeList_) {
        std::string destAddress = route.destination_.address_ + "/" + std::to_string(route.destination_.prefixlen_);
        int32_t ret =
            NetsysController::GetInstance().NetworkAddRoute(netId_, route.iface_, destAddress, route.gateway_.address_);
        int32_t result = 0;
        if (route.destination_.address_ != LOCAL_ROUTE_NEXT_HOP) {
            result = NetsysController::GetInstance().NetworkAddRoute(LOCAL_NET_ID, route.iface_, destAddress,
                                                                     LOCAL_ROUTE_NEXT_HOP);
        }
        if (ret != NETMANAGER_SUCCESS || result != NETMANAGER_SUCCESS) {
            SendSupplierFaultHiSysEvent(FAULT_UPDATE_NETLINK_INFO_FAILED, ERROR_MSG_ADD_NET_ROUTES_FAILED);
        }
    }
    NETMGR_LOG_D("Network UpdateRoutes out.");
    if (netLinkInfo.routeList_.empty()) {
        SendSupplierFaultHiSysEvent(FAULT_UPDATE_NETLINK_INFO_FAILED, ERROR_MSG_UPDATE_NET_ROUTES_FAILED);
    }
}

void Network::UpdateDns(const NetLinkInfo &netLinkInfo)
{
    NETMGR_LOG_D("Network UpdateDns in.");
    std::vector<std::string> servers;
    std::vector<std::string> domains;
    for (const auto &dns : netLinkInfo.dnsList_) {
        servers.emplace_back(dns.address_);
        domains.emplace_back(dns.hostName_);
    }
    // Call netsys to set dns, use default timeout and retry
    int32_t ret = NetsysController::GetInstance().SetResolverConfig(netId_, 0, 0, servers, domains);
    if (ret != NETMANAGER_SUCCESS) {
        SendSupplierFaultHiSysEvent(FAULT_UPDATE_NETLINK_INFO_FAILED, ERROR_MSG_SET_NET_RESOLVER_FAILED);
    }
    NETMGR_LOG_D("Network UpdateDns out.");
    if (netLinkInfo.dnsList_.empty()) {
        SendSupplierFaultHiSysEvent(FAULT_UPDATE_NETLINK_INFO_FAILED, ERROR_MSG_UPDATE_NET_DNSES_FAILED);
    }
}

void Network::UpdateMtu(const NetLinkInfo &netLinkInfo)
{
    NETMGR_LOG_D("Network UpdateMtu in.");
    if (netLinkInfo.mtu_ == netLinkInfo_.mtu_) {
        NETMGR_LOG_D("Network UpdateMtu out. same with before.");
        return;
    }

    int32_t ret = NetsysController::GetInstance().SetInterfaceMtu(netLinkInfo.ifaceName_, netLinkInfo.mtu_);
    if (ret != NETMANAGER_SUCCESS) {
        SendSupplierFaultHiSysEvent(FAULT_UPDATE_NETLINK_INFO_FAILED, ERROR_MSG_SET_NET_MTU_FAILED);
    }
    NETMGR_LOG_D("Network UpdateMtu out.");
}

void Network::RegisterNetDetectionCallback(const sptr<INetDetectionCallback> &callback)
{
    NETMGR_LOG_D("Enter RegisterNetDetectionCallback");
    if (callback == nullptr) {
        NETMGR_LOG_E("The parameter callback is null");
        return;
    }

    for (const auto &iter : netDetectionRetCallback_) {
        if (callback->AsObject().GetRefPtr() == iter->AsObject().GetRefPtr()) {
            NETMGR_LOG_D("netDetectionRetCallback_ had this callback");
            return;
        }
    }

    netDetectionRetCallback_.emplace_back(callback);
}

int32_t Network::UnRegisterNetDetectionCallback(const sptr<INetDetectionCallback> &callback)
{
    NETMGR_LOG_D("Enter UnRegisterNetDetectionCallback");
    if (callback == nullptr) {
        NETMGR_LOG_E("The parameter of callback is null");
        return NETMANAGER_ERR_LOCAL_PTR_NULL;
    }

    for (auto iter = netDetectionRetCallback_.begin(); iter != netDetectionRetCallback_.end(); ++iter) {
        if (callback->AsObject().GetRefPtr() == (*iter)->AsObject().GetRefPtr()) {
            netDetectionRetCallback_.erase(iter);
            return NETMANAGER_SUCCESS;
        }
    }

    return NETMANAGER_SUCCESS;
}

void Network::StartNetDetection(bool needReport)
{
    NETMGR_LOG_D("Enter Network::StartNetDetection");
    if (needReport) {
        StopNetDetection();
    }
    InitNetMonitor();
}

void Network::StopNetDetection()
{
    NETMGR_LOG_D("Enter Network::StopNetDetection");
    if (netMonitor_ != nullptr) {
        netMonitor_->Stop();
        netMonitor_ = nullptr;
    }
}

void Network::InitNetMonitor()
{
    if (netMonitor_ == nullptr) {
        std::weak_ptr<INetMonitorCallback> monitorCallback = shared_from_this();
        netMonitor_ = std::make_shared<NetMonitor>(netId_, monitorCallback);
        if (netMonitor_ == nullptr) {
            NETMGR_LOG_E("new NetMonitor failed,netMonitor_ is null!");
            return;
        }
    }
    netMonitor_->Start();
}

void Network::HandleNetMonitorResult(NetDetectionStatus netDetectionState, const std::string &urlRedirect)
{
    NETMGR_LOG_D("HandleNetMonitorResult, netDetectionState[%{public}d]", netDetectionState);
    NotifyNetDetectionResult(NetDetectionResultConvert(static_cast<int32_t>(netDetectionState)), urlRedirect);
    if (netCallback_ && (detectResult_ != netDetectionState)) {
        detectResult_ = netDetectionState;
        netCallback_(supplierId_, netDetectionState == VERIFICATION_STATE);
    }
}

void Network::NotifyNetDetectionResult(NetDetectionResultCode detectionResult, const std::string &urlRedirect)
{
    for (const auto &callback : netDetectionRetCallback_) {
        NETMGR_LOG_D("start callback!");
        if (callback) {
            callback->OnNetDetectionResultChanged(detectionResult, urlRedirect);
        }
    }
}

NetDetectionResultCode Network::NetDetectionResultConvert(int32_t internalRet)
{
    switch (internalRet) {
        case static_cast<int32_t>(INVALID_DETECTION_STATE):
            return NET_DETECTION_FAIL;
        case static_cast<int32_t>(VERIFICATION_STATE):
            return NET_DETECTION_SUCCESS;
        case static_cast<int32_t>(CAPTIVE_PORTAL_STATE):
            return NET_DETECTION_CAPTIVE_PORTAL;
        default:
            break;
    }
    return NET_DETECTION_FAIL;
}

void Network::SetDefaultNetWork()
{
    int32_t ret = NetsysController::GetInstance().SetDefaultNetWork(netId_);
    if (ret != NETMANAGER_SUCCESS) {
        SendSupplierFaultHiSysEvent(FAULT_SET_DEFAULT_NETWORK_FAILED, ERROR_MSG_SET_DEFAULT_NETWORK_FAILED);
    }
}

void Network::ClearDefaultNetWorkNetId()
{
    int32_t ret = NetsysController::GetInstance().ClearDefaultNetWorkNetId();
    if (ret != NETMANAGER_SUCCESS) {
        SendSupplierFaultHiSysEvent(FAULT_CLEAR_DEFAULT_NETWORK_FAILED, ERROR_MSG_CLEAR_DEFAULT_NETWORK_FAILED);
    }
}

bool Network::IsConnecting() const
{
    return state_ == NET_CONN_STATE_CONNECTING;
}

bool Network::IsConnected() const
{
    return state_ == NET_CONN_STATE_CONNECTED;
}

void Network::UpdateNetConnState(NetConnState netConnState)
{
    if (state_ == netConnState) {
        NETMGR_LOG_E("Ignore same network state changed.");
        return;
    }
    NetConnState oldState = state_;
    switch (netConnState) {
        case NET_CONN_STATE_IDLE:
        case NET_CONN_STATE_CONNECTING:
        case NET_CONN_STATE_CONNECTED:
        case NET_CONN_STATE_DISCONNECTING:
            state_ = netConnState;
            break;
        case NET_CONN_STATE_DISCONNECTED:
            state_ = netConnState;
            ResetNetlinkInfo();
            break;
        default:
            state_ = NET_CONN_STATE_UNKNOWN;
            break;
    }

    SendConnectionChangedBroadcast(netConnState);
    NETMGR_LOG_D("Network[%{public}d] state changed, from [%{public}d] to [%{public}d]", netId_, oldState, state_);
}

void Network::SendConnectionChangedBroadcast(const NetConnState &netConnState) const
{
    BroadcastInfo info;
    info.action = EventFwk::CommonEventSupport::COMMON_EVENT_CONNECTIVITY_CHANGE;
    info.data = "Net Manager Connection State Changed";
    info.code = static_cast<int32_t>(netConnState);
    info.ordered = true;
    std::map<std::string, int32_t> param = {{"NetType", static_cast<int32_t>(netSupplierType_)}};
    DelayedSingleton<BroadcastManager>::GetInstance()->SendBroadcast(info, param);
}

void Network::SendSupplierFaultHiSysEvent(NetConnSupplerFault errorType, const std::string &errMsg)
{
    struct EventInfo eventInfo = {.netlinkInfo = netLinkInfo_.ToString(" "),
                                  .supplierId = static_cast<int32_t>(supplierId_),
                                  .errorType = static_cast<int32_t>(errorType),
                                  .errorMsg = errMsg};
    EventReport::SendSupplierFaultEvent(eventInfo);
}

void Network::ResetNetlinkInfo()
{
    netLinkInfo_.Initialize();
}

void Network::OnHandleNetMonitorResult(NetDetectionStatus netDetectionState, const std::string &urlRedirect)
{
    if (eventHandler_) {
        eventHandler_->PostAsyncTask(
            [netDetectionState, &urlRedirect, this]() { this->HandleNetMonitorResult(netDetectionState, urlRedirect); },
            0);
    }
}
} // namespace NetManagerStandard
} // namespace OHOS
