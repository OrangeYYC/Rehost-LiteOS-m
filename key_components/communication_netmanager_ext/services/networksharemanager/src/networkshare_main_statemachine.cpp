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

#include "networkshare_main_statemachine.h"

#include "netmgr_ext_log_wrapper.h"
#include "netsys_controller.h"
#include "networkshare_constants.h"
#include "networkshare_sub_statemachine.h"
#include "networkshare_tracker.h"

namespace OHOS {
namespace NetManagerStandard {
static constexpr const char *ERROR_MSG_TRUNON = "Turn on Ip Forward failed";
static constexpr const char *ERROR_MSG_TRUNOFF = "Turn off Ip Forward failed";
static constexpr const char *ERROR_MSG_ENABLE_FORWARD = "Enable Forward failed";
static constexpr const char *ERROR_MSG_DISABLE_FORWARD = "Disable Forward failed";
static constexpr const char *FAKE_DOWNSTREAM_IFACENAME = "";
static constexpr const char *EMPTY_UPSTREAM_IFACENAME = "";

NetworkShareMainStateMachine::NetworkShareMainStateMachine(std::shared_ptr<NetworkShareUpstreamMonitor> &networkmonitor)
    : netshareRequester_("netsharing_requester"), networkMonitor_(networkmonitor)
{
    MainSmStateTable temp;
    temp.event_ = EVENT_IFACE_SM_STATE_ACTIVE;
    temp.curState_ = MAINSTATE_INIT;
    temp.func_ = &NetworkShareMainStateMachine::HandleInitInterfaceStateActive;
    temp.nextState_ = MAINSTATE_ALIVE;
    stateTable_.push_back(temp);

    temp.event_ = EVENT_IFACE_SM_STATE_INACTIVE;
    temp.curState_ = MAINSTATE_INIT;
    temp.func_ = &NetworkShareMainStateMachine::HandleInitInterfaceStateInactive;
    temp.nextState_ = NO_NEXT_STATE;
    stateTable_.push_back(temp);

    temp.event_ = EVENT_IFACE_SM_STATE_ACTIVE;
    temp.curState_ = MAINSTATE_ALIVE;
    temp.func_ = &NetworkShareMainStateMachine::HandleAliveInterfaceStateActive;
    temp.nextState_ = NO_NEXT_STATE;
    stateTable_.push_back(temp);

    temp.event_ = EVENT_IFACE_SM_STATE_INACTIVE;
    temp.curState_ = MAINSTATE_ALIVE;
    temp.func_ = &NetworkShareMainStateMachine::HandleAliveInterfaceStateInactive;
    temp.nextState_ = NO_NEXT_STATE;
    stateTable_.push_back(temp);

    temp.event_ = EVENT_UPSTREAM_CALLBACK;
    temp.curState_ = MAINSTATE_ALIVE;
    temp.func_ = &NetworkShareMainStateMachine::HandleAliveUpstreamMonitorCallback;
    temp.nextState_ = NO_NEXT_STATE;
    stateTable_.push_back(temp);

    temp.event_ = EVENT_IFACE_SM_STATE_INACTIVE;
    temp.curState_ = MAINSTATE_ERROR;
    temp.func_ = &NetworkShareMainStateMachine::HandleErrorInterfaceStateInactive;
    temp.nextState_ = NO_NEXT_STATE;
    stateTable_.push_back(temp);

    temp.event_ = CMD_CLEAR_ERROR;
    temp.curState_ = MAINSTATE_ERROR;
    temp.func_ = &NetworkShareMainStateMachine::HandleErrorClear;
    temp.nextState_ = MAINSTATE_INIT;
    stateTable_.push_back(temp);
}

void NetworkShareMainStateMachine::MainSmStateSwitch(int newState)
{
    int oldState = curState_;
    curState_ = newState;
    NETMGR_EXT_LOG_I("MainSmStateSwitch from[%{public}d] to[%{public}d].", oldState, newState);

    if (oldState == MAINSTATE_INIT) {
        InitStateExit();
    } else if (oldState == MAINSTATE_ALIVE) {
        AliveStateExit();
    } else if (oldState == MAINSTATE_ERROR) {
        ErrorStateExit();
    } else {
        NETMGR_EXT_LOG_E("MainSmStateSwitch oldState is unknow type value.");
    }

    if (newState == MAINSTATE_INIT) {
        InitStateEnter();
    } else if (newState == MAINSTATE_ALIVE) {
        AliveStateEnter();
    } else if (newState == MAINSTATE_ERROR) {
        ErrorStateEnter();
    } else {
        NETMGR_EXT_LOG_E("MainSmStateSwitch newState is unknow type value.");
    }
}

void NetworkShareMainStateMachine::MainSmEventHandle(int eventId, const std::any &messageObj)
{
    std::lock_guard<std::recursive_mutex> lock(mutex_);
    int nextState = NO_NEXT_STATE;
    int (NetworkShareMainStateMachine::*eventActionFun)(const std::any &messageObj) = nullptr;
    for (const auto &iter : stateTable_) {
        if ((eventId == iter.event_) && (curState_ == iter.curState_)) {
            eventActionFun = iter.func_;
            nextState = iter.nextState_;
            break;
        }
    }
    if (eventActionFun == nullptr) {
        NETMGR_EXT_LOG_W("currentstate[%{public}d]  eventId[%{public}d] is not matched.", curState_, eventId);
        return;
    }
    (this->*eventActionFun)(messageObj);
    if (nextState >= MAINSTATE_INIT && nextState < MAINSTATE_MAX) {
        MainSmStateSwitch(nextState);
    }

    NETMGR_EXT_LOG_I("MainSm eventId[%{public}d], handle successful.", eventId);
}

void NetworkShareMainStateMachine::InitStateEnter()
{
    NETMGR_EXT_LOG_I("Enter Init state");
}

void NetworkShareMainStateMachine::InitStateExit()
{
    NETMGR_EXT_LOG_I("Exit Init state");
}

void NetworkShareMainStateMachine::AliveStateEnter()
{
    NETMGR_EXT_LOG_I("Enter Alive State");
    if (!(TurnOnMainShareSettings())) {
        NETMGR_EXT_LOG_E("Enter Alive State TurnOnMainShareSettings error.");
        return;
    }
    if (NetworkShareTracker::GetInstance().UpstreamWanted()) {
        ChooseUpstreamNetwork();
    }
}

void NetworkShareMainStateMachine::AliveStateExit()
{
    NETMGR_EXT_LOG_I("Exit Alive state");
    NetworkShareTracker::GetInstance().NotifyDownstreamsHasNewUpstreamIface(nullptr);
}

void NetworkShareMainStateMachine::ErrorStateEnter()
{
    NETMGR_EXT_LOG_I("Enter Error state, error[%{public}d].", errorType_);
    for_each(subMachineList_.begin(), subMachineList_.end(),
             [this](std::shared_ptr<NetworkShareSubStateMachine> subsm) {
                 if (subsm != nullptr) {
                     NETMGR_EXT_LOG_I("NOTIFY TO SUB SM [%{public}s] EVENT[%{public}d].",
                                      subsm->GetInterfaceName().c_str(), errorType_);
                     subsm->SubSmEventHandle(errorType_, 0);
                 }
             });
}

void NetworkShareMainStateMachine::ErrorStateExit() const
{
    NETMGR_EXT_LOG_I("Exit Error state, error[%{public}d].", errorType_);
}

int NetworkShareMainStateMachine::HandleInitInterfaceStateActive(const std::any &messageObj)
{
    const MessageIfaceActive &temp = std::any_cast<const MessageIfaceActive &>(messageObj);
    std::vector<std::shared_ptr<NetworkShareSubStateMachine>>::iterator iter =
        find(subMachineList_.begin(), subMachineList_.end(), temp.subsm_);
    if (iter == subMachineList_.end()) {
        NETMGR_EXT_LOG_I("add new subSm.");
        subMachineList_.push_back(temp.subsm_);
    }

    NetworkShareTracker::GetInstance().ModifySharedSubStateMachineList(true, temp.subsm_);
    return NETMANAGER_EXT_SUCCESS;
}

int NetworkShareMainStateMachine::HandleInitInterfaceStateInactive(const std::any &messageObj)
{
    return EraseSharedSubSM(messageObj);
}

int NetworkShareMainStateMachine::HandleAliveInterfaceStateActive(const std::any &messageObj)
{
    const MessageIfaceActive &temp = std::any_cast<const MessageIfaceActive &>(messageObj);
    std::vector<std::shared_ptr<NetworkShareSubStateMachine>>::iterator iter =
        find(subMachineList_.begin(), subMachineList_.end(), temp.subsm_);
    if (iter == subMachineList_.end()) {
        NETMGR_EXT_LOG_I("add new subSm.");
        subMachineList_.push_back(temp.subsm_);
    }

    NetworkShareTracker::GetInstance().ModifySharedSubStateMachineList(true, temp.subsm_);
    if (temp.subsm_ != nullptr) {
        std::shared_ptr<UpstreamNetworkInfo> upstreamInfo = nullptr;
        NetworkShareTracker::GetInstance().GetUpstreamInfo(upstreamInfo);
        NETMGR_EXT_LOG_I("NOTIFY TO SUB SM [%{public}s] CMD_NETSHARE_CONNECTION_CHANGED.",
                         temp.subsm_->GetInterfaceName().c_str());
        temp.subsm_->SubSmEventHandle(CMD_NETSHARE_CONNECTION_CHANGED, upstreamInfo);
    }
    return NETMANAGER_EXT_SUCCESS;
}

int NetworkShareMainStateMachine::HandleAliveInterfaceStateInactive(const std::any &messageObj)
{
    int ret = EraseSharedSubSM(messageObj);
    if (ret != NETMANAGER_EXT_SUCCESS) {
        return ret;
    }
    if (subMachineList_.size() == 0) {
        DisableForward();
        TurnOffMainShareSettings();
    }
    return NETMANAGER_EXT_SUCCESS;
}

int NetworkShareMainStateMachine::EraseSharedSubSM(const std::any &messageObj)
{
    const MessageIfaceActive &temp = std::any_cast<const MessageIfaceActive &>(messageObj);
    if (temp.subsm_ == nullptr) {
        NETMGR_EXT_LOG_E("subsm[%{public}d] is null.", temp.value_);
        return NETMANAGER_EXT_ERR_LOCAL_PTR_NULL;
    }
    subMachineList_.erase(
        remove_if(subMachineList_.begin(), subMachineList_.end(),
                  [temp](std::shared_ptr<NetworkShareSubStateMachine> sm) { return sm == temp.subsm_; }),
        subMachineList_.end());

    NetworkShareTracker::GetInstance().ModifySharedSubStateMachineList(false, temp.subsm_);
    return NETMANAGER_EXT_SUCCESS;
}

void NetworkShareMainStateMachine::ChooseUpstreamNetwork()
{
    sptr<NetHandle> pNetHandle = new (std::nothrow) NetHandle();
    sptr<NetAllCapabilities> pNetCapabilities = new (std::nothrow) NetAllCapabilities();
    sptr<NetLinkInfo> pNetLinkInfo = new (std::nothrow) NetLinkInfo();
    std::shared_ptr<UpstreamNetworkInfo> netInfoPtr =
        std::make_shared<UpstreamNetworkInfo>(pNetHandle, pNetCapabilities, pNetLinkInfo);
    if (networkMonitor_ != nullptr && networkMonitor_->GetCurrentGoodUpstream(netInfoPtr)) {
        upstreamIfaceName_ = netInfoPtr->netLinkPro_->ifaceName_;
        int32_t result = NetsysController::GetInstance().EnableNat(FAKE_DOWNSTREAM_IFACENAME, upstreamIfaceName_);
        if (result != NETSYS_SUCCESS) {
            NetworkShareHisysEvent::GetInstance().SendFaultEvent(
                NetworkShareEventOperator::OPERATION_CONFIG_FORWARD, NetworkShareEventErrorType::ERROR_CONFIG_FORWARD,
                ERROR_MSG_ENABLE_FORWARD, NetworkShareEventType::SETUP_EVENT);
            NETMGR_EXT_LOG_E("Main StateMachine enable NAT newIface[%{public}s] error[%{public}d].",
                             upstreamIfaceName_.c_str(), result);
        }
        NetworkShareTracker::GetInstance().SetUpstreamNetHandle(netInfoPtr);
    }
}

int NetworkShareMainStateMachine::HandleAliveUpstreamMonitorCallback(const std::any &messageObj)
{
    if (!NetworkShareTracker::GetInstance().UpstreamWanted()) {
        NETMGR_EXT_LOG_W("don't need handle upstream callback.");
        return NETMANAGER_EXT_SUCCESS;
    }
    const MessageUpstreamInfo &temp = std::any_cast<const MessageUpstreamInfo &>(messageObj);
    switch (temp.cmd_) {
        case EVENT_UPSTREAM_CALLBACK_ON_LINKPROPERTIES: {
            ChooseUpstreamNetwork();
            break;
        }
        case EVENT_UPSTREAM_CALLBACK_ON_LOST: {
            DisableForward();
            break;
        }
        case EVENT_UPSTREAM_CALLBACK_ON_CAPABILITIES: {
            break;
        }
        case EVENT_UPSTREAM_CALLBACK_DEFAULT_SWITCHED: {
            DisableForward();
            ChooseUpstreamNetwork();
            break;
        }
        default:
            break;
    }
    return NETMANAGER_EXT_SUCCESS;
}

int NetworkShareMainStateMachine::HandleErrorInterfaceStateInactive(const std::any &messageObj)
{
    const MessageIfaceActive &temp = std::any_cast<const MessageIfaceActive &>(messageObj);
    if (temp.subsm_ == nullptr) {
        NETMGR_EXT_LOG_E("mode[%{public}d] subsm is null.", temp.value_);
        return NETMANAGER_EXT_ERR_LOCAL_PTR_NULL;
    }
    NETMGR_EXT_LOG_I("NOTIFY TO SUB SM [%{public}s] EVENT[%{public}d].", temp.subsm_->GetInterfaceName().c_str(),
                     errorType_);
    temp.subsm_->SubSmEventHandle(errorType_, 0);
    return NETMANAGER_EXT_SUCCESS;
}

int NetworkShareMainStateMachine::HandleErrorClear(const std::any &messageObj)
{
    (void)messageObj;
    errorType_ = NETWORKSHARING_SHARING_NO_ERROR;
    return NETMANAGER_EXT_SUCCESS;
}

void NetworkShareMainStateMachine::SwitcheToErrorState(int32_t errType)
{
    NETMGR_EXT_LOG_W("SwitcheToErrorState errType[%{public}d].", errType);
    errorType_ = errType;
    MainSmStateSwitch(MAINSTATE_ERROR);
}

bool NetworkShareMainStateMachine::TurnOnMainShareSettings()
{
    if (hasSetForward_) {
        return true;
    }
    int32_t result = NetsysController::GetInstance().IpEnableForwarding(netshareRequester_);
    if (result != NETSYS_SUCCESS) {
        NetworkShareHisysEvent::GetInstance().SendFaultEvent(NetworkShareEventOperator::OPERATION_TURNON_IP_FORWARD,
                                                             NetworkShareEventErrorType::ERROR_TURNON_IP_FORWARD,
                                                             ERROR_MSG_TRUNON, NetworkShareEventType::SETUP_EVENT);
        NETMGR_EXT_LOG_E("ipfwdEnableForwarding is error, switch error State.");
        errorType_ = CMD_IP_FORWARDING_ENABLE_ERROR;
        MainSmStateSwitch(MAINSTATE_ERROR);
        return false;
    }
    hasSetForward_ = true;
    NETMGR_EXT_LOG_I("turn on main ip forward successful.");
    return true;
}

bool NetworkShareMainStateMachine::TurnOffMainShareSettings()
{
    int32_t result = NetsysController::GetInstance().IpDisableForwarding(netshareRequester_);
    if (result != NETSYS_SUCCESS) {
        NetworkShareHisysEvent::GetInstance().SendFaultEvent(NetworkShareEventOperator::OPERATION_TURNOFF_IP_FORWARD,
                                                             NetworkShareEventErrorType::ERROR_TURNOFF_IP_FORWARD,
                                                             ERROR_MSG_TRUNOFF, NetworkShareEventType::CANCEL_EVENT);
        NETMGR_EXT_LOG_E("IpfwdDisableForwarding is error, switch to error State.");
        errorType_ = CMD_IP_FORWARDING_DISABLE_ERROR;
        MainSmStateSwitch(MAINSTATE_ERROR);
        return false;
    }
    NETMGR_EXT_LOG_I("turn off main ip forward successful, switch to init state.");
    MainSmStateSwitch(MAINSTATE_INIT);
    hasSetForward_ = false;
    return true;
}

void NetworkShareMainStateMachine::DisableForward()
{
    NetworkShareTracker::GetInstance().SetUpstreamNetHandle(nullptr);
    int32_t result = NetsysController::GetInstance().DisableNat(FAKE_DOWNSTREAM_IFACENAME, upstreamIfaceName_);
    if (result != NETSYS_SUCCESS) {
        NetworkShareHisysEvent::GetInstance().SendFaultEvent(
            NetworkShareEventOperator::OPERATION_CONFIG_FORWARD, NetworkShareEventErrorType::ERROR_CONFIG_FORWARD,
            ERROR_MSG_DISABLE_FORWARD, NetworkShareEventType::SETUP_EVENT);
        NETMGR_EXT_LOG_E("MainSM disable NAT newIface[%{public}s] in Lost Network error[%{public}d].",
                         upstreamIfaceName_.c_str(), result);
    }
    upstreamIfaceName_ = EMPTY_UPSTREAM_IFACENAME;
}
} // namespace NetManagerStandard
} // namespace OHOS
