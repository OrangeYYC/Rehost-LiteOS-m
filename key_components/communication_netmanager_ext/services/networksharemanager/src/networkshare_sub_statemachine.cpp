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

#include "networkshare_sub_statemachine.h"

#include "net_manager_constants.h"
#include "net_manager_ext_constants.h"
#include "netmgr_ext_log_wrapper.h"
#include "netsys_controller.h"
#include "route_utils.h"

namespace OHOS {
namespace NetManagerStandard {
static constexpr const char *NEXT_HOT = "0.0.0.0";
static constexpr const char *ERROR_MSG_CONFIG_FORWARD = "Config Forward failed";
static constexpr const char *ERROR_MSG_ADD_ROUTE_STRATEGY = "Add Route Strategy failed";
static constexpr const char *ERROR_MSG_ADD_ROUTE_RULE = "Add Route Rule failed";
static constexpr const char *ERROR_MSG_REMOVE_ROUTE_RULE = "Remove Route Rule failed";
static constexpr const char *EMPTY_UPSTREAM_IFACENAME = "";
static constexpr int32_t IP_V4 = 0;

NetworkShareSubStateMachine::NetworkShareSubStateMachine(
    const std::string &ifaceName, const SharingIfaceType &interfaceType,
    const std::shared_ptr<NetworkShareConfiguration> &configuration)
    : ifaceName_(ifaceName), netShareType_(interfaceType), configuration_(configuration)
{
    CreateInitStateTable();
    CreateSharedStateTable();
}

void NetworkShareSubStateMachine::CreateInitStateTable()
{
    SubSmStateTable temp;
    temp.event_ = CMD_NETSHARE_REQUESTED;
    temp.curState_ = SUBSTATE_INIT;
    temp.func_ = &NetworkShareSubStateMachine::HandleInitSharingRequest;
    temp.nextState_ = SUBSTATE_SHARED;
    stateTable_.push_back(temp);

    temp.event_ = CMD_INTERFACE_DOWN;
    temp.curState_ = SUBSTATE_INIT;
    temp.func_ = &NetworkShareSubStateMachine::HandleInitInterfaceDown;
    temp.nextState_ = SUBSTATE_UNAVAILABLE;
    stateTable_.push_back(temp);
}

void NetworkShareSubStateMachine::CreateSharedStateTable()
{
    SubSmStateTable temp;
    temp.event_ = CMD_NETSHARE_UNREQUESTED;
    temp.curState_ = SUBSTATE_SHARED;
    temp.func_ = &NetworkShareSubStateMachine::HandleSharedUnrequest;
    temp.nextState_ = SUBSTATE_INIT;
    stateTable_.push_back(temp);

    temp.event_ = CMD_INTERFACE_DOWN;
    temp.curState_ = SUBSTATE_SHARED;
    temp.func_ = &NetworkShareSubStateMachine::HandleSharedInterfaceDown;
    temp.nextState_ = SUBSTATE_UNAVAILABLE;
    stateTable_.push_back(temp);

    temp.event_ = CMD_NETSHARE_CONNECTION_CHANGED;
    temp.curState_ = SUBSTATE_SHARED;
    temp.func_ = &NetworkShareSubStateMachine::HandleSharedConnectionChange;
    temp.nextState_ = NO_NEXT_STATE;
    stateTable_.push_back(temp);

    temp.event_ = CMD_IP_FORWARDING_ENABLE_ERROR;
    temp.curState_ = SUBSTATE_SHARED;
    temp.func_ = &NetworkShareSubStateMachine::HandleSharedErrors;
    temp.nextState_ = SUBSTATE_INIT;
    stateTable_.push_back(temp);

    temp.event_ = CMD_IP_FORWARDING_DISABLE_ERROR;
    temp.curState_ = SUBSTATE_SHARED;
    temp.func_ = &NetworkShareSubStateMachine::HandleSharedErrors;
    temp.nextState_ = SUBSTATE_INIT;
    stateTable_.push_back(temp);

    temp.event_ = CMD_START_SHARING_ERROR;
    temp.curState_ = SUBSTATE_SHARED;
    temp.func_ = &NetworkShareSubStateMachine::HandleSharedErrors;
    temp.nextState_ = SUBSTATE_INIT;
    stateTable_.push_back(temp);

    temp.event_ = CMD_STOP_SHARING_ERROR;
    temp.curState_ = SUBSTATE_SHARED;
    temp.func_ = &NetworkShareSubStateMachine::HandleSharedErrors;
    temp.nextState_ = SUBSTATE_INIT;
    stateTable_.push_back(temp);

    temp.event_ = CMD_SET_DNS_FORWARDERS_ERROR;
    temp.curState_ = SUBSTATE_SHARED;
    temp.func_ = &NetworkShareSubStateMachine::HandleSharedErrors;
    temp.nextState_ = SUBSTATE_INIT;
    stateTable_.push_back(temp);
}

void NetworkShareSubStateMachine::SubSmStateSwitch(int newState)
{
    int oldState = curState_;
    curState_ = newState;
    NETMGR_EXT_LOG_I("Sub SM from[%{public}d] to[%{public}d].", oldState, newState);

    if (oldState == SUBSTATE_INIT) {
        InitStateExit();
    } else if (oldState == SUBSTATE_SHARED) {
        SharedStateExit();
    } else if (oldState == SUBSTATE_UNAVAILABLE) {
        UnavailableStateExit();
    } else {
        NETMGR_EXT_LOG_E("oldState is unknow type value.");
    }

    if (newState == SUBSTATE_INIT) {
        InitStateEnter();
    } else if (newState == SUBSTATE_SHARED) {
        SharedStateEnter();
    } else if (newState == SUBSTATE_UNAVAILABLE) {
        UnavailableStateEnter();
    } else {
        NETMGR_EXT_LOG_E("newState is unknow type value.");
    }
}

void NetworkShareSubStateMachine::SubSmEventHandle(int eventId, const std::any &messageObj)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    int nextState = NO_NEXT_STATE;
    int (NetworkShareSubStateMachine::*eventFunc)(const std::any &messageObj) = nullptr;
    for (const auto &iterState : stateTable_) {
        if ((eventId == iterState.event_) && (curState_ == iterState.curState_)) {
            eventFunc = iterState.func_;
            nextState = iterState.nextState_;
            break;
        }
    }
    if (eventFunc == nullptr) {
        NETMGR_EXT_LOG_W("SubSM currentstate[%{public}d] eventId[%{public}d] is not matched.", curState_, eventId);
        return;
    }
    (this->*eventFunc)(messageObj);
    if (nextState >= SUBSTATE_INIT && nextState < SUBSTATE_MAX) {
        SubSmStateSwitch(nextState);
    }

    NETMGR_EXT_LOG_I("SubSM eventId[%{public}d] handle successfull.", eventId);
}

void NetworkShareSubStateMachine::GetDownIfaceName(std::string &downIface)
{
    downIface = ifaceName_;
}

void NetworkShareSubStateMachine::GetUpIfaceName(std::string &upIface)
{
    upIface = upstreamIfaceName_;
}

void NetworkShareSubStateMachine::InitStateEnter()
{
    if (trackerCallback_ == nullptr) {
        NETMGR_EXT_LOG_E("Enter Sub StateMachine Init State error, trackerCallback_ is null.");
        return;
    }
    NETMGR_EXT_LOG_I("Enter Sub StateMachine[%{public}s] Init State.", ifaceName_.c_str());
    trackerCallback_->OnUpdateInterfaceState(shared_from_this(), SUB_SM_STATE_AVAILABLE, lastError_);
}

void NetworkShareSubStateMachine::InitStateExit()
{
    NETMGR_EXT_LOG_I("Exit Sub StateMachine[%{public}s] Init State.", ifaceName_.c_str());
}

int NetworkShareSubStateMachine::HandleInitSharingRequest(const std::any &messageObj)
{
    (void)messageObj;
    lastError_ = NETMANAGER_EXT_SUCCESS;
    return NETMANAGER_EXT_SUCCESS;
}

int NetworkShareSubStateMachine::HandleInitInterfaceDown(const std::any &messageObj)
{
    (void)messageObj;
    return NETMANAGER_EXT_SUCCESS;
}

void NetworkShareSubStateMachine::SharedStateEnter()
{
    NETMGR_EXT_LOG_I("Enter Sub StateMachine[%{public}s] Shared State.", ifaceName_.c_str());
    if (!ConfigureShareDhcp(true)) {
        lastError_ = NETWORKSHARE_ERROR_IFACE_CFG_ERROR;
        NETMGR_EXT_LOG_E("Enter sub StateMachine[%{public}s] Shared State configIpv4 error.", ifaceName_.c_str());
    }
    if (lastError_ != NETMANAGER_EXT_SUCCESS) {
        SubSmStateSwitch(SUBSTATE_INIT);
        return;
    }
    if (trackerCallback_ == nullptr) {
        NETMGR_EXT_LOG_E("Enter Sub StateMachine Shared State error, trackerCallback_ is null.");
        return;
    }
    trackerCallback_->OnUpdateInterfaceState(shared_from_this(), SUB_SM_STATE_SHARED, lastError_);
}

void NetworkShareSubStateMachine::SharedStateExit()
{
    NETMGR_EXT_LOG_I("Exit Sub StateMachine[%{public}s] Shared State.", ifaceName_.c_str());
    CleanupUpstreamInterface();
    ConfigureShareDhcp(false);
}

int NetworkShareSubStateMachine::HandleSharedUnrequest(const std::any &messageObj)
{
    (void)messageObj;
    return NETMANAGER_EXT_SUCCESS;
}

int NetworkShareSubStateMachine::HandleSharedInterfaceDown(const std::any &messageObj)
{
    (void)messageObj;
    return NETMANAGER_EXT_SUCCESS;
}

int NetworkShareSubStateMachine::HandleSharedConnectionChange(const std::any &messageObj)
{
    std::shared_ptr<UpstreamNetworkInfo> upstreamNetInfo =
        std::any_cast<std::shared_ptr<UpstreamNetworkInfo>>(messageObj);
    if (upstreamNetInfo == nullptr) {
        NETMGR_EXT_LOG_I("Sub StateMachine[%{public}s] upstreamNetInfo is null, need clean.", ifaceName_.c_str());
        CleanupUpstreamInterface();
        upstreamIfaceName_ = EMPTY_UPSTREAM_IFACENAME;
        return NETMANAGER_EXT_SUCCESS;
    }
    HandleConnectionChanged(upstreamNetInfo);
    return NETMANAGER_EXT_SUCCESS;
}

int NetworkShareSubStateMachine::HandleSharedErrors(const std::any &messageObj)
{
    (void)messageObj;
    NETMGR_EXT_LOG_I("Sub StateMachine[%{public}s] SharedState has ERROR.", ifaceName_.c_str());
    lastError_ = NETWORKSHARE_ERROR_INTERNAL_ERROR;
    return NETMANAGER_EXT_SUCCESS;
}

void NetworkShareSubStateMachine::UnavailableStateEnter()
{
    NETMGR_EXT_LOG_I("Enter Sub StateMachine[%{public}s] Unavailable State.", ifaceName_.c_str());
    lastError_ = NETMANAGER_EXT_SUCCESS;
    if (trackerCallback_ == nullptr) {
        NETMGR_EXT_LOG_E("Enter Sub StateMachine Unavailable State error, trackerCallback_ is null.");
        return;
    }
    trackerCallback_->OnUpdateInterfaceState(shared_from_this(), SUB_SM_STATE_UNAVAILABLE, NETMANAGER_EXT_SUCCESS);
}

void NetworkShareSubStateMachine::UnavailableStateExit()
{
    NETMGR_EXT_LOG_I("Exit Sub StateMachine[%{public}s] Unavailable State.", ifaceName_.c_str());
}

void NetworkShareSubStateMachine::HandleConnectionChanged(const std::shared_ptr<UpstreamNetworkInfo> &upstreamNetInfo)
{
    if (upstreamNetInfo == nullptr) {
        return;
    }
    if (upstreamNetInfo->netLinkPro_ == nullptr) {
        NETMGR_EXT_LOG_E("Sub StateMachine[%{public}s] HandleConnectionChanged netLinkPro_ is null.",
                         ifaceName_.c_str());
        return;
    }
    if (!HasChangeUpstreamIfaceSet(upstreamNetInfo->netLinkPro_->ifaceName_)) {
        NETMGR_EXT_LOG_I("Sub StateMachine[%{public}s] HandleConnectionChanged Upstream Iface no change.",
                         ifaceName_.c_str());
        return;
    }

    CleanupUpstreamInterface();
    upstreamIfaceName_ = upstreamNetInfo->netLinkPro_->ifaceName_;

    HandleConnection();
    AddRoutesToLocalNetwork();
}

void NetworkShareSubStateMachine::HandleConnection()
{
    int32_t result = NetsysController::GetInstance().IpfwdAddInterfaceForward(ifaceName_, upstreamIfaceName_);
    if (result != NETSYS_SUCCESS) {
        NetworkShareHisysEvent::GetInstance().SendFaultEvent(
            netShareType_, NetworkShareEventOperator::OPERATION_CONFIG_FORWARD,
            NetworkShareEventErrorType::ERROR_CONFIG_FORWARD, ERROR_MSG_CONFIG_FORWARD,
            NetworkShareEventType::SETUP_EVENT);
        NETMGR_EXT_LOG_E(
            "Sub StateMachine[%{public}s] IpfwdAddInterfaceForward newIface[%{public}s] error[%{public}d].",
            ifaceName_.c_str(), upstreamIfaceName_.c_str(), result);
        lastError_ = NETWORKSHARE_ERROR_ENABLE_FORWARDING_ERROR;
        SubSmStateSwitch(SUBSTATE_INIT);
        return;
    }

    result = NetsysController::GetInstance().NetworkAddInterface(LOCAL_NET_ID, ifaceName_);
    if (result != NETMANAGER_SUCCESS) {
        NetworkShareHisysEvent::GetInstance().SendFaultEvent(
            netShareType_, NetworkShareEventOperator::OPERATION_CONFIG_FORWARD,
            NetworkShareEventErrorType::ERROR_CONFIG_FORWARD, ERROR_MSG_ADD_ROUTE_STRATEGY,
            NetworkShareEventType::SETUP_EVENT);
        NETMGR_EXT_LOG_E(
            "Sub StateMachine[%{public}s] SharedState NetworkAddInterface newIface[%{public}s] error[%{public}d].",
            ifaceName_.c_str(), upstreamIfaceName_.c_str(), result);
        NetsysController::GetInstance().IpfwdRemoveInterfaceForward(ifaceName_, upstreamIfaceName_);
        lastError_ = NETWORKSHARE_ERROR_ENABLE_FORWARDING_ERROR;
        SubSmStateSwitch(SUBSTATE_INIT);
        return;
    }
}

void NetworkShareSubStateMachine::RemoveRoutesToLocalNetwork()
{
    std::string destination;
    if (!FindDestinationAddr(destination)) {
        NETMGR_EXT_LOG_E("Get Destination fail");
        return;
    }
    int32_t result =
        NetsysController::GetInstance().NetworkRemoveRoute(LOCAL_NET_ID, ifaceName_, destination, NEXT_HOT);
    if (result != NETSYS_SUCCESS) {
        NetworkShareHisysEvent::GetInstance().SendFaultEvent(
            netShareType_, NetworkShareEventOperator::OPERATION_CANCEL_FORWARD,
            NetworkShareEventErrorType::ERROR_CANCEL_FORWARD, ERROR_MSG_REMOVE_ROUTE_RULE,
            NetworkShareEventType::CANCEL_EVENT);
        NETMGR_EXT_LOG_E("Sub StateMachine[%{public}s] Remove Route error[%{public}d].", ifaceName_.c_str(), result);
    }
}

void NetworkShareSubStateMachine::AddRoutesToLocalNetwork()
{
    std::string destination;
    if (!FindDestinationAddr(destination)) {
        NETMGR_EXT_LOG_E("Get Destination fail");
        return;
    }
    int32_t result = NetsysController::GetInstance().NetworkAddRoute(LOCAL_NET_ID, ifaceName_, destination, NEXT_HOT);
    if (result != NETSYS_SUCCESS) {
        NetworkShareHisysEvent::GetInstance().SendFaultEvent(
            netShareType_, NetworkShareEventOperator::OPERATION_CONFIG_FORWARD,
            NetworkShareEventErrorType::ERROR_CONFIG_FORWARD, ERROR_MSG_ADD_ROUTE_RULE,
            NetworkShareEventType::SETUP_EVENT);
        NETMGR_EXT_LOG_E("Sub StateMachine[%{public}s] Add Route error[%{public}d].", ifaceName_.c_str(), result);
    }
}

bool NetworkShareSubStateMachine::FindDestinationAddr(std::string &destination)
{
    if (netShareType_ == SharingIfaceType::SHARING_BLUETOOTH) {
        if (!GetBtDestinationAddr(destination)) {
            NETMGR_EXT_LOG_E("Sub StateMachine[%{public}s] Add Route Get btpan Destination Addr failed.",
                             ifaceName_.c_str());
            return false;
        }
        return true;
    }
    if (netShareType_ == SharingIfaceType::SHARING_WIFI) {
        if (!GetWifiApDestinationAddr(destination)) {
            NETMGR_EXT_LOG_E("Sub StateMachine[%{public}s] Add Route Get wifi Destination Addr failed.",
                             ifaceName_.c_str());
            return false;
        }
        return true;
    }
    if (netShareType_ == SharingIfaceType::SHARING_USB) {
        if (!GetUsbDestinationAddr(destination)) {
            NETMGR_EXT_LOG_E("Sub StateMachine[%{public}s] Add Route Get usb Destination Addr failed.",
                             ifaceName_.c_str());
            return false;
        }
        return true;
    }
    NETMGR_EXT_LOG_E("Sub StateMachine[%{public}s] Add Route sharetype is unknown.", ifaceName_.c_str());
    return false;
}

bool NetworkShareSubStateMachine::GetWifiHotspotDhcpFlag()
{
    if (configuration_ == nullptr) {
        return false;
    }
    return configuration_->GetWifiHotspotSetDhcpFlag();
}

bool NetworkShareSubStateMachine::GetBtDestinationAddr(std::string &addrStr)
{
    if (configuration_ == nullptr) {
        NETMGR_EXT_LOG_E("GetBtDestinationAddr configuration is null.");
        return false;
    }
    std::string btpanIpv4Addr = configuration_->GetBtpanIpv4Addr();
    if (btpanIpv4Addr.empty()) {
        NETMGR_EXT_LOG_E("Sub StateMachine[%{public}s] get btpan ipv4 addr failed.", ifaceName_.c_str());
        return false;
    }
    std::string::size_type dotPos = btpanIpv4Addr.rfind(".");
    if (dotPos == std::string::npos) {
        NETMGR_EXT_LOG_E("Sub StateMachine[%{public}s] btpan ipv4 addr error.", ifaceName_.c_str());
        return false;
    }
    std::string routeSuffix = configuration_->GetRouteSuffix();
    if (routeSuffix.empty()) {
        NETMGR_EXT_LOG_E("Sub StateMachine[%{public}s] get route suffix failed.", ifaceName_.c_str());
        return false;
    }

    addrStr = btpanIpv4Addr.substr(0, dotPos) + routeSuffix;
    return true;
}

bool NetworkShareSubStateMachine::GetWifiApDestinationAddr(std::string &addrStr)
{
    if (configuration_ == nullptr) {
        NETMGR_EXT_LOG_E("GetWifiApDestinationAddr configuration is null.");
        return false;
    }
    std::string wifiIpv4Addr = configuration_->GetWifiHotspotIpv4Addr();
    if (wifiIpv4Addr.empty()) {
        NETMGR_EXT_LOG_E("Sub StateMachine[%{public}s] get wifi ipv4 addr failed.", ifaceName_.c_str());
        return false;
    }
    std::string::size_type dotPos = wifiIpv4Addr.rfind(".");
    if (dotPos == std::string::npos) {
        NETMGR_EXT_LOG_E("Sub StateMachine[%{public}s] wifi ipv4 addr error.", ifaceName_.c_str());
        return false;
    }
    std::string routeSuffix = configuration_->GetRouteSuffix();
    if (routeSuffix.empty()) {
        NETMGR_EXT_LOG_E("Sub StateMachine[%{public}s] get route suffix failed.", ifaceName_.c_str());
        return false;
    }
    addrStr = wifiIpv4Addr.substr(0, dotPos) + routeSuffix;
    return true;
}

bool NetworkShareSubStateMachine::GetUsbDestinationAddr(std::string &addrStr)
{
    if (configuration_ == nullptr) {
        NETMGR_EXT_LOG_E("GetUsbDestinationAddr configuration is null.");
        return false;
    }
    std::string usbIpv4Addr = configuration_->GetUsbRndisIpv4Addr();
    if (usbIpv4Addr.empty()) {
        NETMGR_EXT_LOG_E("Sub StateMachine[%{public}s] get usb ipv4 addr failed.", ifaceName_.c_str());
        return false;
    }
    std::string::size_type dotPos = usbIpv4Addr.rfind(".");
    if (dotPos == std::string::npos) {
        NETMGR_EXT_LOG_E("Sub StateMachine[%{public}s] usb ipv4 addr error.", ifaceName_.c_str());
        return false;
    }
    std::string routeSuffix = configuration_->GetRouteSuffix();
    if (routeSuffix.empty()) {
        NETMGR_EXT_LOG_E("Sub StateMachine[%{public}s] get route suffix failed.", ifaceName_.c_str());
        return false;
    }
    addrStr = usbIpv4Addr.substr(0, dotPos) + routeSuffix;
    return true;
}

bool NetworkShareSubStateMachine::StartDhcp(const std::shared_ptr<INetAddr> &netAddr)
{
    if (dhcpService_ == nullptr) {
        dhcpService_ = std::make_unique<OHOS::Wifi::DhcpService>();
        if (dhcpService_ == nullptr) {
            NETMGR_EXT_LOG_E("StartDhcp DhcpService create failed.");
            return false;
        }
    }
    if (netAddr == nullptr) {
        NETMGR_EXT_LOG_E("StartDhcp netAddr is null.");
        return false;
    }
    std::string endIp;
    std::string mask;
    if (!CheckConfig(endIp, mask)) {
        NETMGR_EXT_LOG_E("StartDhcp Get necessary config failed.");
        return false;
    }

    std::string ipAddr = netAddr->address_;
    std::string::size_type pos = ipAddr.rfind(".");
    if (pos == std::string::npos) {
        NETMGR_EXT_LOG_E("StartDhcp addr is error.");
        return false;
    }
    std::string ipHead = ipAddr.substr(0, pos);
    std::string ipEnd = ipAddr.substr(pos + 1);
    std::string startIp = std::to_string(atoi(ipEnd.c_str()) + 1);

    OHOS::Wifi::DhcpRange range;
    range.iptype = IP_V4;
    range.strStartip = ipHead + "." + startIp;
    range.strEndip = ipHead + "." + endIp;
    range.strSubnet = mask;
    range.strTagName = ifaceName_;
    NETMGR_EXT_LOG_I(
        "Set dhcp range : ifaceName[%{public}s] TagName[%{public}s] start ip[%{private}s] end ip[%{private}s]",
        ifaceName_.c_str(), range.strTagName.c_str(), range.strStartip.c_str(), range.strEndip.c_str());
    if (dhcpService_->SetDhcpRange(ifaceName_, range) != Wifi::DHCP_OPT_SUCCESS) {
        NETMGR_EXT_LOG_E("StartDhcp SetDhcpRange failed.");
        return false;
    }
    if (dhcpService_->StartDhcpServer(ifaceName_) != Wifi::DHCP_OPT_SUCCESS) {
        NETMGR_EXT_LOG_E("StartDhcpServer failed.");
        return false;
    }

    NETMGR_EXT_LOG_I("StartDhcpServer successful.");
    return true;
}

bool NetworkShareSubStateMachine::CheckConfig(std::string &endIp, std::string &mask)
{
    if (configuration_ == nullptr) {
        NETMGR_EXT_LOG_E("StartDhcp configuration is null.");
        return false;
    }
    endIp = configuration_->GetDhcpEndIP();
    if (endIp.empty()) {
        NETMGR_EXT_LOG_E("StartDhcp GetDhcpEndIP is null.");
        return false;
    }
    mask = configuration_->GetDefaultMask();
    if (mask.empty()) {
        NETMGR_EXT_LOG_E("StartDhcp GetDefaultMask is null.");
        return false;
    }
    return true;
}

bool NetworkShareSubStateMachine::StopDhcp()
{
    if (netShareType_ == SharingIfaceType::SHARING_WIFI) {
        NETMGR_EXT_LOG_W("StopDhcp wifi hotspot not need stop.");
        return true;
    }
    if (dhcpService_ == nullptr) {
        NETMGR_EXT_LOG_E("StopDhcp dhcpService is null.");
        return false;
    }
    int ret = dhcpService_->StopDhcpServer(ifaceName_);
    if (ret != Wifi::DHCP_OPT_SUCCESS) {
        NETMGR_EXT_LOG_E("StopDhcpServer failed, error[%{public}d].", ret);
        return false;
    }
    NETMGR_EXT_LOG_I("StopDhcpServer successful.");
    return true;
}

bool NetworkShareSubStateMachine::ConfigureShareDhcp(bool enabled)
{
    std::shared_ptr<INetAddr> ipv4Address = nullptr;
    if (enabled) {
        bool ret = RequestIpv4Address(ipv4Address);
        if (ipv4Address == nullptr || !ret) {
            NETMGR_EXT_LOG_E("ConfigureShareDhcp no available ipv4 address.");
            return false;
        }
        if (netShareType_ == SharingIfaceType::SHARING_WIFI && !GetWifiHotspotDhcpFlag()) {
            NETMGR_EXT_LOG_W("StartDhcp wifi hotspot not need start.");
            return true;
        }
        return StartDhcp(ipv4Address);
    }
    return StopDhcp();
}

bool NetworkShareSubStateMachine::RequestIpv4Address(std::shared_ptr<INetAddr> &netAddr)
{
    if (configuration_ == nullptr) {
        NETMGR_EXT_LOG_E("RequestIpv4Address get configuration failed.");
        return false;
    }

    netAddr = std::make_shared<INetAddr>();
    netAddr->type_ = INetAddr::IPV4;
    netAddr->prefixlen_ = PREFIX_LENGTH_24;
    netAddr->netMask_ = configuration_->GetDefaultMask();
    if (netAddr->netMask_.empty()) {
        NETMGR_EXT_LOG_E("RequestIpv4Address get default mask failed.");
        return false;
    }
    switch (netShareType_) {
        case SharingIfaceType::SHARING_BLUETOOTH: {
            netAddr->address_ = configuration_->GetBtpanIpv4Addr();
            netAddr->hostName_ = configuration_->GetBtpanDhcpServerName();
            break;
        }
        case SharingIfaceType::SHARING_WIFI: {
            netAddr->address_ = configuration_->GetWifiHotspotIpv4Addr();
            netAddr->hostName_ = configuration_->GetWifiHotspotDhcpServerName();
            break;
        }
        case SharingIfaceType::SHARING_USB: {
            netAddr->address_ = configuration_->GetUsbRndisIpv4Addr();
            netAddr->hostName_ = configuration_->GetUsbRndisDhcpServerName();
            break;
        }
        default:
            NETMGR_EXT_LOG_E("Unknown share type");
            return false;
    }

    if (netAddr->address_.empty() || netAddr->hostName_.empty()) {
        NETMGR_EXT_LOG_E("Failed to get ipv4 Address or dhcp server name.");
        return false;
    }
    return true;
}

void NetworkShareSubStateMachine::CleanupUpstreamInterface()
{
    NETMGR_EXT_LOG_I("Clearn Forward, downstream Iface[%{public}s], upstream iface[%{public}s].", ifaceName_.c_str(),
                     upstreamIfaceName_.c_str());
    RemoveRoutesToLocalNetwork();
    NetsysController::GetInstance().NetworkRemoveInterface(LOCAL_NET_ID, ifaceName_);
    NetsysController::GetInstance().IpfwdRemoveInterfaceForward(ifaceName_, upstreamIfaceName_);
}

bool NetworkShareSubStateMachine::HasChangeUpstreamIfaceSet(const std::string &newUpstreamIface)
{
    if ((upstreamIfaceName_.empty()) && (newUpstreamIface.empty())) {
        return false;
    }
    if ((!upstreamIfaceName_.empty()) && (!newUpstreamIface.empty())) {
        return upstreamIfaceName_ != newUpstreamIface;
    }
    return true;
}

void NetworkShareSubStateMachine::RegisterSubSMCallback(const std::shared_ptr<SubStateMachineCallback> &callback)
{
    trackerCallback_ = callback;
}

SharingIfaceType NetworkShareSubStateMachine::GetNetShareType() const
{
    return netShareType_;
}

const std::string &NetworkShareSubStateMachine::GetInterfaceName() const
{
    return ifaceName_;
}
} // namespace NetManagerStandard
} // namespace OHOS
