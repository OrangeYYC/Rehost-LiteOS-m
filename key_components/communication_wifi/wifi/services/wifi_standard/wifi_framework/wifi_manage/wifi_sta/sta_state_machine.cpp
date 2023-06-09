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

#include "sta_state_machine.h"
#include <cstdio>
#include "if_config.h"
#include "ip_tools.h"
#include "log_helper.h"
#include "mac_address.h"
#include "sta_monitor.h"
#include "wifi_chip_capability.h"
#include "wifi_common_util.h"
#include "wifi_logger.h"
#include "wifi_settings.h"
#include "wifi_sta_hal_interface.h"
#include "wifi_supplicant_hal_interface.h"
#ifndef OHOS_ARCH_LITE
#include "iservice_registry.h"
#include "system_ability_definition.h"
#endif // OHOS_ARCH_LITE

#ifndef OHOS_WIFI_STA_TEST
#include "dhcp_service.h"
#else
#include "mock_dhcp_service.h"
#endif

namespace OHOS {
namespace Wifi {
DEFINE_WIFILOG_LABEL("StaStateMachine");
#define PBC_ANY_BSSID "any"
StaStateMachine::StaStateMachine()
    : StateMachine("StaStateMachine"),
      lastNetworkId(INVALID_NETWORK_ID),
      operationalMode(STA_CONNECT_MODE),
      targetNetworkId(INVALID_NETWORK_ID),
      pinCode(0),
      wpsState(SetupMethod::INVALID),
      lastSignalLevel(-1),
      targetRoamBssid(WPA_BSSID_ANY),
      currentTpType(IPTYPE_IPV4),
      isWpsConnect(IsWpsConnected::WPS_INVALID),
      getIpSucNum(0),
      getIpFailNum(0),
      isRoam(false),
      pDhcpService(nullptr),
      pDhcpResultNotify(nullptr),
      pNetcheck(nullptr),
      pRootState(nullptr),
      pInitState(nullptr),
      pWpaStartingState(nullptr),
      pWpaStartedState(nullptr),
      pWpaStoppingState(nullptr),
      pLinkState(nullptr),
      pSeparatingState(nullptr),
      pSeparatedState(nullptr),
      pApLinkedState(nullptr),
      pWpsState(nullptr),
      pGetIpState(nullptr),
      pLinkedState(nullptr),
      pApRoamingState(nullptr)
{
#ifndef OHOS_ARCH_LITE
    SubscribeSystemAbilityChanged();
#endif // OHOS_ARCH_LITE
}

StaStateMachine::~StaStateMachine()
{
    WIFI_LOGI("StaStateMachine::~StaStateMachine");
    StopHandlerThread();
    ParsePointer(pRootState);
    ParsePointer(pInitState);
    ParsePointer(pWpaStartingState);
    ParsePointer(pWpaStartedState);
    ParsePointer(pWpaStoppingState);
    ParsePointer(pLinkState);
    ParsePointer(pSeparatingState);
    ParsePointer(pSeparatedState);
    ParsePointer(pApLinkedState);
    ParsePointer(pWpsState);
    ParsePointer(pGetIpState);
    ParsePointer(pLinkedState);
    ParsePointer(pApRoamingState);
    if (pDhcpService != nullptr) {
        if (currentTpType == IPTYPE_IPV4) {
            pDhcpService->StopDhcpClient(IF_NAME, false);
        } else {
            pDhcpService->StopDhcpClient(IF_NAME, true);
        }

        pDhcpService->RemoveDhcpResult(pDhcpResultNotify);
    }
    ParsePointer(pDhcpResultNotify);
    ParsePointer(pDhcpService);
    ParsePointer(pNetcheck);
}

/* ---------------------------Initialization functions------------------------------ */
ErrCode StaStateMachine::InitStaStateMachine()
{
    WIFI_LOGI("Enter StaStateMachine::InitStaStateMachine.\n");
    if (!InitialStateMachine()) {
        WIFI_LOGE("Initial StateMachine failed.\n");
        return WIFI_OPT_FAILED;
    }

    if (InitStaStates() == WIFI_OPT_FAILED) {
        return WIFI_OPT_FAILED;
    }
    BuildStateTree();
    SetFirstState(pInitState);
    StartStateMachine();
    InitStaSMHandleMap();

    pDhcpService = new (std::nothrow) DhcpService();
    if (pDhcpService == nullptr) {
        WIFI_LOGE("pDhcpServer is null\n");
        return WIFI_OPT_FAILED;
    }

    pNetcheck = new (std::nothrow)
        StaNetworkCheck(std::bind(&StaStateMachine::HandleNetCheckResult, this,
            std::placeholders::_1, std::placeholders::_2));
    if (pNetcheck == nullptr) {
        WIFI_LOGE("pNetcheck is null\n");
        return WIFI_OPT_FAILED;
    }
    pNetcheck->InitNetCheckThread();
#ifndef OHOS_ARCH_LITE
    NetSupplierInfo = std::make_unique<NetManagerStandard::NetSupplierInfo>().release();
#endif
    return WIFI_OPT_SUCCESS;
}

ErrCode StaStateMachine::InitStaStates()
{
    WIFI_LOGE("Enter InitStaStates\n");
    int tmpErrNumber;
    pRootState = new (std::nothrow)RootState();
    tmpErrNumber = JudgmentEmpty(pRootState);
    pInitState = new (std::nothrow)InitState(this);
    tmpErrNumber += JudgmentEmpty(pInitState);
    pWpaStartingState = new (std::nothrow)WpaStartingState(this);
    tmpErrNumber += JudgmentEmpty(pWpaStartingState);
    pWpaStartedState = new (std::nothrow)WpaStartedState(this);
    tmpErrNumber += JudgmentEmpty(pWpaStartedState);
    pWpaStoppingState = new (std::nothrow)WpaStoppingState(this);
    tmpErrNumber += JudgmentEmpty(pWpaStoppingState);
    pLinkState = new (std::nothrow)LinkState(this);
    tmpErrNumber += JudgmentEmpty(pLinkState);
    pSeparatingState = new (std::nothrow)SeparatingState();
    tmpErrNumber += JudgmentEmpty(pSeparatingState);
    pSeparatedState = new (std::nothrow)SeparatedState(this);
    tmpErrNumber += JudgmentEmpty(pSeparatedState);
    pApLinkedState = new (std::nothrow)ApLinkedState(this);
    tmpErrNumber += JudgmentEmpty(pApLinkedState);
    pWpsState = new (std::nothrow)StaWpsState(this);
    tmpErrNumber += JudgmentEmpty(pWpsState);
    pGetIpState = new (std::nothrow)GetIpState(this);
    tmpErrNumber += JudgmentEmpty(pGetIpState);
    pLinkedState = new (std::nothrow)LinkedState(this);
    tmpErrNumber += JudgmentEmpty(pLinkedState);
    pApRoamingState = new (std::nothrow)ApRoamingState(this);
    tmpErrNumber += JudgmentEmpty(pApRoamingState);
    pDhcpResultNotify = new (std::nothrow)DhcpResultNotify(this);
    tmpErrNumber += JudgmentEmpty(pDhcpResultNotify);
    if (tmpErrNumber != 0) {
        WIFI_LOGE("InitStaStates some one state is null\n");
        return WIFI_OPT_FAILED;
    }
    return WIFI_OPT_SUCCESS;
}

void StaStateMachine::InitWifiLinkedInfo()
{
    linkedInfo.networkId = INVALID_NETWORK_ID;
    linkedInfo.ssid = "";
    linkedInfo.bssid = "";
    linkedInfo.macAddress = "";
    linkedInfo.macType = 0;
    linkedInfo.rxLinkSpeed = 0;
    linkedInfo.txLinkSpeed = 0;
    linkedInfo.rssi = 0;
    linkedInfo.band = 0;
    linkedInfo.frequency = 0;
    linkedInfo.linkSpeed = 0;
    linkedInfo.ipAddress = 0;
    linkedInfo.connState = ConnState::DISCONNECTED;
    linkedInfo.ifHiddenSSID = false;
    linkedInfo.chload = 0;
    linkedInfo.snr = 0;
    linkedInfo.isDataRestricted = 0;
    linkedInfo.portalUrl = "";
    linkedInfo.detailedState = DetailedState::DISCONNECTED;
    linkedInfo.channelWidth = WifiChannelWidth::WIDTH_INVALID;
}

void StaStateMachine::InitLastWifiLinkedInfo()
{
    lastLinkedInfo.networkId = INVALID_NETWORK_ID;
    lastLinkedInfo.ssid = "";
    lastLinkedInfo.bssid = "";
    lastLinkedInfo.macAddress = "";
    linkedInfo.macType = 0;
    lastLinkedInfo.rxLinkSpeed = 0;
    lastLinkedInfo.txLinkSpeed = 0;
    lastLinkedInfo.rssi = 0;
    lastLinkedInfo.band = 0;
    lastLinkedInfo.frequency = 0;
    lastLinkedInfo.linkSpeed = 0;
    lastLinkedInfo.ipAddress = 0;
    lastLinkedInfo.connState = ConnState::DISCONNECTED;
    lastLinkedInfo.ifHiddenSSID = false;
    lastLinkedInfo.chload = 0;
    lastLinkedInfo.snr = 0;
    linkedInfo.isDataRestricted = 0;
    linkedInfo.portalUrl = "";
    lastLinkedInfo.detailedState = DetailedState::DISCONNECTED;
}

void StaStateMachine::BuildStateTree()
{
    StatePlus(pRootState, nullptr);
    StatePlus(pInitState, pRootState);
    StatePlus(pWpaStartingState, pRootState);
    StatePlus(pWpaStartedState, pRootState);
    StatePlus(pLinkState, pWpaStartedState);
    StatePlus(pSeparatingState, pLinkState);
    StatePlus(pSeparatedState, pLinkState);
    StatePlus(pApLinkedState, pLinkState);
    StatePlus(pGetIpState, pApLinkedState);
    StatePlus(pLinkedState, pApLinkedState);
    StatePlus(pApRoamingState, pApLinkedState);
    StatePlus(pWpsState, pLinkState);
    StatePlus(pWpaStoppingState, pRootState);
}

void StaStateMachine::RegisterStaServiceCallback(const StaServiceCallback &callbacks)
{
    WIFI_LOGI("RegisterStaServiceCallback.");
    staCallback = callbacks;
}

/* --------------------------- state machine root state ------------------------------ */
StaStateMachine::RootState::RootState() : State("RootState")
{}

StaStateMachine::RootState::~RootState()
{}

void StaStateMachine::RootState::GoInState()
{
    WIFI_LOGI("RootState GoInState function.");
    return;
}

void StaStateMachine::RootState::GoOutState()
{
    WIFI_LOGI("RootState GoOutState function.");
    return;
}

bool StaStateMachine::RootState::ExecuteStateMsg(InternalMessage *msg)
{
    if (msg == nullptr) {
        return false;
    }

    WIFI_LOGI("RootState-msgCode=%{public}d not handled.\n", msg->GetMessageName());
    return true;
}

/* --------------------------- state machine Init State ------------------------------ */
StaStateMachine::InitState::InitState(StaStateMachine *staStateMachine)
    : State("InitState"), pStaStateMachine(staStateMachine)
{}

StaStateMachine::InitState::~InitState()
{}

void StaStateMachine::InitState::GoInState()
{
    WIFI_LOGI("InitState GoInState function.");
    return;
}

void StaStateMachine::InitState::GoOutState()
{
    WIFI_LOGI("InitState GoOutState function.");
    return;
}

bool StaStateMachine::InitState::ExecuteStateMsg(InternalMessage *msg)
{
    if (msg == nullptr) {
        return false;
    }

    WIFI_LOGI("InitState-msgCode=%{public}d is received.\n", msg->GetMessageName());
    bool ret = NOT_EXECUTED;
    switch (msg->GetMessageName()) {
        case WIFI_SVR_CMD_STA_ENABLE_WIFI: {
            ret = EXECUTED;
            pStaStateMachine->operationalMode = msg->GetParam1();
            pStaStateMachine->StartWifiProcess();
            break;
        }

        case WIFI_SVR_CMD_STA_OPERATIONAL_MODE:
            break;

        default:
            WIFI_LOGI("InitState-msgCode=%d not handled.\n", msg->GetMessageName());
            break;
    }
    return ret;
}

ErrCode StaStateMachine::ConvertDeviceCfg(const WifiDeviceConfig &config) const
{
    LOGI("Enter StaStateMachine::ConvertDeviceCfg.\n");
    WifiIdlDeviceConfig idlConfig;
    idlConfig.networkId = config.networkId;
    idlConfig.ssid = config.ssid;
    idlConfig.bssid = config.bssid;
    idlConfig.psk = config.preSharedKey;
    idlConfig.keyMgmt = config.keyMgmt;
    idlConfig.priority = config.priority;
    idlConfig.scanSsid = config.hiddenSSID ? 1 : 0;
    idlConfig.eap = config.wifiEapConfig.eap;
    idlConfig.identity = config.wifiEapConfig.identity;
    idlConfig.password = config.wifiEapConfig.password;
    idlConfig.clientCert = config.wifiEapConfig.clientCert;
    idlConfig.privateKey = config.wifiEapConfig.privateKey;
    idlConfig.phase2Method = static_cast<int>(config.wifiEapConfig.phase2Method);
    idlConfig.wepKeyIdx = config.wepTxKeyIndex;
    if (strcmp(config.keyMgmt.c_str(), "WEP") == 0) {
        /* for wep */
        idlConfig.authAlgorithms = 0x02;
    }

    for (int i = 0; i < MAX_WEPKEYS_SIZE; i++) {
        idlConfig.wepKeys[i] = config.wepKeys[i];
    }

    if (WifiStaHalInterface::GetInstance().SetDeviceConfig(config.networkId, idlConfig) != WIFI_IDL_OPT_OK) {
        LOGE("StaStateMachine::ConvertDeviceCfg SetDeviceConfig failed!");
        return WIFI_OPT_FAILED;
    }

    if (WifiStaHalInterface::GetInstance().SaveDeviceConfig() != WIFI_IDL_OPT_OK) {
        LOGW("StaStateMachine::ConvertDeviceCfg SaveDeviceConfig failed!");
    }
    return WIFI_OPT_SUCCESS;
}

void StaStateMachine::SyncDeviceConfigToWpa() const
{
    WIFI_LOGI("SyncDeviceConfigToWpa");
    /* Reload wifi Configurations. */
    if (WifiSettings::GetInstance().ReloadDeviceConfig() != 0) {
        WIFI_LOGE("ReloadDeviceConfig is failed!");
    }

    if (WifiStaHalInterface::GetInstance().ClearDeviceConfig() != WIFI_IDL_OPT_OK) {
        WIFI_LOGE("ClearDeviceConfig() failed!");
    } else {
        WIFI_LOGD("ClearDeviceConfig() succeeded!");
        std::vector<WifiDeviceConfig> results;
        WifiSettings::GetInstance().GetDeviceConfig(results);
        for(WifiDeviceConfig result : results) {
            WIFI_LOGD("SyncDeviceConfigToWpa:result.networkId=[%d]!", result.networkId);
            int networkId = INVALID_NETWORK_ID;
            if (WifiStaHalInterface::GetInstance().GetNextNetworkId(networkId) != WIFI_IDL_OPT_OK) {
                WIFI_LOGE("GetNextNetworkId failed.");
                return;
            }
            if (networkId != result.networkId) {
                WIFI_LOGE("DeviceConfig networkId different from wpa config networkId.");
                return;
            }
            ConvertDeviceCfg(result);
        }
        WIFI_LOGD("SyncDeviceConfigToWpa-SaveDeviceConfig() succeed!");
    }
}

void StaStateMachine::StartWifiProcess()
{
    WifiSettings::GetInstance().SetWifiState(static_cast<int>(WifiState::ENABLING));
    staCallback.OnStaOpenRes(OperateResState::OPEN_WIFI_OPENING);
    int res = WifiStaHalInterface::GetInstance().StartWifi();
    if (res == static_cast<int>(WIFI_IDL_OPT_OK)) {
        WIFI_LOGI("Start wifi successfully!");
        if (WifiStaHalInterface::GetInstance().WpaAutoConnect(false) != WIFI_IDL_OPT_OK) {
            WIFI_LOGI("The automatic Wpa connection is disabled failed.");
        }

        /* callback the InterfaceService that wifi is enabled successfully. */
        WifiSettings::GetInstance().SetWifiState(static_cast<int>(WifiState::ENABLED));
        staCallback.OnStaOpenRes(OperateResState::OPEN_WIFI_SUCCEED);
        /* Sets the MAC address of WifiSettings. */
        std::string mac;
        if ((WifiStaHalInterface::GetInstance().GetStaDeviceMacAddress(mac)) == WIFI_IDL_OPT_OK) {
            WifiSettings::GetInstance().SetMacAddress(mac);
        } else {
            WIFI_LOGI("GetStaDeviceMacAddress failed!");
        }
#ifndef OHOS_ARCH_LITE
        WIFI_LOGI("Register netsupplier");
        WifiNetAgent::GetInstance().OnStaMachineWifiStart(staCallback);
#endif
        /* Initialize Connection Information. */
        InitWifiLinkedInfo();
        InitLastWifiLinkedInfo();
        WifiSettings::GetInstance().SaveLinkedInfo(linkedInfo);
        SyncDeviceConfigToWpa();
#ifndef OHOS_ARCH_LITE
        ChipCapability::GetInstance().InitializeChipCapability();
#endif
        /* The current state of StaStateMachine transfers to SeparatedState after
         * enable supplicant.
         */
        SwitchState(pSeparatedState);
    } else {
        /* Notify the InterfaceService that wifi is failed to enable wifi. */
        LOGE("StartWifi failed, and errcode is %d.", res);
        WifiSettings::GetInstance().SetWifiState(static_cast<int>(WifiState::DISABLED));
        WifiSettings::GetInstance().SetUserLastSelectedNetworkId(INVALID_NETWORK_ID);
        staCallback.OnStaOpenRes(OperateResState::OPEN_WIFI_FAILED);
    }
}

/* --------------------------- state machine WpaStarting State ------------------------------ */
StaStateMachine::WpaStartingState::WpaStartingState(StaStateMachine *staStateMachine)
    : State("WpaStartingState"), pStaStateMachine(staStateMachine)
{}

StaStateMachine::WpaStartingState::~WpaStartingState()
{}

void StaStateMachine::WpaStartingState::InitWpsSettings()
{
    WIFI_LOGI("WpaStartingState InitWpsSettings function.");
    return;
}

void StaStateMachine::WpaStartingState::GoInState()
{
    WIFI_LOGI("WpaStartingState GoInState function.");
    return;
}

void StaStateMachine::WpaStartingState::GoOutState()
{
    LOGI("WpaStartingState GoOutState function.");
    return;
}

bool StaStateMachine::WpaStartingState::ExecuteStateMsg(InternalMessage *msg)
{
    if (msg == nullptr) {
        return false;
    }

    bool ret = NOT_EXECUTED;
    switch (msg->GetMessageName()) {
        case WIFI_SVR_CMD_STA_SUP_CONNECTION_EVENT: {
            ret = EXECUTED;
            pStaStateMachine->SwitchState(pStaStateMachine->pWpaStartedState);
            break;
        }
        default:
            break;
    }
    return ret;
}

/* --------------------------- state machine WpaStarted State ------------------------------ */
StaStateMachine::WpaStartedState::WpaStartedState(StaStateMachine *staStateMachine)
    : State("WpaStartedState"), pStaStateMachine(staStateMachine)
{}

StaStateMachine::WpaStartedState::~WpaStartedState()
{}

void StaStateMachine::WpaStartedState::GoInState()
{
    WIFI_LOGI("WpaStartedState GoInState function.");
    if (pStaStateMachine->operationalMode == STA_CONNECT_MODE) {
        pStaStateMachine->SwitchState(pStaStateMachine->pSeparatedState);
    } else if (pStaStateMachine->operationalMode == STA_DISABLED_MODE) {
        pStaStateMachine->SwitchState(pStaStateMachine->pWpaStoppingState);
    }
    return;
}
void StaStateMachine::WpaStartedState::GoOutState()
{
    WIFI_LOGI("WpaStartedState GoOutState function.");
    return;
}

bool StaStateMachine::WpaStartedState::ExecuteStateMsg(InternalMessage *msg)
{
    if (msg == nullptr) {
        LOGI("msg is nullptr");
        return false;
    }

    WIFI_LOGI("WpaStartedState ExecuteStateMsg-msgCode:%{public}d.\n", msg->GetMessageName());
    bool ret = NOT_EXECUTED;
    switch (msg->GetMessageName()) {
        case WIFI_SVR_CMD_STA_DISABLE_WIFI: {
            ret = EXECUTED;
            pStaStateMachine->StopWifiProcess();
            break;
        }

        default:
            break;
    }
    return ret;
}

void StaStateMachine::StopWifiProcess()
{
    WIFI_LOGI("Enter StaStateMachine::StopWifiProcess.\n");
#ifndef OHOS_ARCH_LITE
    WifiNetAgent::GetInstance().UnregisterNetSupplier();
#endif
    WIFI_LOGI("Stop wifi is in process...\n");
    WifiSettings::GetInstance().SetWifiState(static_cast<int>(WifiState::DISABLING));
    staCallback.OnStaCloseRes(OperateResState::CLOSE_WIFI_CLOSING);
    StopTimer(static_cast<int>(CMD_SIGNAL_POLL));
    if (currentTpType == IPTYPE_IPV4) {
        pDhcpService->StopDhcpClient(IF_NAME, false);
    } else {
        pDhcpService->StopDhcpClient(IF_NAME, true);
    }
    isRoam = false;
    WifiSettings::GetInstance().SetMacAddress("");

    IpInfo ipInfo;
    WifiSettings::GetInstance().SaveIpInfo(ipInfo);
#ifdef OHOS_ARCH_LITE
    IfConfig::GetInstance().FlushIpAddr(IF_NAME, IPTYPE_IPV4);
#endif

    /* clear connection information. */
    InitWifiLinkedInfo();
    WifiSettings::GetInstance().SaveLinkedInfo(linkedInfo);

    if (WifiStaHalInterface::GetInstance().StopWifi() == WIFI_IDL_OPT_OK) {
        /* Callback result to InterfaceService. */
        WifiSettings::GetInstance().SetWifiState(static_cast<int>(WifiState::DISABLED));
        staCallback.OnStaCloseRes(OperateResState::CLOSE_WIFI_SUCCEED);
        WIFI_LOGI("Stop WifiProcess successfully!");

        /* The current state of StaStateMachine transfers to InitState. */
        SwitchState(pInitState);
    } else {
        WIFI_LOGE("StopWifiProcess failed.");
        WifiSettings::GetInstance().SetWifiState(static_cast<int>(WifiState::UNKNOWN));
        staCallback.OnStaCloseRes(OperateResState::CLOSE_WIFI_FAILED);
    }
    WifiSettings::GetInstance().SetUserLastSelectedNetworkId(INVALID_NETWORK_ID);
}

/* --------------------------- state machine WpaStopping State ------------------------------ */
StaStateMachine::WpaStoppingState::WpaStoppingState(StaStateMachine *staStateMachine)
    : State("WpaStoppingState"), pStaStateMachine(staStateMachine)
{}

StaStateMachine::WpaStoppingState::~WpaStoppingState()
{}

void StaStateMachine::WpaStoppingState::GoInState()
{
    WIFI_LOGI("WpaStoppingState GoInState function.");
    pStaStateMachine->SwitchState(pStaStateMachine->pInitState);
    return;
}

void StaStateMachine::WpaStoppingState::GoOutState()
{
    WIFI_LOGI("WpaStoppingState GoOutState function.");
    return;
}

bool StaStateMachine::WpaStoppingState::ExecuteStateMsg(InternalMessage *msg)
{
    if (msg == nullptr) {
        return false;
    }

    bool ret = NOT_EXECUTED;
    WIFI_LOGI("WpaStoppingState-msgCode=%{public}d not handled.\n", msg->GetMessageName());
    return ret;
}

/* --------------------------- state machine link State ------------------------------ */
StaStateMachine::LinkState::LinkState(StaStateMachine *staStateMachine)
    : State("LinkState"), pStaStateMachine(staStateMachine)
{}

StaStateMachine::LinkState::~LinkState()
{}

void StaStateMachine::LinkState::GoInState()
{
    WIFI_LOGI("LinkState GoInState function.");
    return;
}

void StaStateMachine::LinkState::GoOutState()
{
    WIFI_LOGI("LinkState GoOutState function.");
    return;
}

bool StaStateMachine::LinkState::ExecuteStateMsg(InternalMessage *msg)
{
    if (msg == nullptr) {
        return false;
    }
    LOGI("LinkState ExecuteStateMsg function:msgName=[%{public}d].\n", msg->GetMessageName());
    auto iter = pStaStateMachine->staSmHandleFuncMap.find(msg->GetMessageName());
    if (iter != pStaStateMachine->staSmHandleFuncMap.end()) {
        (pStaStateMachine->*(iter->second))(msg);
        return EXECUTED;
    }
    return NOT_EXECUTED;
}

/* -- state machine Connect State Message processing function -- */
int StaStateMachine::InitStaSMHandleMap()
{
    staSmHandleFuncMap[CMD_SIGNAL_POLL] = &StaStateMachine::DealSignalPollResult;
    staSmHandleFuncMap[WIFI_SVR_CMD_STA_CONNECT_NETWORK] = &StaStateMachine::DealConnectToUserSelectedNetwork;
    staSmHandleFuncMap[WIFI_SVR_CMD_STA_CONNECT_SAVED_NETWORK] = &StaStateMachine::DealConnectToUserSelectedNetwork;
    staSmHandleFuncMap[WIFI_SVR_CMD_STA_NETWORK_DISCONNECTION_EVENT] = &StaStateMachine::DealDisconnectEvent;
    staSmHandleFuncMap[WIFI_SVR_CMD_STA_NETWORK_CONNECTION_EVENT] = &StaStateMachine::DealConnectionEvent;
    staSmHandleFuncMap[CMD_NETWORK_CONNECT_TIMEOUT] = &StaStateMachine::DealConnectTimeOutCmd;
    staSmHandleFuncMap[WPA_BLOCK_LIST_CLEAR_EVENT] = &StaStateMachine::DealWpaBlockListClearEvent;
    staSmHandleFuncMap[WIFI_SVR_CMD_STA_STARTWPS] = &StaStateMachine::DealStartWpsCmd;
    staSmHandleFuncMap[WIFI_SVR_CMD_STA_WPS_TIMEOUT_EVNET] = &StaStateMachine::DealWpsConnectTimeOutEvent;
    staSmHandleFuncMap[WIFI_SVR_CMD_STA_CANCELWPS] = &StaStateMachine::DealCancelWpsCmd;
    staSmHandleFuncMap[WIFI_SVR_CMD_STA_RECONNECT_NETWORK] = &StaStateMachine::DealReConnectCmd;
    staSmHandleFuncMap[WIFI_SVR_CMD_STA_REASSOCIATE_NETWORK] = &StaStateMachine::DealReassociateCmd;
    staSmHandleFuncMap[WIFI_SVR_COM_STA_START_ROAM] = &StaStateMachine::DealStartRoamCmd;
    staSmHandleFuncMap[WIFI_SVR_CMD_STA_WPA_PASSWD_WRONG_EVENT] = &StaStateMachine::DealWpaLinkFailEvent;
    staSmHandleFuncMap[WIFI_SVR_CMD_STA_WPA_FULL_CONNECT_EVENT] = &StaStateMachine::DealWpaLinkFailEvent;
    staSmHandleFuncMap[WIFI_SVR_CMD_STA_WPA_ASSOC_REJECT_EVENT] = &StaStateMachine::DealWpaLinkFailEvent;
    staSmHandleFuncMap[CMD_START_NETCHECK] = &StaStateMachine::DealNetworkCheck;
    return WIFI_OPT_SUCCESS;
}

int setRssi(int rssi)
{
    if (rssi < INVALID_RSSI_VALUE) {
        rssi = INVALID_RSSI_VALUE;
    }

    if (rssi > MAX_RSSI_VALUE) {
        rssi = MAX_RSSI_VALUE;
    }
    return rssi;
}

void StaStateMachine::DealSignalPollResult(InternalMessage *msg)
{
    LOGI("enter DealSignalPollResult.\n");
    if (msg == nullptr) {
        LOGE("InternalMessage msg is null.");
        return;
    }
    WifiWpaSignalInfo signalInfo;
    WifiStaHalInterface::GetInstance().GetConnectSignalInfo(linkedInfo.bssid, signalInfo);
    LOGI("DealSignalPollResult, signal:%{public}d, txLinkSpeed:%{public}d, rxLinkSpeed:%{public}d, "
        "freq:%{public}d, noise:%{public}d.\n",
        signalInfo.signal, signalInfo.txrate, signalInfo.rxrate, signalInfo.frequency,
        signalInfo.noise);
    if (signalInfo.signal > INVALID_RSSI_VALUE && signalInfo.signal < MAX_RSSI_VALUE) {
        if (signalInfo.signal > 0) {
            linkedInfo.rssi = setRssi((signalInfo.signal - SIGNAL_INFO));
        } else {
            linkedInfo.rssi = setRssi(signalInfo.signal);
        }
        int currentSignalLevel = WifiSettings::GetInstance().GetSignalLevel(linkedInfo.rssi, linkedInfo.band);
        LOGI("DealSignalPollResult, networkId:%{public}d, ssid:%{private}s, rssi:%{public}d, band:%{public}d, "
            "connState:%{public}d, detailedState:%{public}d.\n",
            linkedInfo.networkId, SsidAnonymize(linkedInfo.ssid).c_str(), linkedInfo.rssi, linkedInfo.band,
            linkedInfo.connState, linkedInfo.detailedState);
        LOGI("DealSignalPollResult currentSignalLevel:%{public}d, lastSignalLevel:%{public}d.\n",
            currentSignalLevel, lastSignalLevel);
        if (currentSignalLevel != lastSignalLevel) {
            if (staCallback.OnStaRssiLevelChanged != nullptr) {
                staCallback.OnStaRssiLevelChanged(linkedInfo.rssi);
            }
#ifndef OHOS_ARCH_LITE
            if (NetSupplierInfo != nullptr) {
                TimeStats timeStats("Call UpdateNetSupplierInfo");
                NetSupplierInfo->isAvailable_ = true;
                NetSupplierInfo->isRoaming_ = isRoam;
                NetSupplierInfo->strength_ = linkedInfo.rssi;
                NetSupplierInfo->frequency_ = linkedInfo.frequency;
                WifiNetAgent::GetInstance().UpdateNetSupplierInfo(NetSupplierInfo);
            }
#endif
            lastSignalLevel = currentSignalLevel;
        }
    } else {
        linkedInfo.rssi = INVALID_RSSI_VALUE;
    }

    if (signalInfo.txrate > 0) {
        linkedInfo.txLinkSpeed = signalInfo.txrate;
        linkedInfo.linkSpeed = signalInfo.txrate;
        if (staCallback.OnStaStreamChanged != nullptr) {
            staCallback.OnStaStreamChanged(StreamDirection::STREAM_DIRECTION_UP);
        }
    }

    if (signalInfo.rxrate > 0) {
        linkedInfo.rxLinkSpeed = signalInfo.rxrate;
        if (staCallback.OnStaStreamChanged != nullptr) {
            staCallback.OnStaStreamChanged(StreamDirection::STREAM_DIRECTION_DOWN);
        }
    }

    if (signalInfo.frequency > 0) {
        linkedInfo.frequency = signalInfo.frequency;
    }
    linkedInfo.snr = linkedInfo.rssi - signalInfo.noise;
    if (linkedInfo.wifiStandard == WIFI_MODE_UNDEFINED) {
        WifiSettings::GetInstance().SetWifiLinkedStandardAndMaxSpeed(linkedInfo);
    }
    LOGI("DealSignalPollResult GetWifiStandard:%{public}d, bssid:%{public}s rxmax:%{public}d txmax:%{public}d.\n",
         linkedInfo.wifiStandard, MacAnonymize(linkedInfo.bssid).c_str(), linkedInfo.maxSupportedRxLinkSpeed,
         linkedInfo.maxSupportedTxLinkSpeed);
    WifiSettings::GetInstance().SaveLinkedInfo(linkedInfo);
    ConvertFreqToChannel();
    StartTimer(static_cast<int>(CMD_SIGNAL_POLL), STA_SIGNAL_POLL_DELAY);
}

void StaStateMachine::ConvertFreqToChannel()
{
    WifiDeviceConfig config;
    if (WifiSettings::GetInstance().GetDeviceConfig(linkedInfo.networkId, config) != 0) {
        LOGE("GetDeviceConfig failed!");
        return;
    }
    config.frequency = linkedInfo.frequency;
    if (linkedInfo.frequency >= FREQ_2G_MIN && linkedInfo.frequency <= FREQ_2G_MAX) {
        config.band = linkedInfo.band = static_cast<int>(BandType::BAND_2GHZ);
        config.channel = (linkedInfo.frequency - FREQ_2G_MIN) / CENTER_FREQ_DIFF + CHANNEL_2G_MIN;
    } else if (linkedInfo.frequency == CHANNEL_14_FREQ) {
        config.channel = CHANNEL_14;
    } else if (linkedInfo.frequency >= FREQ_5G_MIN && linkedInfo.frequency <= FREQ_5G_MAX) {
        config.band = linkedInfo.band = static_cast<int>(BandType::BAND_5GHZ);
        config.channel = (linkedInfo.frequency - FREQ_5G_MIN) / CENTER_FREQ_DIFF + CHANNEL_5G_MIN;
    }
    WifiSettings::GetInstance().AddDeviceConfig(config);
    return;
}

void StaStateMachine::OnConnectFailed(int networkId)
{
    WIFI_LOGE("Connect to network failed: %{public}d.\n", networkId);
    SaveLinkstate(ConnState::DISCONNECTED, DetailedState::FAILED);
    staCallback.OnStaConnChanged(OperateResState::CONNECT_ENABLE_NETWORK_FAILED, linkedInfo);
    staCallback.OnStaConnChanged(OperateResState::DISCONNECT_DISCONNECTED, linkedInfo);
}

void StaStateMachine::DealConnectToUserSelectedNetwork(InternalMessage *msg)
{
    LOGI("enter DealConnectToUserSelectedNetwork.\n");
    if (msg == nullptr) {
        LOGE("msg is null.\n");
        return;
    }

    int networkId = msg->GetParam1();
    bool forceReconnect = msg->GetParam2();

    if (networkId == linkedInfo.networkId) {
        if (linkedInfo.connState == ConnState::CONNECTED) {
            staCallback.OnStaConnChanged(OperateResState::CONNECT_AP_CONNECTED, linkedInfo);
            WIFI_LOGI("This network is in use and does not need to be reconnected.\n");
            return;
        }
        if (linkedInfo.connState == ConnState::CONNECTING &&
            linkedInfo.detailedState == DetailedState::OBTAINING_IPADDR) {
            WIFI_LOGI("This network is connecting and does not need to be reconnected.\n");
            return;
        }
    }

    /* Save connection information. */
    SaveLinkstate(ConnState::CONNECTING, DetailedState::CONNECTING);
    /* Callback result to InterfaceService. */
    staCallback.OnStaConnChanged(OperateResState::CONNECT_CONNECTING, linkedInfo);

    if (StartConnectToNetwork(networkId) != WIFI_OPT_SUCCESS) {
        OnConnectFailed(networkId);
        return;
    }

    /* Sets network status. */
    WifiSettings::GetInstance().EnableNetwork(networkId, forceReconnect);
    WifiSettings::GetInstance().SetDeviceAfterConnect(networkId);
    WifiSettings::GetInstance().SetDeviceState(networkId, (int)WifiDeviceConfigStatus::ENABLED, false);
}

void StaStateMachine::DealConnectTimeOutCmd(InternalMessage *msg)
{
    LOGW("enter DealConnectTimeOutCmd.\n");
    if (msg == nullptr) {
        WIFI_LOGE("msg is nul\n");
    }

    if (linkedInfo.connState == ConnState::CONNECTED) {
        WIFI_LOGE("Currently connected and do not process timeout.\n");
        return;
    }

    WifiSettings::GetInstance().SetConnectTimeoutBssid(linkedInfo.bssid);
    InitWifiLinkedInfo();
    SaveLinkstate(ConnState::DISCONNECTED, DetailedState::CONNECTION_TIMEOUT);
    WifiSettings::GetInstance().SaveLinkedInfo(linkedInfo);
    staCallback.OnStaConnChanged(OperateResState::CONNECT_CONNECTING_TIMEOUT, linkedInfo);
    staCallback.OnStaConnChanged(OperateResState::DISCONNECT_DISCONNECTED, linkedInfo);
}

void StaStateMachine::DealConnectionEvent(InternalMessage *msg)
{
    if (msg == nullptr) {
        WIFI_LOGE("DealConnectionEvent, msg is nullptr.\n");
        return;
    }

    WIFI_LOGI("enter DealConnectionEvent");
    WifiSettings::GetInstance().SetDeviceAfterConnect(targetNetworkId);
    WifiSettings::GetInstance().SetDeviceState(targetNetworkId, (int)WifiDeviceConfigStatus::ENABLED, false);
    WifiSettings::GetInstance().SyncDeviceConfig();
    /* Stop clearing the Wpa_blocklist. */
    StopTimer(static_cast<int>(WPA_BLOCK_LIST_CLEAR_EVENT));
    ConnectToNetworkProcess(msg);
    StopTimer(static_cast<int>(CMD_NETWORK_CONNECT_TIMEOUT));
    StartTimer(static_cast<int>(CMD_SIGNAL_POLL), 0);

    if (wpsState != SetupMethod::INVALID) {
        SyncAllDeviceConfigs();
        wpsState = SetupMethod::INVALID;
    }
#ifndef OHOS_ARCH_LITE
    if (NetSupplierInfo != nullptr) {
        NetSupplierInfo->isAvailable_ = true;
        NetSupplierInfo->isRoaming_ = isRoam;
        WIFI_LOGI("On connect update net supplier info\n");
        WifiNetAgent::GetInstance().OnStaMachineUpdateNetSupplierInfo(NetSupplierInfo);
    }
#endif
    /* Callback result to InterfaceService. */
    staCallback.OnStaConnChanged(OperateResState::CONNECT_OBTAINING_IP, linkedInfo);

    /* The current state of StaStateMachine transfers to GetIpState. */
    SwitchState(pGetIpState);
    WifiSettings::GetInstance().SetUserLastSelectedNetworkId(INVALID_NETWORK_ID);
}

void StaStateMachine::DealDisconnectEvent(InternalMessage *msg)
{
    LOGI("Enter DealDisconnectEvent.\n");
    if (msg == nullptr) {
        WIFI_LOGE("msg is null\n");
    }
    if (wpsState != SetupMethod::INVALID) {
        WIFI_LOGE("wpsState is INVALID\n");
        return;
    }
#ifndef OHOS_ARCH_LITE
    if (NetSupplierInfo != nullptr) {
        NetSupplierInfo->isAvailable_ = false;
        WIFI_LOGI("On disconnect update net supplier info\n");
        WifiNetAgent::GetInstance().OnStaMachineUpdateNetSupplierInfo(NetSupplierInfo);
    }
#endif
    StopTimer(static_cast<int>(CMD_SIGNAL_POLL));
    StopTimer(static_cast<int>(CMD_START_NETCHECK));
    pNetcheck->StopNetCheckThread();
    if (currentTpType == IPTYPE_IPV4) {
        pDhcpService->StopDhcpClient(IF_NAME, false);
    } else {
        pDhcpService->StopDhcpClient(IF_NAME, true);
    }
    getIpSucNum = 0;
    getIpFailNum = 0;
    isRoam = false;

    IpInfo ipInfo;
    WifiSettings::GetInstance().SaveIpInfo(ipInfo);
#ifdef OHOS_ARCH_LITE
    IfConfig::GetInstance().FlushIpAddr(IF_NAME, IPTYPE_IPV4);
#endif
    /* Initialize connection information. */
    InitWifiLinkedInfo();
    if (lastLinkedInfo.detailedState == DetailedState::CONNECTING) {
        linkedInfo.networkId = lastLinkedInfo.networkId;
        linkedInfo.ssid = lastLinkedInfo.ssid;
        linkedInfo.connState = ConnState::CONNECTING;
        linkedInfo.detailedState = DetailedState::CONNECTING;
        WifiSettings::GetInstance().SaveLinkedInfo(linkedInfo);
    } else {
        WifiSettings::GetInstance().SaveLinkedInfo(linkedInfo);
    }
    /* Callback result to InterfaceService. */
    staCallback.OnStaConnChanged(OperateResState::DISCONNECT_DISCONNECTED, linkedInfo);
    SwitchState(pSeparatedState);
    return;
}

void StaStateMachine::DealWpaLinkFailEvent(InternalMessage *msg)
{
    LOGW("enter DealWpaLinkFailEvent.\n");
    if (msg == nullptr) {
        LOGE("msg is null.\n");
        return;
    }
    StopTimer(static_cast<int>(CMD_NETWORK_CONNECT_TIMEOUT));
    InitWifiLinkedInfo();
    WifiSettings::GetInstance().SaveLinkedInfo(linkedInfo);
    if (msg->GetMessageName() == WIFI_SVR_CMD_STA_WPA_PASSWD_WRONG_EVENT) {
        SaveLinkstate(ConnState::DISCONNECTED, DetailedState::PASSWORD_ERROR);
        staCallback.OnStaConnChanged(OperateResState::CONNECT_PASSWORD_WRONG, linkedInfo);
    } else if (msg->GetMessageName() == WIFI_SVR_CMD_STA_WPA_FULL_CONNECT_EVENT) {
        DisableNetwork(targetNetworkId);
        SaveLinkstate(ConnState::DISCONNECTED, DetailedState::CONNECTION_FULL);
        staCallback.OnStaConnChanged(OperateResState::CONNECT_CONNECTION_FULL, linkedInfo);
        staCallback.OnStaConnChanged(OperateResState::DISCONNECT_DISCONNECTED, linkedInfo);
    } else if (msg->GetMessageName() == WIFI_SVR_CMD_STA_WPA_ASSOC_REJECT_EVENT) {
        DisableNetwork(targetNetworkId);
        SaveLinkstate(ConnState::DISCONNECTED, DetailedState::CONNECTION_REJECT);
        staCallback.OnStaConnChanged(OperateResState::CONNECT_CONNECTION_REJECT, linkedInfo);
        staCallback.OnStaConnChanged(OperateResState::DISCONNECT_DISCONNECTED, linkedInfo);
    }
}

void StaStateMachine::DealReConnectCmd(InternalMessage *msg)
{
    LOGI("enter DealReConnectCmd.\n");
    if (msg == nullptr) {
        WIFI_LOGE("msg is null\n");
    }

    if (linkedInfo.connState == ConnState::CONNECTED) {
        WIFI_LOGE("Network is already connected, ignore the re-connect command!\n");
        return;
    }

    if (WifiStaHalInterface::GetInstance().Reconnect() == WIFI_IDL_OPT_OK) {
        WIFI_LOGI("StaStateMachine ReConnect successfully!");
        /* Callback result to InterfaceService */
        staCallback.OnStaConnChanged(OperateResState::CONNECT_CONNECTING, linkedInfo);
        StopTimer(static_cast<int>(CMD_NETWORK_CONNECT_TIMEOUT));
        StartTimer(static_cast<int>(CMD_NETWORK_CONNECT_TIMEOUT), STA_NETWORK_CONNECTTING_DELAY);
    } else {
        WIFI_LOGE("ReConnect failed!");
    }
}

void StaStateMachine::DealReassociateCmd(InternalMessage *msg)
{
    LOGI("enter DealReassociateCmd.\n");
    if (msg == nullptr) {
        WIFI_LOGE("msg is null\n");
    }

    if (WifiStaHalInterface::GetInstance().Reassociate() == WIFI_IDL_OPT_OK) {
        /* Callback result to InterfaceService */
        staCallback.OnStaConnChanged(OperateResState::CONNECT_ASSOCIATING, linkedInfo);
        WIFI_LOGD("StaStateMachine ReAssociate successfully!");
        StopTimer(static_cast<int>(CMD_NETWORK_CONNECT_TIMEOUT));
        StartTimer(static_cast<int>(CMD_NETWORK_CONNECT_TIMEOUT), STA_NETWORK_CONNECTTING_DELAY);
    } else {
        WIFI_LOGE("ReAssociate failed!");
    }
}

void StaStateMachine::DealStartWpsCmd(InternalMessage *msg)
{
    WIFI_LOGI("enter DealStartWpsCmd\n");
    if (msg == nullptr) {
        return;
    }

    if (WifiStaHalInterface::GetInstance().ClearDeviceConfig() != WIFI_IDL_OPT_OK) {
        LOGE("ClearDeviceConfig() failed!");
        return;
    }

    StartWpsMode(msg);
    if ((wpsState == SetupMethod::DISPLAY) || (wpsState == SetupMethod::KEYPAD)) {
        WIFI_LOGW("Clear WPA block list every ten second!");
        SendMessage(WPA_BLOCK_LIST_CLEAR_EVENT);
    }
}

void StaStateMachine::StartWpsMode(InternalMessage *msg)
{
    if (msg == nullptr) {
        return;
    }
    /*
     * Make judgement to wps configuration information: the function will exit if
     * the result is fail, then else continue to chose the Wps starting mode. The
     * current state of StaStateMachine transfers to WpsState after Wps code start
     * successfully.
     */
    WifiIdlWpsConfig wpsParam;
    WpsConfig wpsConfig;
    wpsConfig.setup = static_cast<SetupMethod>(msg->GetParam1());
    wpsConfig.pin = msg->GetStringFromMessage();
    wpsConfig.bssid = msg->GetStringFromMessage();
    if (wpsConfig.bssid.length() == 0 || wpsConfig.bssid == PBC_ANY_BSSID) {
        wpsParam.anyFlag = 1;
        wpsParam.bssid = PBC_ANY_BSSID;
    } else {
        wpsParam.anyFlag = 0;
        wpsParam.bssid = wpsConfig.bssid;
    }
    wpsParam.multiAp = MULTI_AP;
    WIFI_LOGI("wpsConfig  setup = %{public}d", wpsConfig.setup);
    WIFI_LOGI("wpsParam.AnyFlag = %{public}d, wpsParam.mulitAp = %{public}d, wpsParam.bssid = %{public}s",
        wpsParam.anyFlag,
        wpsParam.multiAp,
        MacAnonymize(wpsParam.bssid).c_str());

    if (wpsConfig.setup == SetupMethod::PBC) {
        if (WifiStaHalInterface::GetInstance().StartWpsPbcMode(wpsParam) == WIFI_IDL_OPT_OK) {
            wpsState = wpsConfig.setup;
            WIFI_LOGD("StartWpsPbcMode() succeed!");
            /* Callback result to InterfaceService. */
            staCallback.OnWpsChanged(WpsStartState::START_PBC_SUCCEED, pinCode);
            SwitchState(pWpsState);
        } else {
            LOGE("StartWpsPbcMode() failed!");
            staCallback.OnWpsChanged(WpsStartState::START_PBC_FAILED, pinCode);
        }
    } else if (wpsConfig.setup == SetupMethod::DISPLAY) {
        if (WifiStaHalInterface::GetInstance().StartWpsPinMode(wpsParam, pinCode) == WIFI_IDL_OPT_OK) {
            wpsState = wpsConfig.setup;
            /* Callback result to InterfaceService. */
            staCallback.OnWpsChanged(WpsStartState::START_PIN_SUCCEED, pinCode);
            WIFI_LOGD("StartWpsPinMode() succeed!  pincode: %d", pinCode);
            SwitchState(pWpsState);
        } else {
            WIFI_LOGE("StartWpsPinMode() failed!");
            staCallback.OnWpsChanged(WpsStartState::START_PIN_FAILED, pinCode);
        }
    } else if (wpsConfig.setup == SetupMethod::KEYPAD) {
        if (WifiStaHalInterface::GetInstance().StartWpsPinMode(wpsParam, pinCode) == WIFI_IDL_OPT_OK) {
            wpsState = wpsConfig.setup;
            /* Callback result to InterfaceService. */
                staCallback.OnWpsChanged(WpsStartState::START_AP_PIN_SUCCEED, pinCode);
            SwitchState(pWpsState);
        } else {
            LOGE("StartWpsPinMode() failed.");
            staCallback.OnWpsChanged(WpsStartState::START_AP_PIN_FAILED, pinCode);
        }
    } else {
        LOGE("Start Wps failed!");
        staCallback.OnWpsChanged(WpsStartState::START_WPS_FAILED, pinCode);
    }
}

void StaStateMachine::DealWpaBlockListClearEvent(InternalMessage *msg)
{
    if (msg != nullptr) {
        WIFI_LOGE("enter DealWpaBlockListClearEvent\n");
    }
    if (WifiStaHalInterface::GetInstance().WpaBlocklistClear() != WIFI_IDL_OPT_OK) {
        WIFI_LOGE("Clearing the Wpa_blocklist failed\n");
    }
    StartTimer(static_cast<int>(WPA_BLOCK_LIST_CLEAR_EVENT), BLOCK_LIST_CLEAR_TIMER);
    WIFI_LOGI("Clearing the Wpa_blocklist.\n");
}

void StaStateMachine::DealWpsConnectTimeOutEvent(InternalMessage *msg)
{
    WIFI_LOGW("enter DealWpsConnectTimeOutEvent\n");
    if (msg == nullptr) {
        WIFI_LOGE("msg is nullptr!\n");
        return;
    }

    DealCancelWpsCmd(msg);

    /* Callback InterfaceService that WPS time out. */
    staCallback.OnWpsChanged(WpsStartState::WPS_TIME_OUT, pinCode);
    SwitchState(pSeparatedState);
}

void StaStateMachine::DealCancelWpsCmd(InternalMessage *msg)
{
    if (msg == nullptr) {
        WIFI_LOGE("msg is null\n");
    }

    StopTimer(static_cast<int>(WPA_BLOCK_LIST_CLEAR_EVENT));
    isWpsConnect = IsWpsConnected::WPS_INVALID;
    if (WifiStaHalInterface::GetInstance().StopWps() == WIFI_IDL_OPT_OK) {
        WIFI_LOGI("CancelWps succeed!");
        /* Callback result to InterfaceService that stop Wps connection successfully. */
        if (wpsState == SetupMethod::PBC) {
            staCallback.OnWpsChanged(WpsStartState::STOP_PBC_SUCCEED, pinCode);
        } else if (wpsState == SetupMethod::DISPLAY) {
            staCallback.OnWpsChanged(WpsStartState::STOP_PIN_SUCCEED, pinCode);
        } else if (wpsState == SetupMethod::KEYPAD) {
                staCallback.OnWpsChanged(WpsStartState::STOP_AP_PIN_SUCCEED, pinCode);
        }
        if (wpsState != SetupMethod::INVALID) {
            wpsState = SetupMethod::INVALID;
            SyncAllDeviceConfigs();

            if (WifiStaHalInterface::GetInstance().EnableNetwork(lastNetworkId) == WIFI_IDL_OPT_OK) {
                WIFI_LOGI("EnableNetwork success! networkId is %{public}d", lastNetworkId);
                if (WifiStaHalInterface::GetInstance().SaveDeviceConfig() != WIFI_IDL_OPT_OK) {
                    WIFI_LOGW("SaveDeviceConfig failed!");
                } else {
                    WIFI_LOGI("SaveDeviceConfig success!");
                }
            } else {
                WIFI_LOGE("EnableNetwork failed");
            }
        }
    } else {
        WIFI_LOGE("CancelWps failed!");
        if (wpsState == SetupMethod::PBC) {
            staCallback.OnWpsChanged(WpsStartState::STOP_PBC_FAILED, pinCode);
        } else if (wpsState == SetupMethod::DISPLAY) {
            staCallback.OnWpsChanged(WpsStartState::STOP_PIN_FAILED, pinCode);
        } else if (wpsState == SetupMethod::KEYPAD) {
            staCallback.OnWpsChanged(WpsStartState::STOP_AP_PIN_FAILED, pinCode);
        }
    }
    SwitchState(pSeparatedState);
}

void StaStateMachine::DealStartRoamCmd(InternalMessage *msg)
{
    if (msg == nullptr) {
        return;
    }

    WIFI_LOGI("enter DealStartRoamCmd\n");
    std::string bssid = msg->GetStringFromMessage();
    /* GetDeviceConfig from Configuration center. */
    WifiDeviceConfig network;
    WifiSettings::GetInstance().GetDeviceConfig(linkedInfo.networkId, network);

    /* Setting the network. */
    WifiIdlDeviceConfig idlConfig;
    idlConfig.networkId = linkedInfo.networkId;
    idlConfig.ssid = linkedInfo.ssid;
    idlConfig.bssid = bssid;
    idlConfig.psk = network.preSharedKey;
    idlConfig.keyMgmt = network.keyMgmt;
    idlConfig.priority = network.priority;
    idlConfig.scanSsid = network.hiddenSSID ? 1 : 0;
    idlConfig.eap = network.wifiEapConfig.eap;
    idlConfig.identity = network.wifiEapConfig.identity;
    idlConfig.password = network.wifiEapConfig.password;
    idlConfig.clientCert = network.wifiEapConfig.clientCert;
    idlConfig.privateKey = network.wifiEapConfig.privateKey;
    idlConfig.phase2Method = static_cast<int>(network.wifiEapConfig.phase2Method);

    if (WifiStaHalInterface::GetInstance().SetDeviceConfig(linkedInfo.networkId, idlConfig) != WIFI_IDL_OPT_OK) {
        WIFI_LOGE("DealStartRoamCmd SetDeviceConfig() failed!");
        return;
    }
    WIFI_LOGD("DealStartRoamCmd  SetDeviceConfig() succeed!");

    /* Save to Configuration center. */
    network.bssid = bssid;
    WifiSettings::GetInstance().AddDeviceConfig(network);
    WifiSettings::GetInstance().SyncDeviceConfig();

    /* Save linkedinfo */
    linkedInfo.bssid = bssid;
    WifiSettings::GetInstance().SaveLinkedInfo(linkedInfo);

    if (WifiStaHalInterface::GetInstance().Reassociate() != WIFI_IDL_OPT_OK) {
        WIFI_LOGE("START_ROAM-ReAssociate() failed!");
    }
    WIFI_LOGI("START_ROAM-ReAssociate() succeeded!");
    /* Start roaming */
    SwitchState(pApRoamingState);
}

ErrCode StaStateMachine::StartConnectToNetwork(int networkId)
{
    targetNetworkId = networkId;
    SetRandomMac(targetNetworkId);

    WifiDeviceConfig deviceConfig;
    if (WifiSettings::GetInstance().GetDeviceConfig(networkId, deviceConfig) != 0) {
        LOGE("StartConnectToNetwork get GetDeviceConfig failed!");
        return WIFI_OPT_FAILED;
    }
    
    WifiStaHalInterface::GetInstance().SetBssid(networkId, deviceConfig.userSelectBssid.c_str());

    if (WifiStaHalInterface::GetInstance().EnableNetwork(targetNetworkId) != WIFI_IDL_OPT_OK) {
        LOGE("EnableNetwork() failed!");
        return WIFI_OPT_FAILED;
    }

    if (WifiStaHalInterface::GetInstance().Connect(targetNetworkId) != WIFI_IDL_OPT_OK) {
        LOGE("Connect failed!");
        staCallback.OnStaConnChanged(OperateResState::CONNECT_SELECT_NETWORK_FAILED, linkedInfo);
        return WIFI_OPT_FAILED;
    }

    if (WifiStaHalInterface::GetInstance().SaveDeviceConfig() != WIFI_IDL_OPT_OK) {
        /* OHOS's wpa don't support save command, so don't judge as failure */
        LOGE("SaveDeviceConfig() failed!");
    }

    StopTimer(static_cast<int>(CMD_NETWORK_CONNECT_TIMEOUT));
    StartTimer(static_cast<int>(CMD_NETWORK_CONNECT_TIMEOUT), STA_NETWORK_CONNECTTING_DELAY);
    return WIFI_OPT_SUCCESS;
}

void StaStateMachine::MacAddressGenerate(std::string &strMac)
{
    LOGI("enter MacAddressGenerate\n");
    constexpr int arraySize = 4;
    constexpr int macBitSize = 12;
    constexpr int firstBit = 1;
    constexpr int lastBit = 11;
    constexpr int two = 2;
    constexpr int hexBase = 16;
    constexpr int octBase = 8;
    int ret = 0;
    char strMacTmp[arraySize] = {0};
    srand(static_cast<unsigned int>(time(nullptr)));
    for (int i = 0; i < macBitSize; i++) {
        if (i != firstBit) {
            ret = sprintf_s(strMacTmp, arraySize, "%x", rand() % hexBase);
        } else {
            ret = sprintf_s(strMacTmp, arraySize, "%x", two * (rand() % octBase));
        }
        if (ret == -1) {
            LOGE("StaStateMachine::MacAddressGenerate failed, sprintf_s return -1!\n");
        }
        strMac += strMacTmp;
        if ((i % two) != 0 && (i != lastBit)) {
            strMac.append(":");
        }
    }
}

bool StaStateMachine::ComparedKeymgmt(const std::string scanInfoKeymgmt, const std::string deviceKeymgmt)
{
    if (deviceKeymgmt == "WPA-PSK") {
        return scanInfoKeymgmt.find("PSK") != std::string::npos;
    } else if (deviceKeymgmt == "WPA-EAP") {
        return scanInfoKeymgmt.find("EAP") != std::string::npos;
    } else if (deviceKeymgmt == "SAE") {
        return scanInfoKeymgmt.find("SAE") != std::string::npos;
    } else if (deviceKeymgmt == "NONE") {
        return (scanInfoKeymgmt.find("PSK") == std::string::npos) &&
               (scanInfoKeymgmt.find("EAP") == std::string::npos) && (scanInfoKeymgmt.find("SAE") == std::string::npos);
    } else {
        return false;
    }
}

bool StaStateMachine::SetRandomMac(int networkId)
{
    LOGI("enter SetRandomMac.\n");
    WifiDeviceConfig deviceConfig;
    if (WifiSettings::GetInstance().GetDeviceConfig(networkId, deviceConfig) != 0) {
        LOGE("SetRandomMac : GetDeviceConfig failed!\n");
        return false;
    }
    std::string lastMac;
    std::string currentMac;
    if (deviceConfig.wifiPrivacySetting == WifiPrivacyConfig::DEVICEMAC) {
    } else {
        WifiStoreRandomMac randomMacInfo;
        std::vector<WifiScanInfo> scanInfoList;
        WifiSettings::GetInstance().GetScanInfoList(scanInfoList);
        for (auto scanInfo : scanInfoList) {
            if ((deviceConfig.ssid == scanInfo.ssid) &&
                (ComparedKeymgmt(scanInfo.capabilities, deviceConfig.keyMgmt))) {
                randomMacInfo.ssid = scanInfo.ssid;
                randomMacInfo.keyMgmt = deviceConfig.keyMgmt;
                randomMacInfo.peerBssid = scanInfo.bssid;
                break;
            }
        }
        if (randomMacInfo.ssid.empty()) {
            LOGE("scanInfo has no target wifi!\n");
            return false;
        }

        /* Sets the MAC address of WifiSettings. */
        MacAddressGenerate(randomMacInfo.randomMac);
        WifiSettings::GetInstance().AddRandomMac(randomMacInfo);
        currentMac = randomMacInfo.randomMac;
    }

    if ((WifiStaHalInterface::GetInstance().GetStaDeviceMacAddress(lastMac)) != WIFI_IDL_OPT_OK) {
        LOGE("GetStaDeviceMacAddress failed!");
        return false;
    }

    if (MacAddress::IsValidMac(currentMac.c_str())) {
        LOGI("Check MacAddress successfully.\n");
        if (lastMac != currentMac) {
            if (WifiStaHalInterface::GetInstance().SetConnectMacAddr(currentMac) != WIFI_IDL_OPT_OK) {
                LOGE("set Mac [%s] failed.\n", currentMac.c_str());
                return false;
            }
        }
        WifiSettings::GetInstance().SetMacAddress(currentMac);
        deviceConfig.macAddress = currentMac;
        WifiSettings::GetInstance().AddDeviceConfig(deviceConfig);
        WifiSettings::GetInstance().SyncDeviceConfig();
    } else {
        LOGE("Check MacAddress error.\n");
        return false;
    }
    return true;
}

void StaStateMachine::StartRoamToNetwork(std::string bssid)
{
    InternalMessage *msg = CreateMessage();
    if (msg == nullptr) {
        return;
    }

    msg->SetMessageName(WIFI_SVR_COM_STA_START_ROAM);
    msg->AddStringMessageBody(bssid);
    SendMessage(msg);
}

bool StaStateMachine::IsRoaming(void)
{
    return isRoam;
}

void StaStateMachine::OnNetworkConnectionEvent(int networkId, std::string bssid)
{
    InternalMessage *msg = CreateMessage();
    if (msg == nullptr) {
        LOGE("msg is nullptr.\n");
        return;
    }

    msg->SetMessageName(WIFI_SVR_CMD_STA_NETWORK_CONNECTION_EVENT);
    msg->SetParam1(networkId);
    msg->AddStringMessageBody(bssid);
    SendMessage(msg);
}

void StaStateMachine::OnBssidChangedEvent(std::string reason, std::string bssid)
{
    InternalMessage *msg = CreateMessage();
    if (msg == nullptr) {
        LOGE("msg is nullptr.\n");
        return;
    }

    msg->SetMessageName(WIFI_SVR_CMD_STA_BSSID_CHANGED_EVENT);
    msg->AddStringMessageBody(reason);
    msg->AddStringMessageBody(bssid);
    SendMessage(msg);
}

void StaStateMachine::OnDhcpResultNotifyEvent(bool result)
{
    InternalMessage *msg = CreateMessage();
    if (msg == nullptr) {
        LOGE("msg is nullptr.\n");
        return;
    }

    msg->SetMessageName(WIFI_SVR_CMD_STA_DHCP_RESULT_NOTIFY_EVENT);
    msg->SetParam1(result);
    SendMessage(msg);
}

/* --------------------------- state machine Separating State ------------------------------ */
StaStateMachine::SeparatingState::SeparatingState() : State("SeparatingState")
{}

StaStateMachine::SeparatingState::~SeparatingState()
{}

void StaStateMachine::SeparatingState::GoInState()
{
    WIFI_LOGI("SeparatingState GoInState function.");
    return;
}

void StaStateMachine::SeparatingState::GoOutState()
{
    WIFI_LOGI("SeparatingState GoOutState function.");
}

bool StaStateMachine::SeparatingState::ExecuteStateMsg(InternalMessage *msg)
{
    if (msg == nullptr) {
        return false;
    }

    bool ret = NOT_EXECUTED;
    WIFI_LOGI("SeparatingState-msgCode=%{public}d not handled.\n", msg->GetMessageName());
    return ret;
}

/* --------------------------- state machine Disconnected State ------------------------------ */
StaStateMachine::SeparatedState::SeparatedState(StaStateMachine *staStateMachine)
    : State("SeparatedState"), pStaStateMachine(staStateMachine)
{}

StaStateMachine::SeparatedState::~SeparatedState()
{}

void StaStateMachine::SeparatedState::GoInState()
{
    WIFI_LOGI("SeparatedState GoInState function.");
    return;
}

void StaStateMachine::SeparatedState::GoOutState()
{
    WIFI_LOGI("SeparatedState GoOutState function.");
    return;
}

bool StaStateMachine::SeparatedState::ExecuteStateMsg(InternalMessage *msg)
{
    if (msg == nullptr) {
        return false;
    }

    WIFI_LOGI("SeparatedState-msgCode=%{public}d received.\n", msg->GetMessageName());
    bool ret = NOT_EXECUTED;
    switch (msg->GetMessageName()) {
        case WIFI_SVR_CMD_STA_NETWORK_DISCONNECTION_EVENT:
            break;

        case WIFI_SVR_CMD_STA_ENABLE_WIFI: {
            ret = EXECUTED;
            WIFI_LOGE("Wifi has already started! start Wifi failed!");
            /* Callback result to InterfaceService. */
            pStaStateMachine->staCallback.OnStaOpenRes(OperateResState::OPEN_WIFI_OVERRIDE_OPEN_FAILED);
            break;
        }

        default:
            break;
    }

    return ret;
}

/* --------------------------- state machine ApConnected State ------------------------------ */
StaStateMachine::ApLinkedState::ApLinkedState(StaStateMachine *staStateMachine)
    : State("ApLinkedState"), pStaStateMachine(staStateMachine)
{}

StaStateMachine::ApLinkedState::~ApLinkedState()
{}

void StaStateMachine::ApLinkedState::GoInState()
{
    WIFI_LOGI("ApLinkedState GoInState function.");
    return;
}

void StaStateMachine::ApLinkedState::GoOutState()
{
    WIFI_LOGI("ApLinkedState GoOutState function.");
    return;
}

bool StaStateMachine::ApLinkedState::ExecuteStateMsg(InternalMessage *msg)
{
    if (msg == nullptr) {
        return false;
    }

    WIFI_LOGI("ApLinkedState-msgCode=%{public}d received.\n", msg->GetMessageName());
    bool ret = NOT_EXECUTED;
    switch (msg->GetMessageName()) {
        /* The current state of StaStateMachine transfers to SeparatingState when
         * receive the Separating message.
         */
        case WIFI_SVR_CMD_STA_DISCONNECT: {
            ret = EXECUTED;
            pStaStateMachine->DisConnectProcess();
            break;
        }
        case WIFI_SVR_CMD_STA_NETWORK_CONNECTION_EVENT: {
            ret = EXECUTED;
            pStaStateMachine->StopTimer(static_cast<int>(WPA_BLOCK_LIST_CLEAR_EVENT));
            WIFI_LOGI("Stop clearing wpa block list");
            /* Save linkedinfo */
            pStaStateMachine->linkedInfo.networkId = msg->GetParam1();
            pStaStateMachine->linkedInfo.bssid = msg->GetStringFromMessage();
            WifiSettings::GetInstance().SaveLinkedInfo(pStaStateMachine->linkedInfo);

            break;
        }
        default:
            break;
    }
    return ret;
}

void StaStateMachine::DisConnectProcess()
{
    WIFI_LOGI("Enter DisConnectProcess!");
    staCallback.OnStaConnChanged(OperateResState::DISCONNECT_DISCONNECTING, linkedInfo);
    if (WifiStaHalInterface::GetInstance().Disconnect() == WIFI_IDL_OPT_OK) {
        WIFI_LOGI("Disconnect() succeed!");
#ifndef OHOS_ARCH_LITE
        if (NetSupplierInfo != nullptr) {
            NetSupplierInfo->isAvailable_ = false;
            WIFI_LOGI("Disconnect process update netsupplierinfo");
            WifiNetAgent::GetInstance().OnStaMachineUpdateNetSupplierInfo(NetSupplierInfo);
        }
#endif
        WIFI_LOGI("Disconnect update wifi status");
        /* Save connection information to WifiSettings. */
        SaveLinkstate(ConnState::DISCONNECTED, DetailedState::DISCONNECTED);
        DisableNetwork(linkedInfo.networkId);

        getIpSucNum = 0;
        /* The current state of StaStateMachine transfers to SeparatedState. */
        SwitchState(pSeparatedState);
    } else {
        SaveLinkstate(ConnState::DISCONNECTING, DetailedState::FAILED);
        staCallback.OnStaConnChanged(OperateResState::DISCONNECT_DISCONNECT_FAILED, linkedInfo);
        WIFI_LOGE("Disconnect() failed!");
    }
}

/* --------------------------- state machine Wps State ------------------------------ */
StaStateMachine::StaWpsState::StaWpsState(StaStateMachine *staStateMachine)
    : State("StaWpsState"), pStaStateMachine(staStateMachine)
{}

StaStateMachine::StaWpsState::~StaWpsState()
{}

void StaStateMachine::StaWpsState::GoInState()
{
    WIFI_LOGI("WpsState GoInState function.");
    return;
}

void StaStateMachine::StaWpsState::GoOutState()
{
    WIFI_LOGI("WpsState GoOutState function.");
}

bool StaStateMachine::StaWpsState::ExecuteStateMsg(InternalMessage *msg)
{
    if (msg == nullptr) {
        return false;
    }

    bool ret = NOT_EXECUTED;
    switch (msg->GetMessageName()) {
        case WIFI_SVR_CMD_STA_WPS_START_EVENT: {
            /* Wps starts successfully and Wait until the connection is complete. */
            break;
        }
        case WIFI_SVR_CMD_STA_NETWORK_CONNECTION_EVENT: {
            ret = EXECUTED;
            /* Stop clearing the Wpa_blocklist. */
            pStaStateMachine->StopTimer(static_cast<int>(WPA_BLOCK_LIST_CLEAR_EVENT));

            WIFI_LOGI("WPS mode connect to a network!");
            pStaStateMachine->ConnectToNetworkProcess(msg);
            pStaStateMachine->SyncAllDeviceConfigs();
            /* Callback result to InterfaceService. */
            pStaStateMachine->SaveLinkstate(ConnState::CONNECTING, DetailedState::OBTAINING_IPADDR);
            pStaStateMachine->staCallback.OnStaConnChanged(OperateResState::CONNECT_OBTAINING_IP,
                                                           pStaStateMachine->linkedInfo);
            pStaStateMachine->SwitchState(pStaStateMachine->pGetIpState);
            break;
        }
        case WIFI_SVR_CMD_STA_STARTWPS: {
            ret = EXECUTED;
            auto setup = static_cast<SetupMethod>(msg->GetParam1());
            /* Callback InterfaceService that wps has started successfully. */
            WIFI_LOGE("WPS has already started, start wps failed!");
            if (setup == SetupMethod::PBC) {
                pStaStateMachine->staCallback.OnWpsChanged(WpsStartState::PBC_STARTED_ALREADY,
                                                           pStaStateMachine->pinCode);
                } else if ((setup == SetupMethod::DISPLAY) || (setup == SetupMethod::KEYPAD)) {
                pStaStateMachine->staCallback.OnWpsChanged(WpsStartState::PIN_STARTED_ALREADY,
                                                           pStaStateMachine->pinCode);
            }
            break;
        }
        case WIFI_SVR_CMD_STA_WPS_OVERLAP_EVENT: {
            ret = EXECUTED;
            WIFI_LOGI("Wps PBC Overlap!");
            /* Callback InterfaceService that PBC is conflicting. */
            pStaStateMachine->staCallback.OnWpsChanged(WpsStartState::START_PBC_FAILED_OVERLAP,
                                                       pStaStateMachine->pinCode);
            pStaStateMachine->SwitchState(pStaStateMachine->pSeparatedState);
            break;
        }
        case WIFI_SVR_CMD_STA_CANCELWPS: {
            ret = EXECUTED;
            pStaStateMachine->DealCancelWpsCmd(msg);
            break;
        }
        default:
            break;
    }
    return ret;
}

void StaStateMachine::SyncAllDeviceConfigs()
{
    std::vector<WifiDeviceConfig> result;
    WifiIdlDeviceConfig idlConfig;
    if (WifiSettings::GetInstance().GetDeviceConfig(result) != -1) {
        for (std::vector<WifiDeviceConfig>::iterator it = result.begin(); it != result.end(); ++it) {
            if (isWpsConnect == IsWpsConnected::WPS_CONNECTED && it->networkId == 0) {
                continue;
            }
            if (WifiStaHalInterface::GetInstance().GetNextNetworkId(it->networkId) == WIFI_IDL_OPT_OK) {
                WIFI_LOGI("GetNextNetworkId succeed");
                idlConfig.networkId = it->networkId;
                idlConfig.ssid = it->ssid;
                idlConfig.psk = it->preSharedKey;
                idlConfig.keyMgmt = it->keyMgmt;
                idlConfig.priority = it->priority;
                idlConfig.scanSsid = it->hiddenSSID ? 1 : 0;
                idlConfig.eap = it->wifiEapConfig.eap;
                idlConfig.identity = it->wifiEapConfig.identity;
                idlConfig.password = it->wifiEapConfig.password;
                idlConfig.clientCert = it->wifiEapConfig.clientCert;
                idlConfig.privateKey = it->wifiEapConfig.privateKey;
                idlConfig.phase2Method = static_cast<int>(it->wifiEapConfig.phase2Method);
                if (WifiStaHalInterface::GetInstance().SetDeviceConfig(it->networkId, idlConfig) != WIFI_IDL_OPT_OK) {
                    WIFI_LOGE("SetDeviceConfig failed!");
                }
                WIFI_LOGD("SetDeviceConfig succeed!");
            } else {
                WIFI_LOGE("GetNextNetworkId failed!");
            }
            WIFI_LOGD("networkId = %{public}d", it->networkId);
            WifiStaHalInterface::GetInstance().SaveDeviceConfig();
        }
        WIFI_LOGD("Synchronizing network information!");
    } else {
        WIFI_LOGE("The Device config in WifiSettings is empty!");
    }
}

/* --------------------------- state machine GetIp State ------------------------------ */
StaStateMachine::GetIpState::GetIpState(StaStateMachine *staStateMachine)
    : State("GetIpState"), pStaStateMachine(staStateMachine)
{}

StaStateMachine::GetIpState::~GetIpState()
{}

void StaStateMachine::GetIpState::GoInState()
{
    WIFI_LOGI("GetIpState GoInState function.");
#ifdef WIFI_DHCP_DISABLED
    SaveLinkstate(ConnState::CONNECTED, DetailedState::WORKING);
    staCallback.OnStaConnChanged(OperateResState::CONNECT_NETWORK_ENABLED, linkedInfo);
    SwitchState(pLinkedState);
    return;
#endif
    pStaStateMachine->getIpSucNum = 0;
    WifiDeviceConfig config;
    AssignIpMethod assignMethod = AssignIpMethod::DHCP;
    int ret = WifiSettings::GetInstance().GetDeviceConfig(pStaStateMachine->linkedInfo.networkId, config);
    if (ret == 0) {
        assignMethod = config.wifiIpConfig.assignMethod;
    }

    if (config.wifiProxyconfig.configureMethod == ConfigureProxyMethod::MANUALCONFIGUE) {
        std::string hostName = config.wifiProxyconfig.manualProxyConfig.serverHostName;
        std::string noProxys = config.wifiProxyconfig.manualProxyConfig.exclusionObjectList;
        std::string port = std::to_string(config.wifiProxyconfig.manualProxyConfig.serverPort);
        if (!hostName.empty()) {
            IfConfig::GetInstance().SetProxy(true, hostName, port, noProxys, "");
        }
    }

    if (assignMethod == AssignIpMethod::STATIC) {
        pStaStateMachine->currentTpType = config.wifiIpConfig.staticIpAddress.ipAddress.address.family;
        if (!pStaStateMachine->ConfigStaticIpAddress(config.wifiIpConfig.staticIpAddress)) {
            if (pStaStateMachine->staCallback.OnStaConnChanged != nullptr) {
                pStaStateMachine->staCallback.OnStaConnChanged(
                OperateResState::CONNECT_NETWORK_DISABLED, pStaStateMachine->linkedInfo);
            }
            pStaStateMachine->DisConnectProcess();
            LOGE("ConfigstaticIpAddress failed!\n");
        }
    } else {
        int dhcpRet = 0;
        DhcpServiceInfo dhcpInfo;
        pStaStateMachine->pDhcpService->GetDhcpInfo(IF_NAME, dhcpInfo);
        LOGI("GetIpState get dhcp result, isRoam=%{public}d, clientRunStatus=%{public}d.",
            pStaStateMachine->isRoam, dhcpInfo.clientRunStatus);
        pStaStateMachine->currentTpType = static_cast<int>(WifiSettings::GetInstance().GetDhcpIpType());
        if (pStaStateMachine->currentTpType == IPTYPE_IPV6) {
            dhcpRet = pStaStateMachine->pDhcpService->StartDhcpClient(IF_NAME, true);
        } else {
            dhcpRet = pStaStateMachine->pDhcpService->StartDhcpClient(IF_NAME, false);
        }
        if ((dhcpRet != 0) || (pStaStateMachine->pDhcpService->GetDhcpResult(
            IF_NAME, pStaStateMachine->pDhcpResultNotify, DHCP_TIME) != 0)) {
            LOGE(" Dhcp connection failed.\n");
            pStaStateMachine->SaveLinkstate(ConnState::DISCONNECTED, DetailedState::OBTAINING_IPADDR_FAIL);
            pStaStateMachine->staCallback.OnStaConnChanged(
                OperateResState::CONNECT_OBTAINING_IP_FAILED, pStaStateMachine->linkedInfo);
            if (!pStaStateMachine->isRoam) {
                pStaStateMachine->DisConnectProcess();
            }
        }
    }
    return;
}

void StaStateMachine::GetIpState::GoOutState()
{
    WIFI_LOGI("GetIpState GoOutState function.");
}

bool StaStateMachine::GetIpState::ExecuteStateMsg(InternalMessage *msg)
{
    if (msg == nullptr) {
        return false;
    }

    bool ret = NOT_EXECUTED;
    bool result = false;
    WIFI_LOGI("GetIpState-msgCode=%{public}d received.\n", msg->GetMessageName());
    switch (msg->GetMessageName()) {
        case WIFI_SVR_CMD_STA_DHCP_RESULT_NOTIFY_EVENT: {
            ret = EXECUTED;
            result = msg->GetParam1();
            WIFI_LOGI("GetIpState, get ip result:%{public}d.\n", result);
            pStaStateMachine->SwitchState(pStaStateMachine->pLinkedState);
            break;
        }
        default:
            break;
    }

    return ret;
}

/* --- state machine GetIp State functions ----- */
bool StaStateMachine::ConfigStaticIpAddress(StaticIpAddress &staticIpAddress)
{
    WIFI_LOGI("Enter StaStateMachine::SetDhcpResultFromStatic.");
    DhcpResult result;
    switch (currentTpType) {
        case IPTYPE_IPV4: {
            result.iptype = IPTYPE_IPV4;
            result.strYourCli = staticIpAddress.ipAddress.address.GetIpv4Address();
            result.strRouter1 = staticIpAddress.gateway.GetIpv4Address();
            result.strSubnet = staticIpAddress.GetIpv4Mask();
            result.strDns1 = staticIpAddress.dnsServer1.GetIpv4Address();
            result.strDns2 = staticIpAddress.dnsServer2.GetIpv4Address();
            pDhcpResultNotify->OnSuccess(1, IF_NAME, result);
            break;
        }
        case IPTYPE_IPV6: {
            result.iptype = IPTYPE_IPV6;
            result.strYourCli = staticIpAddress.ipAddress.address.GetIpv6Address();
            result.strRouter1 = staticIpAddress.gateway.GetIpv6Address();
            result.strSubnet = staticIpAddress.GetIpv6Mask();
            result.strDns1 = staticIpAddress.dnsServer1.GetIpv6Address();
            result.strDns2 = staticIpAddress.dnsServer2.GetIpv6Address();
            pDhcpResultNotify->OnSuccess(1, IF_NAME, result);
            break;
        }
        case IPTYPE_MIX: {
            result.iptype = IPTYPE_IPV4;
            result.strYourCli = staticIpAddress.ipAddress.address.GetIpv4Address();
            result.strRouter1 = staticIpAddress.gateway.GetIpv4Address();
            result.strSubnet = staticIpAddress.GetIpv4Mask();
            result.strDns1 = staticIpAddress.dnsServer1.GetIpv4Address();
            result.strDns2 = staticIpAddress.dnsServer2.GetIpv4Address();
            pDhcpResultNotify->OnSuccess(1, IF_NAME, result);

            result.iptype = IPTYPE_IPV6;
            result.strYourCli = staticIpAddress.ipAddress.address.GetIpv6Address();
            result.strRouter1 = staticIpAddress.gateway.GetIpv6Address();
            result.strSubnet = staticIpAddress.GetIpv6Mask();
            result.strDns1 = staticIpAddress.dnsServer1.GetIpv6Address();
            result.strDns2 = staticIpAddress.dnsServer2.GetIpv6Address();
            pDhcpResultNotify->OnSuccess(1, IF_NAME, result);
            break;
        }

        default:
            WIFI_LOGE("Invalid currentTpType: %{public}d", currentTpType);
            return false;
    }
    return true;
}

void StaStateMachine::HandleNetCheckResult(StaNetState netState, const std::string portalUrl)
{
    WIFI_LOGI("Enter HandleNetCheckResult, netState:%{public}d.", netState);
    if (linkedInfo.connState != ConnState::CONNECTED) {
        WIFI_LOGE("connState is NOT in connected state, connState:%{public}d\n", linkedInfo.connState);
        return;
    }

    if (netState == StaNetState::NETWORK_STATE_WORKING) {
        WIFI_LOGI("HandleNetCheckResult network state is working\n");
        /* Save connection information to WifiSettings. */
        SaveLinkstate(ConnState::CONNECTED, DetailedState::WORKING);
        staCallback.OnStaConnChanged(OperateResState::CONNECT_NETWORK_ENABLED, linkedInfo);
    } else if (netState == StaNetState::NETWORK_CHECK_PORTAL) {
        linkedInfo.portalUrl = portalUrl;
        SaveLinkstate(ConnState::CONNECTED, DetailedState::CAPTIVE_PORTAL_CHECK);
        staCallback.OnStaConnChanged(OperateResState::CONNECT_CHECK_PORTAL, linkedInfo);
    } else {
        WIFI_LOGI("HandleNetCheckResult network state is notworking.\n");
        SaveLinkstate(ConnState::CONNECTED, DetailedState::NOTWORKING);
        staCallback.OnStaConnChanged(OperateResState::CONNECT_NETWORK_DISABLED, linkedInfo);
    }
}

/* --------------------------- state machine Connected State ------------------------------ */
StaStateMachine::LinkedState::LinkedState(StaStateMachine *staStateMachine)
    : State("LinkedState"), pStaStateMachine(staStateMachine)
{}

StaStateMachine::LinkedState::~LinkedState()
{}

void StaStateMachine::LinkedState::GoInState()
{
    WIFI_LOGI("LinkedState GoInState function.");

    return;
}

void StaStateMachine::LinkedState::GoOutState()
{
    WIFI_LOGI("LinkedState GoOutState function.");
}

bool StaStateMachine::LinkedState::ExecuteStateMsg(InternalMessage *msg)
{
    if (msg == nullptr) {
        WIFI_LOGI("msg is nullptr.");
        return false;
    }

    WIFI_LOGI("LinkedState, reveived msgCode=%{public}d msg.\n", msg->GetMessageName());
    bool ret = NOT_EXECUTED;
    switch (msg->GetMessageName()) {
        case WIFI_SVR_CMD_STA_BSSID_CHANGED_EVENT: {
            ret = EXECUTED;
            std::string reason = msg->GetStringFromMessage();
            std::string bssid = msg->GetStringFromMessage();
            WIFI_LOGI("reveived bssid changed event, reason:%{public}s,bssid:%{public}s.\n",
                reason.c_str(), MacAnonymize(bssid).c_str());
            if (strcmp(reason.c_str(), "ASSOC_COMPLETE") != 0) {
                WIFI_LOGE("Bssid change not for ASSOC_COMPLETE, do nothing.");
                return false;
            }
            if (WifiStaHalInterface::GetInstance().SetBssid(pStaStateMachine->linkedInfo.networkId,
                bssid) != WIFI_IDL_OPT_OK) {
                WIFI_LOGE("SetBssid return fail.");
                return false;
            }
            pStaStateMachine->isRoam = true;
            /* The current state of StaStateMachine transfers to pApRoamingState. */
            pStaStateMachine->SwitchState(pStaStateMachine->pApRoamingState);
            break;
        }
        default:
            WIFI_LOGE("NOT handle this event!");
            break;
    }

    return ret;
}

/* --------------------------- state machine Roaming State ------------------------------ */
StaStateMachine::ApRoamingState::ApRoamingState(StaStateMachine *staStateMachine)
    : State("ApRoamingState"), pStaStateMachine(staStateMachine)
{}

StaStateMachine::ApRoamingState::~ApRoamingState()
{}

void StaStateMachine::ApRoamingState::GoInState()
{
    WIFI_LOGI("ApRoamingState GoInState function.");
}

void StaStateMachine::ApRoamingState::GoOutState()
{
    WIFI_LOGI("ApRoamingState GoOutState function.");
}

bool StaStateMachine::ApRoamingState::ExecuteStateMsg(InternalMessage *msg)
{
    if (msg == nullptr) {
        return false;
    }

    WIFI_LOGI("ApRoamingState, reveived msgCode=%{public}d msg.", msg->GetMessageName());
    bool ret = NOT_EXECUTED;
    switch (msg->GetMessageName()) {
        case WIFI_SVR_CMD_STA_NETWORK_CONNECTION_EVENT: {
            WIFI_LOGI("ApRoamingState, receive WIFI_SVR_CMD_STA_NETWORK_CONNECTION_EVENT event.");
            ret = EXECUTED;
            pStaStateMachine->isRoam = true;
            pStaStateMachine->StopTimer(static_cast<int>(CMD_NETWORK_CONNECT_TIMEOUT));
            pStaStateMachine->ConnectToNetworkProcess(msg);
            /* Notify result to InterfaceService. */
            pStaStateMachine->staCallback.OnStaConnChanged(OperateResState::CONNECT_ASSOCIATED,
                pStaStateMachine->linkedInfo);
            pStaStateMachine->staCallback.OnStaConnChanged(OperateResState::CONNECT_OBTAINING_IP,
                pStaStateMachine->linkedInfo);

            /* The current state of StaStateMachine transfers to GetIpState. */
            pStaStateMachine->SwitchState(pStaStateMachine->pGetIpState);
            break;
        }
        default:
            WIFI_LOGI("ApRoamingState-msgCode=%d not handled.", msg->GetMessageName());
            break;
    }

    return ret;
}

void StaStateMachine::ConnectToNetworkProcess(InternalMessage *msg)
{
    if (msg == nullptr) {
        return;
    }
    
    lastNetworkId = msg->GetParam1();
    std::string bssid = msg->GetStringFromMessage();
    WIFI_LOGI("ConnectToNetworkProcess, Receive msg: lastNetworkId=%{public}d, bssid=%{public}s",
        lastNetworkId, MacAnonymize(bssid).c_str());
    WifiDeviceConfig deviceConfig;
    int result = WifiSettings::GetInstance().GetDeviceConfig(lastNetworkId, deviceConfig);
    WIFI_LOGI("Device config networkId = %{public}d", deviceConfig.networkId);

    if (result == 0 && deviceConfig.bssid == bssid) {
        LOGI("Device Configuration already exists.");
    } else {
        deviceConfig.bssid = bssid;
        if ((wpsState == SetupMethod::DISPLAY) || (wpsState == SetupMethod::PBC) || (wpsState == SetupMethod::KEYPAD)) {
            /* Save connection information. */
            WifiIdlGetDeviceConfig config;
            config.networkId = lastNetworkId;
            config.param = "ssid";
            if (WifiStaHalInterface::GetInstance().GetDeviceConfig(config) != WIFI_IDL_OPT_OK) {
                LOGE("GetDeviceConfig failed!");
            }

            deviceConfig.networkId = lastNetworkId;
            deviceConfig.bssid = bssid;
            deviceConfig.ssid = config.value;
            /* Remove the double quotation marks at the head and tail. */
            deviceConfig.ssid.erase(0, 1);
            deviceConfig.ssid.erase(deviceConfig.ssid.length() - 1, 1);
            WifiSettings::GetInstance().AddWpsDeviceConfig(deviceConfig);
            isWpsConnect = IsWpsConnected::WPS_CONNECTED;
        } else {
            WifiSettings::GetInstance().AddDeviceConfig(deviceConfig);
        }
        WifiSettings::GetInstance().SyncDeviceConfig();
        WIFI_LOGD("Device ssid = %s", SsidAnonymize(deviceConfig.ssid).c_str());
    }

    std::string macAddr;
    WifiSettings::GetInstance().GetMacAddress(macAddr);
    linkedInfo.networkId = lastNetworkId;
    linkedInfo.bssid = bssid;
    linkedInfo.ssid = deviceConfig.ssid;
    linkedInfo.macAddress = macAddr;
    linkedInfo.ifHiddenSSID = deviceConfig.hiddenSSID;
    lastLinkedInfo.bssid = bssid;
    lastLinkedInfo.macAddress = deviceConfig.macAddress;
    lastLinkedInfo.ifHiddenSSID = deviceConfig.hiddenSSID;
    SetWifiLinkedInfo(lastNetworkId);
    SaveLinkstate(ConnState::CONNECTING, DetailedState::OBTAINING_IPADDR);
}

void StaStateMachine::SetWifiLinkedInfo(int networkId)
{
    WIFI_LOGI("SetWifiLinkedInfo, linkedInfo.networkId=%{public}d, lastLinkedInfo.networkId=%{public}d",
        linkedInfo.networkId, lastLinkedInfo.networkId);
    if (linkedInfo.networkId == INVALID_NETWORK_ID) {
        if (lastLinkedInfo.networkId != INVALID_NETWORK_ID) {
            /* Update connection information according to the last connecting information. */
            linkedInfo.networkId = lastLinkedInfo.networkId;
            linkedInfo.ssid = lastLinkedInfo.ssid;
            linkedInfo.bssid = lastLinkedInfo.bssid;
            linkedInfo.macAddress = lastLinkedInfo.macAddress;
            linkedInfo.rssi = lastLinkedInfo.rssi;
            linkedInfo.band = lastLinkedInfo.band;
            linkedInfo.frequency = lastLinkedInfo.frequency;
            linkedInfo.linkSpeed = lastLinkedInfo.linkSpeed;
            linkedInfo.ipAddress = lastLinkedInfo.ipAddress;
            linkedInfo.connState = lastLinkedInfo.connState;
            linkedInfo.ifHiddenSSID = lastLinkedInfo.ifHiddenSSID;
            linkedInfo.rxLinkSpeed = lastLinkedInfo.rxLinkSpeed;
            linkedInfo.txLinkSpeed = lastLinkedInfo.txLinkSpeed;
            linkedInfo.chload = lastLinkedInfo.chload;
            linkedInfo.snr = lastLinkedInfo.snr;
            linkedInfo.isDataRestricted = lastLinkedInfo.isDataRestricted;
            linkedInfo.portalUrl = lastLinkedInfo.portalUrl;
            linkedInfo.detailedState = lastLinkedInfo.detailedState;
        } else if (networkId != INVALID_NETWORK_ID) {
            linkedInfo.networkId = networkId;
            WifiDeviceConfig config;
            int ret = WifiSettings::GetInstance().GetDeviceConfig(networkId, config);
            if (ret == 0) {
                /* Update connection information according to configuration. */
                linkedInfo.networkId = config.networkId;
                linkedInfo.ssid = config.ssid;
                linkedInfo.bssid = config.bssid;
                linkedInfo.band = config.band;
                linkedInfo.connState = ConnState::CONNECTING;
                linkedInfo.ifHiddenSSID = config.hiddenSSID;
                linkedInfo.detailedState = DetailedState::OBTAINING_IPADDR;

                lastLinkedInfo.networkId = config.networkId;
                lastLinkedInfo.ssid = config.ssid;
                lastLinkedInfo.bssid = config.bssid;
                lastLinkedInfo.band = config.band;
                lastLinkedInfo.connState = ConnState::CONNECTING;
                lastLinkedInfo.ifHiddenSSID = config.hiddenSSID;
                lastLinkedInfo.detailedState = DetailedState::OBTAINING_IPADDR;
            }
        }
    }
}

void StaStateMachine::DealNetworkCheck(InternalMessage *msg)
{
    LOGI("enter DealNetworkCheck.\n");
    if (msg == nullptr) {
        LOGE("InternalMessage msg is null.");
        return;
    }

    if (pNetcheck == nullptr) {
        LOGE("pNetcheck is null.");
        return;
    }
    pNetcheck->SignalNetCheckThread();
}

/* ------------------ state machine dhcp callback function ----------------- */

StaStateMachine::DhcpResultNotify::DhcpResultNotify(StaStateMachine *staStateMachine)
{
    pStaStateMachine = staStateMachine;
}

StaStateMachine::DhcpResultNotify::~DhcpResultNotify()
{}

void StaStateMachine::DhcpResultNotify::OnSuccess(int status, const std::string &ifname, DhcpResult &result)
{
    LOGI("Enter Sta DhcpResultNotify::OnSuccess. ifname=[%{public}s] status=[%{public}d]\n",
        ifname.c_str(), status);

    if ((pStaStateMachine->linkedInfo.detailedState == DetailedState::DISCONNECTING) ||
        (pStaStateMachine->linkedInfo.detailedState == DetailedState::DISCONNECTED)) {
        return;
    }
    WIFI_LOGI("iptype=%{public}d, ip=%{public}s, gateway=%{public}s, \
        subnet=%{public}s, serverAddress=%{public}s, leaseDuration=%{public}d.",
        result.iptype,
        IpAnonymize(result.strYourCli).c_str(),
        IpAnonymize(result.strRouter1).c_str(),
        IpAnonymize(result.strSubnet).c_str(),
        IpAnonymize(result.strServer).c_str(),
        result.uLeaseTime);
    WIFI_LOGI("strDns1=%{public}s, strDns2=%{public}s", IpAnonymize(result.strDns1).c_str(),
        IpAnonymize(result.strDns2).c_str());

    IpInfo ipInfo;
    WifiSettings::GetInstance().GetIpInfo(ipInfo);
    if (!((IpTools::ConvertIpv4Address(result.strYourCli) == ipInfo.ipAddress) &&
        (IpTools::ConvertIpv4Address(result.strRouter1) == ipInfo.gateway))) {
        result.strDns2 = WifiSettings::GetInstance().GetStrDnsBak();
        if (result.iptype == 0) {
            ipInfo.ipAddress = IpTools::ConvertIpv4Address(result.strYourCli);
            ipInfo.gateway = IpTools::ConvertIpv4Address(result.strRouter1);
            ipInfo.netmask = IpTools::ConvertIpv4Address(result.strSubnet);
            ipInfo.primaryDns = IpTools::ConvertIpv4Address(result.strDns1);
            ipInfo.secondDns = IpTools::ConvertIpv4Address(result.strDns2);
            ipInfo.serverIp = IpTools::ConvertIpv4Address(result.strServer);
            ipInfo.leaseDuration = result.uLeaseTime;
            WifiSettings::GetInstance().SaveIpInfo(ipInfo);
            pStaStateMachine->linkedInfo.ipAddress = IpTools::ConvertIpv4Address(result.strYourCli);
            pStaStateMachine->linkedInfo.isDataRestricted =
                (result.strVendor.find("ANDROID_METERED") == std::string::npos) ? 0 : 1;
            WifiSettings::GetInstance().SaveLinkedInfo(pStaStateMachine->linkedInfo);
#ifndef OHOS_ARCH_LITE
            WIFI_LOGI("Update NetLink info, strYourCli=%{public}s, strSubnet=%{public}s, \
                strRouter1=%{public}s, strDns1=%{public}s, strDns2=%{public}s",
                IpAnonymize(result.strYourCli).c_str(), IpAnonymize(result.strSubnet).c_str(),
                IpAnonymize(result.strRouter1).c_str(), IpAnonymize(result.strDns1).c_str(),
                IpAnonymize(result.strDns2).c_str());
            WIFI_LOGI("On dhcp success update net linke info");
            WifiNetAgent::GetInstance().OnStaMachineUpdateNetLinkInfo(result.strYourCli, result.strSubnet,
                result.strRouter1, result.strDns1, result.strDns2);
#endif
        }
#ifdef OHOS_ARCH_LITE
        IfConfig::GetInstance().SetIfDnsAndRoute(result, result.iptype);
#endif
    }

    WIFI_LOGI("DhcpResultNotify::OnSuccess, getIpSucNum=%{public}d, isRoam=%{public}d",
        pStaStateMachine->getIpSucNum, pStaStateMachine->isRoam);
    pStaStateMachine->OnDhcpResultNotifyEvent(true);
    if (pStaStateMachine->getIpSucNum == 0 || pStaStateMachine->isRoam) {
        pStaStateMachine->SaveLinkstate(ConnState::CONNECTED, DetailedState::CONNECTED);
        pStaStateMachine->staCallback.OnStaConnChanged(
            OperateResState::CONNECT_AP_CONNECTED, pStaStateMachine->linkedInfo);
        /* Delay to wait for the network adapter information to take effect. */
        constexpr int NETCHECK_DELAY_TIME = 2000; // 2000 ms
        pStaStateMachine->StartTimer(static_cast<int>(CMD_START_NETCHECK), NETCHECK_DELAY_TIME);
    }
    pStaStateMachine->getIpSucNum++;

    WIFI_LOGI("DhcpResultNotify::OnSuccess, stop dhcp client");
    if (pStaStateMachine->pDhcpService != nullptr) {
        if (pStaStateMachine->currentTpType == IPTYPE_IPV6) {
            pStaStateMachine->pDhcpService->StopDhcpClient(IF_NAME, true);
        } else {
            pStaStateMachine->pDhcpService->StopDhcpClient(IF_NAME, false);
        }
    }
    return;
}

void StaStateMachine::DhcpResultNotify::OnFailed(int status, const std::string &ifname, const std::string &reason)
{
    LOGI("Enter DhcpResultNotify::OnFailed. ifname=[%s] status=[%d], reason = [%s], detailedState = [%d]\n",
        ifname.c_str(), status, reason.c_str(), static_cast<int>(pStaStateMachine->linkedInfo.detailedState));
    if ((pStaStateMachine->linkedInfo.detailedState == DetailedState::DISCONNECTING) ||
        (pStaStateMachine->linkedInfo.detailedState == DetailedState::DISCONNECTED)) {
        return;
    }

    LOGI("currentTpType: %{public}d, getIpSucNum: %{public}d, getIpFailNum: %{public}d, isRoam: %{public}d",
        pStaStateMachine->currentTpType, pStaStateMachine->getIpSucNum,
        pStaStateMachine->getIpFailNum, pStaStateMachine->isRoam);

    if (pStaStateMachine->getIpFailNum == 0) {
        pStaStateMachine->staCallback.OnStaConnChanged(OperateResState::CONNECT_OBTAINING_IP_FAILED,
            pStaStateMachine->linkedInfo);
        pStaStateMachine->DisConnectProcess();
        pStaStateMachine->SaveLinkstate(ConnState::DISCONNECTED, DetailedState::OBTAINING_IPADDR_FAIL);
        pStaStateMachine->staCallback.OnStaConnChanged(OperateResState::DISCONNECT_DISCONNECTED,
            pStaStateMachine->linkedInfo);
    }
    pStaStateMachine->getIpFailNum++;
}

void StaStateMachine::DhcpResultNotify::OnSerExitNotify(const std::string &ifname)
{
    LOGI("Enter DhcpResultNotify::OnSerExitNotify. ifname = [%s]\n", ifname.c_str());
}

/* ------------------ state machine Comment function ----------------- */
void StaStateMachine::SaveLinkstate(ConnState state, DetailedState detailState)
{
    linkedInfo.connState = state;
    linkedInfo.detailedState = detailState;
    lastLinkedInfo.connState = state;
    lastLinkedInfo.detailedState = detailState;
    WifiSettings::GetInstance().SaveLinkedInfo(linkedInfo);
}

int StaStateMachine::GetLinkedInfo(WifiLinkedInfo& linkedInfo)
{
    return WifiSettings::GetInstance().GetLinkedInfo(linkedInfo);
}

ErrCode StaStateMachine::DisableNetwork(int networkId)
{
    if (WifiStaHalInterface::GetInstance().DisableNetwork(networkId) != WIFI_IDL_OPT_OK) {
        LOGE("DisableNetwork() failed, networkId=%d.", networkId);
        return WIFI_OPT_FAILED;
    }

    if (WifiStaHalInterface::GetInstance().SaveDeviceConfig() != WIFI_IDL_OPT_OK) {
        LOGE("DisableNetwork-SaveDeviceConfig() failed!");
        return WIFI_OPT_FAILED;
    }
    LOGI("DisableNetwork-SaveDeviceConfig() succeed!");
    return WIFI_OPT_SUCCESS;
}

void StaStateMachine::SetOperationalMode(int mode)
{
    SendMessage(WIFI_SVR_CMD_STA_OPERATIONAL_MODE, mode, 0);
}

#ifndef OHOS_ARCH_LITE
void StaStateMachine::SubscribeSystemAbilityChanged(void)
{
    auto samgrProxy = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    statusChangeListener_ = new (std::nothrow) SystemAbilityStatusChangeListener(*this);
    if (samgrProxy == nullptr || statusChangeListener_ == nullptr) {
        LOGE("samgrProxy or statusChangeListener_ is nullptr");
        return;
    }
    int32_t ret = samgrProxy->SubscribeSystemAbility(COMM_NET_CONN_MANAGER_SYS_ABILITY_ID, statusChangeListener_);
    LOGI("SubscribeSystemAbility COMM_NET_CONN_MANAGER_SYS_ABILITY_ID result:%{public}d", ret);
}

void StaStateMachine::OnNetManagerRestart(void)
{
    LOGI("OnNetManagerRestart()");
    int state = WifiSettings::GetInstance().GetWifiState();
    if (state != static_cast<int>(WifiState::ENABLED)) {
        return;
    }
    WifiNetAgent::GetInstance().OnStaMachineNetManagerRestart(NetSupplierInfo, staCallback);
}

void StaStateMachine::ReUpdateNetSupplierInfo(sptr<NetManagerStandard::NetSupplierInfo> supplierInfo)
{
    LOGI("ReUpdateNetSupplierInfo()");
    WifiLinkedInfo linkedInfo;
    WifiSettings::GetInstance().GetLinkedInfo(linkedInfo);
    if ((linkedInfo.detailedState == DetailedState::NOTWORKING) && (linkedInfo.connState == ConnState::CONNECTED)) {
        if (supplierInfo != nullptr) {
            TimeStats timeStats("Call UpdateNetSupplierInfo");
            WifiNetAgent::GetInstance().UpdateNetSupplierInfo(supplierInfo);
        }
    }
}

void StaStateMachine::ReUpdateNetLinkInfo(void)
{
    LOGI("ReUpdateNetLinkInfo()");
    WifiLinkedInfo linkedInfo;
    WifiSettings::GetInstance().GetLinkedInfo(linkedInfo);
    if ((linkedInfo.detailedState == DetailedState::NOTWORKING) && (linkedInfo.connState == ConnState::CONNECTED)) {
        IpInfo wifiIpInfo;
        WifiSettings::GetInstance().GetIpInfo(wifiIpInfo);
        std::string ipAddress = IpTools::ConvertIpv4Address(wifiIpInfo.ipAddress);
        std::string gateway = IpTools::ConvertIpv4Address(wifiIpInfo.gateway);
        std::string netmask = IpTools::ConvertIpv4Address(wifiIpInfo.netmask);
        std::string primaryDns = IpTools::ConvertIpv4Address(wifiIpInfo.primaryDns);
        std::string secondDns = IpTools::ConvertIpv4Address(wifiIpInfo.secondDns);
        WifiNetAgent::GetInstance().UpdateNetLinkInfo(ipAddress, netmask, gateway, primaryDns, secondDns);
    }
}

StaStateMachine::SystemAbilityStatusChangeListener::SystemAbilityStatusChangeListener(StaStateMachine &stateMachine)
    : stateMachine_(stateMachine){};

void StaStateMachine::SystemAbilityStatusChangeListener::OnAddSystemAbility(int32_t systemAbilityId,
                                                                            const std::string &deviceId)
{
    LOGI("OnAddSystemAbility() systemAbilityId:%{public}d", systemAbilityId);
    if (systemAbilityId == COMM_NET_CONN_MANAGER_SYS_ABILITY_ID) {
        if (!hasSARemoved_) {
            return;
        }
        hasSARemoved_ = false;
        stateMachine_.OnNetManagerRestart();
    }
}

void StaStateMachine::SystemAbilityStatusChangeListener::OnRemoveSystemAbility(int32_t systemAbilityId,
                                                                               const std::string &deviceId)
{
    LOGI("OnRemoveSystemAbility() systemAbilityId:%{public}d", systemAbilityId);
    if (systemAbilityId == COMM_NET_CONN_MANAGER_SYS_ABILITY_ID) {
        hasSARemoved_ = true;
    }
}
#endif // OHOS_ARCH_LITE
} // namespace Wifi
} // namespace OHOS
