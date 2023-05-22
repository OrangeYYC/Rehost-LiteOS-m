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

#include "wifi_manager.h"
#include <dirent.h>
#include "wifi_auth_center.h"
#include "wifi_chip_hal_interface.h"
#include "wifi_common_event_helper.h"
#include "wifi_config_center.h"
#include "wifi_global_func.h"
#include "wifi_logger.h"
#ifdef OHOS_ARCH_LITE
#include "wifi_internal_event_dispatcher_lite.h"
#else
#include "wifi_internal_event_dispatcher.h"
#include "common_event_manager.h"
#include "common_event_support.h"
#endif
#include "wifi_sta_hal_interface.h"
#include "wifi_service_manager.h"
#include "wifi_settings.h"


namespace OHOS {
namespace Wifi {
DEFINE_WIFILOG_LABEL("WifiManager");
int WifiManager::mCloseApIndex = 0;
#ifndef OHOS_ARCH_LITE
const uint32_t TIMEOUT_SCREEN_EVENT = 3000;
#endif

WifiManager &WifiManager::GetInstance()
{
    static WifiManager gWifiManager;
    static std::mutex gInitMutex;
    if (gWifiManager.GetInitStatus() == INIT_UNKNOWN) {
        std::unique_lock<std::mutex> lock(gInitMutex);
        if (gWifiManager.GetInitStatus() == INIT_UNKNOWN) {
            if (gWifiManager.Init() != 0) {
                WIFI_LOGE("Failed to `WifiManager::Init` !");
            }
        }
    }

    return gWifiManager;
}

WifiManager::WifiManager() : mInitStatus(INIT_UNKNOWN), mSupportedFeatures(0)
{}

WifiManager::~WifiManager()
{}

void WifiManager::AutoStartStaService(void)
{
    WIFI_LOGI("AutoStartStaService");
    WifiOprMidState staState = WifiConfigCenter::GetInstance().GetWifiMidState();
    if (staState == WifiOprMidState::CLOSED) {
        if (!WifiConfigCenter::GetInstance().SetWifiMidState(staState, WifiOprMidState::OPENING)) {
            WIFI_LOGW("set sta mid state opening failed! may be other activity has been operated");
            return;
        }
        ErrCode errCode = WIFI_OPT_FAILED;
        do {
            if (WifiServiceManager::GetInstance().CheckAndEnforceService(WIFI_SERVICE_STA) < 0) {
                WIFI_LOGE("Load %{public}s service failed!", WIFI_SERVICE_STA);
                break;
            }
            IStaService *pService = WifiServiceManager::GetInstance().GetStaServiceInst();
            if (pService == nullptr) {
                WIFI_LOGE("Create %{public}s service failed!", WIFI_SERVICE_STA);
                break;
            }
            errCode = pService->RegisterStaServiceCallback(WifiManager::GetInstance().GetStaCallback());
            if (errCode != WIFI_OPT_SUCCESS) {
                WIFI_LOGE("Register sta service callback failed!");
                break;
            }
            errCode = pService->EnableWifi();
            if (errCode != WIFI_OPT_SUCCESS) {
                WIFI_LOGE("service enable sta failed, ret %{public}d!", static_cast<int>(errCode));
                break;
            }
        } while (0);
        if (errCode != WIFI_OPT_SUCCESS) {
            WifiConfigCenter::GetInstance().SetWifiMidState(WifiOprMidState::OPENING, WifiOprMidState::CLOSED);
            WifiServiceManager::GetInstance().UnloadService(WIFI_SERVICE_STA);
        }
    }
    return;
}

void WifiManager::ForceStopWifi(void)
{
    WIFI_LOGI("Enter ForceStopWifi");
    IStaService *pService = WifiServiceManager::GetInstance().GetStaServiceInst();
    if (pService == nullptr || (pService->DisableWifi() != WIFI_OPT_SUCCESS)) {
        WIFI_LOGE("service is null or disable wifi failed.");
        WifiConfigCenter::GetInstance().SetWifiMidState(WifiOprMidState::CLOSED);
        WifiServiceManager::GetInstance().UnloadService(WIFI_SERVICE_STA);
        return;
    }
    WifiOprMidState curState = WifiConfigCenter::GetInstance().GetWifiMidState();
    WIFI_LOGI("In force stop wifi, state: %{public}d", static_cast<int>(curState));
    WifiConfigCenter::GetInstance().SetWifiMidState(curState, WifiOprMidState::CLOSED);
}

void WifiManager::CheckAndStartSta(void)
{
    DIR *dir = nullptr;
    struct dirent *dent = nullptr;
    int currentWaitTime = 0;
    const int sleepTime = 1;
    const int maxWaitTimes = 30;

    while (currentWaitTime < maxWaitTimes) {
        dir = opendir("/sys/class/net");
        if (dir == nullptr) {
            AutoStartStaService();
            return;
        }
        while ((dent = readdir(dir)) != nullptr) {
            if (dent->d_name[0] == '.') {
                continue;
            }
            if (strncmp(dent->d_name, "wlan", strlen("wlan")) == 0) {
                closedir(dir);
                AutoStartStaService();
                return;
            }
        }
        closedir(dir);
        sleep(sleepTime);
        currentWaitTime++;
    }
    AutoStartStaService();
}

void WifiManager::AutoStartServiceThread(void)
{
    WIFI_LOGI("Auto start service...");
    CheckAndStartSta();
#ifdef FEATURE_P2P_SUPPORT
    AutoStartP2pService();
#endif
}

#ifdef FEATURE_P2P_SUPPORT
WifiCfgMonitorEventCallback WifiManager::cfgMonitorCallback = {
    nullptr,
};

void WifiManager::AutoStartP2pService(void)
{
    WIFI_LOGI("AutoStartP2pService");
    WifiOprMidState p2pState = WifiConfigCenter::GetInstance().GetP2pMidState();
    if (p2pState == WifiOprMidState::CLOSED) {
        if (!WifiConfigCenter::GetInstance().SetP2pMidState(p2pState, WifiOprMidState::OPENING)) {
            WIFI_LOGW("set p2p mid state opening failed!");
            return;
        }
    }
    ErrCode ret = WIFI_OPT_FAILED;
    do {
        if (WifiServiceManager::GetInstance().CheckAndEnforceService(WIFI_SERVICE_P2P) < 0) {
            WIFI_LOGE("Load %{public}s service failed!", WIFI_SERVICE_P2P);
            break;
        }
        IP2pService *pService = WifiServiceManager::GetInstance().GetP2pServiceInst();
        if (pService == nullptr) {
            WIFI_LOGE("Create %{public}s service failed!", WIFI_SERVICE_P2P);
            break;
        }
        ret = pService->RegisterP2pServiceCallbacks(WifiManager::GetInstance().GetP2pCallback());
        if (ret != WIFI_OPT_SUCCESS) {
            WIFI_LOGE("Register p2p service callback failed!");
            break;
        }
        ret = pService->EnableP2p();
        if (ret != WIFI_OPT_SUCCESS) {
            WIFI_LOGE("service EnableP2p failed, ret %{public}d!", static_cast<int>(ret));
            break;
        }
    } while (false);
    if (ret != WIFI_OPT_SUCCESS) {
        WifiConfigCenter::GetInstance().SetP2pMidState(WifiOprMidState::OPENING, WifiOprMidState::CLOSED);
        WifiServiceManager::GetInstance().UnloadService(WIFI_SERVICE_P2P);
    }
    return;
}
#endif

void WifiManager::AutoStartScanService(void)
{
    WIFI_LOGI("AutoStartScanService");
    if (!WifiConfigCenter::GetInstance().IsScanAlwaysActive()) {
        WIFI_LOGW("Scan always is not open, not open scan service.");
        return;
    }
    ScanControlInfo info;
    WifiConfigCenter::GetInstance().GetScanControlInfo(info);
    if (!IsAllowScanAnyTime(info)) {
        WIFI_LOGW("Scan control does not support scan always, not open scan service here.");
        return;
    }
    CheckAndStartScanService();
    return;
}

int WifiManager::Init()
{
    if (WifiConfigCenter::GetInstance().Init() < 0) {
        WIFI_LOGE("WifiConfigCenter Init failed!");
        mInitStatus = CONFIG_CENTER_INIT_FAILED;
        return -1;
    }
    if (WifiAuthCenter::GetInstance().Init() < 0) {
        WIFI_LOGE("WifiAuthCenter Init failed!");
        mInitStatus = AUTH_CENTER_INIT_FAILED;
        return -1;
    }
    if (WifiServiceManager::GetInstance().Init() < 0) {
        WIFI_LOGE("WifiServiceManager Init failed!");
        mInitStatus = SERVICE_MANAGER_INIT_FAILED;
        return -1;
    }
    if (WifiInternalEventDispatcher::GetInstance().Init() < 0) {
        WIFI_LOGE("WifiInternalEventDispatcher Init failed!");
        mInitStatus = EVENT_BROADCAST_INIT_FAILED;
        return -1;
    }
    mCloseServiceThread = std::thread(WifiManager::DealCloseServiceMsg, std::ref(*this));
    
#ifndef OHOS_ARCH_LITE
    if (screenEventSubscriber_ == nullptr) {
        lpScreenTimer_ = std::make_unique<Utils::Timer>("WifiManager");
        using TimeOutCallback = std::function<void()>;
        TimeOutCallback timeoutCallback = std::bind(&WifiManager::RegisterScreenEvent, this);
        if (lpScreenTimer_ != nullptr) {
            lpScreenTimer_->Setup();
            lpScreenTimer_->Register(timeoutCallback, TIMEOUT_SCREEN_EVENT, true);
        } else {
            WIFI_LOGE("lpScreenTimer_ is nullptr");
        }
    }
#endif
    mInitStatus = INIT_OK;
    InitStaCallback();
    InitScanCallback();
#ifdef FEATURE_AP_SUPPORT
    InitApCallback();
#endif
#ifdef FEATURE_P2P_SUPPORT
    InitP2pCallback();
#endif
    if (WifiServiceManager::GetInstance().CheckPreLoadService() < 0) {
        WIFI_LOGE("WifiServiceManager check preload feature service failed!");
        WifiManager::GetInstance().Exit();
        return -1;
    }
    if (WifiConfigCenter::GetInstance().GetStaLastRunState()) { /* Automatic startup upon startup */
        WIFI_LOGI("AutoStartServiceThread");
        std::thread startStaSrvThread(WifiManager::AutoStartServiceThread);
        startStaSrvThread.detach();
    } else {
        /**
         * The sta service automatically starts upon startup. After the sta
         * service is started, the scanning is directly started.
         */
        AutoStartScanService();
    }
    return 0;
}

void WifiManager::Exit()
{
    WIFI_LOGI("[WifiManager] Exit.");
    WifiServiceManager::GetInstance().UninstallAllService();
    /* NOTE:: DO NOT allow call hal interface function, delete at 2022.10.16 */
    /* Refer to WifiStaHalInterface::GetInstance().ExitAllIdlClient(); */
    WifiInternalEventDispatcher::GetInstance().Exit();
    if (mCloseServiceThread.joinable()) {
        PushServiceCloseMsg(WifiCloseServiceCode::SERVICE_THREAD_EXIT);
        mCloseServiceThread.join();
    }
#ifndef OHOS_ARCH_LITE
    if (screenEventSubscriber_ != nullptr) {
        UnRegisterScreenEvent();
    }
    if (lpScreenTimer_ != nullptr) {
        lpScreenTimer_->Shutdown(false);
        lpScreenTimer_ = nullptr;
    }
#endif
    return;
}

void WifiManager::PushServiceCloseMsg(WifiCloseServiceCode code)
{
    std::unique_lock<std::mutex> lock(mMutex);
    mEventQue.push_back(code);
    mCondition.notify_one();
    return;
}

void WifiManager::AddSupportedFeatures(WifiFeatures feature)
{
    mSupportedFeatures |= static_cast<long>(feature);
}

int WifiManager::GetSupportedFeatures(long &features) const
{
    long supportedFeatures = mSupportedFeatures;
    supportedFeatures |= static_cast<long>(WifiFeatures::WIFI_FEATURE_INFRA);
    supportedFeatures |= static_cast<long>(WifiFeatures::WIFI_FEATURE_INFRA_5G);
    supportedFeatures |= static_cast<long>(WifiFeatures::WIFI_FEATURE_PASSPOINT);
    supportedFeatures |= static_cast<long>(WifiFeatures::WIFI_FEATURE_AP_STA);
    supportedFeatures |= static_cast<long>(WifiFeatures::WIFI_FEATURE_WPA3_SAE);
    supportedFeatures |= static_cast<long>(WifiFeatures::WIFI_FEATURE_WPA3_SUITE_B);
    supportedFeatures |= static_cast<long>(WifiFeatures::WIFI_FEATURE_OWE);
    /* NOTE: features = supportedFeatures & WifiChipHalInterface::GetInstance().GetChipCapabilities */
    /* It does NOT allow call HalInterface from wifi_manager */
    features = supportedFeatures;
    return 0;
}

InitStatus WifiManager::GetInitStatus()
{
    return mInitStatus;
}

void WifiManager::CloseStaService(void)
{
    WIFI_LOGI("close sta service");
    WifiServiceManager::GetInstance().UnloadService(WIFI_SERVICE_STA);
    WifiConfigCenter::GetInstance().SetWifiMidState(WifiOprMidState::CLOSED);
    WifiConfigCenter::GetInstance().SetWifiStaCloseTime();
    return;
}

#ifdef FEATURE_AP_SUPPORT
void WifiManager::CloseApService(int id)
{
    WIFI_LOGI("close %{public}d ap service", id);
    WifiServiceManager::GetInstance().UnloadService(WIFI_SERVICE_AP, id);
    WifiConfigCenter::GetInstance().SetApMidState(WifiOprMidState::CLOSED, id);
    WifiSettings::GetInstance().SetHotspotState(static_cast<int>(ApState::AP_STATE_CLOSED), id);
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_HOTSPOT_STATE_CHANGE;
    cbMsg.msgData = static_cast<int>(ApState::AP_STATE_CLOSED);
    cbMsg.id = id;
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    return;
}
#endif

void WifiManager::CloseScanService(void)
{
    WIFI_LOGI("close scan service");
    WifiServiceManager::GetInstance().UnloadService(WIFI_SERVICE_SCAN);
    WifiConfigCenter::GetInstance().SetScanMidState(WifiOprMidState::CLOSED);
    return;
}

#ifdef FEATURE_P2P_SUPPORT
void WifiManager::CloseP2pService(void)
{
    WIFI_LOGD("close p2p service");
    WifiServiceManager::GetInstance().UnloadService(WIFI_SERVICE_P2P);
    WifiConfigCenter::GetInstance().SetP2pMidState(WifiOprMidState::CLOSED);
    WifiSettings::GetInstance().SetP2pState(static_cast<int>(P2pState::P2P_STATE_CLOSED));
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_P2P_STATE_CHANGE;
    cbMsg.msgData = static_cast<int>(P2pState::P2P_STATE_CLOSED);
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    return;
}
#endif

void WifiManager::DealCloseServiceMsg(WifiManager &manager)
{
    const int waitDealTime = 10 * 1000; /* 10 ms */
    while (true) {
        std::unique_lock<std::mutex> lock(manager.mMutex);
        while (manager.mEventQue.empty()) {
            manager.mCondition.wait(lock);
        }
        WifiCloseServiceCode msg = manager.mEventQue.front();
        manager.mEventQue.pop_front();
        lock.unlock();
        usleep(waitDealTime);
        switch (msg) {
            case WifiCloseServiceCode::STA_SERVICE_CLOSE:
                CloseStaService();
                break;
            case WifiCloseServiceCode::SCAN_SERVICE_CLOSE:
                CloseScanService();
                break;
#ifdef FEATURE_AP_SUPPORT
            case WifiCloseServiceCode::AP_SERVICE_CLOSE:
                CloseApService(mCloseApIndex);
                break;
#endif
#ifdef FEATURE_P2P_SUPPORT
            case WifiCloseServiceCode::P2P_SERVICE_CLOSE:
                CloseP2pService();
                break;
#endif
            case WifiCloseServiceCode::SERVICE_THREAD_EXIT:
                WIFI_LOGI("DealCloseServiceMsg thread exit!");
                return;
            default:
                WIFI_LOGW("Unknown message code, %{public}d", static_cast<int>(msg));
                break;
        }
    }
    WIFI_LOGD("WifiManager Thread exit");
    return;
}

void WifiManager::InitStaCallback(void)
{
    mStaCallback.OnStaOpenRes = DealStaOpenRes;
    mStaCallback.OnStaCloseRes = DealStaCloseRes;
    mStaCallback.OnStaConnChanged = DealStaConnChanged;
    mStaCallback.OnWpsChanged = DealWpsChanged;
    mStaCallback.OnStaStreamChanged = DealStreamChanged;
    mStaCallback.OnStaRssiLevelChanged = DealRssiChanged;
    return;
}

StaServiceCallback WifiManager::GetStaCallback()
{
    return mStaCallback;
}

void WifiManager::DealStaOpenRes(OperateResState state)
{
    WIFI_LOGI("Enter DealStaOpenRes: %{public}d", static_cast<int>(state));
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_STATE_CHANGE;
    if (state == OperateResState::OPEN_WIFI_OPENING) {
        cbMsg.msgData = static_cast<int>(WifiState::ENABLING);
        WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
        return;
    }
    if ((state == OperateResState::OPEN_WIFI_FAILED) || (state == OperateResState::OPEN_WIFI_DISABLED)) {
        WIFI_LOGE("DealStaOpenRes:wifi open failed!");
        WifiConfigCenter::GetInstance().SetWifiMidState(WifiOprMidState::OPENING, WifiOprMidState::CLOSED);
        DealStaCloseRes(state);
        return;
    }

    WIFI_LOGI("DealStaOpenRes:wifi open successfully!");
    WifiConfigCenter::GetInstance().SetWifiMidState(WifiOprMidState::OPENING, WifiOprMidState::RUNNING);
    WifiConfigCenter::GetInstance().SetStaLastRunState(true);
    if (WifiConfigCenter::GetInstance().GetAirplaneModeState() == 1) {
        WifiConfigCenter::GetInstance().SetWifiStateWhenAirplaneMode(true);
    }
    cbMsg.msgData = static_cast<int>(WifiState::ENABLED);
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    CheckAndStartScanService();
}

void WifiManager::DealStaCloseRes(OperateResState state)
{
    WIFI_LOGI("Enter DealStaCloseRes: %{public}d", static_cast<int>(state));
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_STATE_CHANGE;
    if (state == OperateResState::CLOSE_WIFI_CLOSING) {
        cbMsg.msgData = static_cast<int>(WifiState::DISABLING);
        WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
        return;
    }
    if (state == OperateResState::CLOSE_WIFI_FAILED) {
        WIFI_LOGI("DealStaCloseRes: broadcast wifi close failed event!");
        cbMsg.msgData = static_cast<int>(WifiState::UNKNOWN);
        WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
        ForceStopWifi();
    }
    if (WifiConfigCenter::GetInstance().GetAirplaneModeState() == 1) {
        WifiConfigCenter::GetInstance().SetWifiStateWhenAirplaneMode(false);
    }
    cbMsg.msgData = static_cast<int>(WifiState::DISABLED);
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    CheckAndStopScanService();
    WifiManager::GetInstance().PushServiceCloseMsg(WifiCloseServiceCode::STA_SERVICE_CLOSE);
    return;
}

void WifiManager::DealStaConnChanged(OperateResState state, const WifiLinkedInfo &info)
{
    WIFI_LOGI("Enter, DealStaConnChanged, state: %{public}d!\n", static_cast<int>(state));
    bool isReport = true;
    int reportStateNum = static_cast<int>(ConvertConnStateInternal(state, isReport));
    if (isReport) {
        WifiEventCallbackMsg cbMsg;
        cbMsg.msgCode = WIFI_CBK_MSG_CONNECTION_CHANGE;
        cbMsg.msgData = reportStateNum;
        cbMsg.linkInfo = info;
        WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    }

    if (state == OperateResState::CONNECT_CONNECTING || state == OperateResState::CONNECT_AP_CONNECTED ||
        state == OperateResState::DISCONNECT_DISCONNECTING || state == OperateResState::DISCONNECT_DISCONNECTED ||
        state == OperateResState::CONNECT_OBTAINING_IP || state == OperateResState::CONNECT_ASSOCIATING ||
        state == OperateResState::CONNECT_ASSOCIATED) {
        if (WifiConfigCenter::GetInstance().GetScanMidState() == WifiOprMidState::RUNNING) {
            IScanService *pService = WifiServiceManager::GetInstance().GetScanServiceInst();
            if (pService != nullptr) {
                pService->OnClientModeStatusChanged(static_cast<int>(state));
            }
        }
    }
#ifdef FEATURE_P2P_SUPPORT
    if (cfgMonitorCallback.onStaConnectionChange != nullptr) {
        cfgMonitorCallback.onStaConnectionChange(static_cast<int>(state));
    }
#endif
    return;
}

void WifiManager::DealWpsChanged(WpsStartState state, const int pinCode)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_WPS_STATE_CHANGE;
    cbMsg.msgData = static_cast<int>(state);
    cbMsg.pinCode = std::to_string(pinCode);
    int len = cbMsg.pinCode.length();
    if (len < 8) { /* Fill in 8 digits. */
        cbMsg.pinCode = std::string(8 - len, '0') + cbMsg.pinCode;
    }
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    return;
}

void WifiManager::DealStreamChanged(StreamDirection direction)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_STREAM_DIRECTION;
    cbMsg.msgData = static_cast<int>(direction);
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    return;
}

void WifiManager::DealRssiChanged(int rssi)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_RSSI_CHANGE;
    cbMsg.msgData = rssi;
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    return;
}

void WifiManager::CheckAndStartScanService(void)
{
    WifiOprMidState scanState = WifiConfigCenter::GetInstance().GetScanMidState();
    WIFI_LOGI("CheckAndStartScanService scanState: %{public}d", static_cast<int>(scanState));
    if (scanState != WifiOprMidState::CLOSED) {
        /* If the scanning function is enabled when the STA is not enabled, you need to start the scheduled
             scanning function immediately when the STA is enabled. */
        IScanService *pService = WifiServiceManager::GetInstance().GetScanServiceInst();
        if (pService != nullptr) {
            pService->OnClientModeStatusChanged(static_cast<int>(OperateResState::DISCONNECT_DISCONNECTED));
        }
        return;
    }
    if (!WifiConfigCenter::GetInstance().SetScanMidState(scanState, WifiOprMidState::OPENING)) {
        WIFI_LOGW("Failed to set scan mid state opening! may be other activity has been operated");
        return;
    }
    ErrCode errCode = WIFI_OPT_FAILED;
    do {
        if (WifiServiceManager::GetInstance().CheckAndEnforceService(WIFI_SERVICE_SCAN) < 0) {
            WIFI_LOGE("Load %{public}s service failed!", WIFI_SERVICE_SCAN);
            break;
        }
        IScanService *pService = WifiServiceManager::GetInstance().GetScanServiceInst();
        if (pService == nullptr) {
            WIFI_LOGE("Create %{public}s service failed!", WIFI_SERVICE_SCAN);
            break;
        }
        errCode = pService->RegisterScanCallbacks(WifiManager::GetInstance().GetScanCallback());
        if (errCode != WIFI_OPT_SUCCESS) {
            WIFI_LOGE("Register scan service callback failed!");
            break;
        }
        errCode = pService->Init();
        if (errCode != WIFI_OPT_SUCCESS) {
            WIFI_LOGE("init scan service failed, ret %{public}d!", static_cast<int>(errCode));
            break;
        }
    } while (0);
    if (errCode != WIFI_OPT_SUCCESS) {
        WifiConfigCenter::GetInstance().SetScanMidState(WifiOprMidState::OPENING, WifiOprMidState::CLOSED);
        WifiServiceManager::GetInstance().UnloadService(WIFI_SERVICE_SCAN);
    }
    return;
}

void WifiManager::CheckAndStopScanService(void)
{
    /**
     * Check unload SCAN service
     * When anytime scanning is enabled and the control policy allows, airplane
     * mode and power saving mode are disabled.   --- Do not disable the scan
     * service. Otherwise, disable the SCAN service.
     */
    WifiOprMidState scanState = WifiConfigCenter::GetInstance().GetScanMidState();
    WIFI_LOGI("[CheckAndStopScanService] scanState %{public}d!", static_cast<int>(scanState));
    if (scanState != WifiOprMidState::OPENING && scanState != WifiOprMidState::RUNNING) {
        return;
    }
    ScanControlInfo info;
    WifiConfigCenter::GetInstance().GetScanControlInfo(info);
    if (WifiConfigCenter::GetInstance().IsScanAlwaysActive() && IsAllowScanAnyTime(info) &&
        WifiConfigCenter::GetInstance().GetAirplaneModeState() == MODE_STATE_CLOSE &&
        WifiConfigCenter::GetInstance().GetPowerSavingModeState() == MODE_STATE_CLOSE) {
        return;
    }
    /* After check condition over, begin unload SCAN service */
    if (WifiConfigCenter::GetInstance().SetScanMidState(scanState, WifiOprMidState::CLOSING)) {
        IScanService *pService = WifiServiceManager::GetInstance().GetScanServiceInst();
        if (pService == nullptr) {
            WIFI_LOGE("[CheckAndStopScanService] scan service is null.");
            WifiManager::GetInstance().PushServiceCloseMsg(WifiCloseServiceCode::SCAN_SERVICE_CLOSE);
            WifiConfigCenter::GetInstance().SetScanMidState(scanState, WifiOprMidState::CLOSED);
            return;
        }
        ErrCode ret = pService->UnInit();
        if (ret != WIFI_OPT_SUCCESS) { // scan service is not exist
            WIFI_LOGE("[CheckAndStopScanService] UnInit service failed!");
        }
        WifiConfigCenter::GetInstance().SetScanMidState(scanState, WifiOprMidState::CLOSED);
    }
}

void WifiManager::InitScanCallback(void)
{
    mScanCallback.OnScanStartEvent = DealScanOpenRes;
    mScanCallback.OnScanStopEvent = DealScanCloseRes;
    mScanCallback.OnScanFinishEvent = DealScanFinished;
    mScanCallback.OnScanInfoEvent = DealScanInfoNotify;
    mScanCallback.OnStoreScanInfoEvent = DealStoreScanInfoEvent;
}

IScanSerivceCallbacks WifiManager::GetScanCallback()
{
    return mScanCallback;
}

void WifiManager::DealScanOpenRes(void)
{
    WifiConfigCenter::GetInstance().SetScanMidState(WifiOprMidState::OPENING, WifiOprMidState::RUNNING);
}

void WifiManager::DealScanCloseRes(void)
{
    WifiManager::GetInstance().PushServiceCloseMsg(WifiCloseServiceCode::SCAN_SERVICE_CLOSE);
}

void WifiManager::DealScanFinished(int state)
{
    WIFI_LOGE("%{public}s, state: %{public}d!", __func__, state);
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_SCAN_STATE_CHANGE;
    cbMsg.msgData = state;
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    WifiCommonEventHelper::PublishScanFinishedEvent(state,"OnScanFinished");
}

void WifiManager::DealScanInfoNotify(std::vector<InterScanInfo> &results)
{
    if (WifiConfigCenter::GetInstance().GetWifiMidState() == WifiOprMidState::RUNNING) {
        IStaService *pService = WifiServiceManager::GetInstance().GetStaServiceInst();
        if (pService != nullptr) {
            pService->ConnectivityManager(results);
        }
    }
}

void WifiManager::DealStoreScanInfoEvent(std::vector<InterScanInfo> &results)
{
}

#ifdef FEATURE_AP_SUPPORT
void WifiManager::InitApCallback(void)
{
    mApCallback.OnApStateChangedEvent = DealApStateChanged;
    mApCallback.OnHotspotStaJoinEvent = DealApGetStaJoin;
    mApCallback.OnHotspotStaLeaveEvent = DealApGetStaLeave;
    return;
}

IApServiceCallbacks WifiManager::GetApCallback()
{
    return mApCallback;
}

void WifiManager::DealApStateChanged(ApState state, int id)
{
    WIFI_LOGE("%{public}s, state: %{public}d!", __func__, state);
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_HOTSPOT_STATE_CHANGE;
    cbMsg.msgData = static_cast<int>(state);
    cbMsg.id = id;
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    if (state == ApState::AP_STATE_IDLE) {
        mCloseApIndex = id;
        WifiManager::GetInstance().PushServiceCloseMsg(WifiCloseServiceCode::AP_SERVICE_CLOSE);
    }
    if (state == ApState::AP_STATE_STARTED) {
        WifiConfigCenter::GetInstance().SetApMidState(WifiOprMidState::OPENING, WifiOprMidState::RUNNING, id);
    }

    std::string msg = std::string("OnHotspotStateChanged") + std::string("id = ") + std::to_string(id);
    WifiCommonEventHelper::PublishHotspotStateChangedEvent((int)state, msg);
    return;
}

void WifiManager::DealApGetStaJoin(const StationInfo &info, int id)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_HOTSPOT_STATE_JOIN;
    cbMsg.staInfo = info;
    cbMsg.id = id;
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    std::string msg = std::string("ApStaJoined") + std::string("id = ") + std::to_string(id);
    WifiCommonEventHelper::PublishApStaJoinEvent(0, msg);
    return;
}

void WifiManager::DealApGetStaLeave(const StationInfo &info, int id)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_HOTSPOT_STATE_LEAVE;
    cbMsg.staInfo = info;
    cbMsg.id = id;
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    std::string msg = std::string("ApStaLeaved") + std::string("id = ") + std::to_string(id);
    WifiCommonEventHelper::PublishApStaLeaveEvent(0, msg);
    return;
}
#endif

#ifdef FEATURE_P2P_SUPPORT
void WifiManager::InitP2pCallback(void)
{
    mP2pCallback.OnP2pStateChangedEvent = DealP2pStateChanged;
    mP2pCallback.OnP2pPeersChangedEvent = DealP2pPeersChanged;
    mP2pCallback.OnP2pServicesChangedEvent = DealP2pServiceChanged;
    mP2pCallback.OnP2pConnectionChangedEvent = DealP2pConnectionChanged;
    mP2pCallback.OnP2pThisDeviceChangedEvent = DealP2pThisDeviceChanged;
    mP2pCallback.OnP2pDiscoveryChangedEvent = DealP2pDiscoveryChanged;
    mP2pCallback.OnP2pGroupsChangedEvent = DealP2pGroupsChanged;
    mP2pCallback.OnP2pActionResultEvent = DealP2pActionResult;
    mP2pCallback.OnConfigChangedEvent = DealConfigChanged;
    return;
}

IP2pServiceCallbacks WifiManager::GetP2pCallback(void)
{
    return mP2pCallback;
}

void WifiManager::DealP2pStateChanged(P2pState state)
{
    WIFI_LOGI("DealP2pStateChanged, state: %{public}d", static_cast<int>(state));
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_P2P_STATE_CHANGE;
    cbMsg.msgData = static_cast<int>(state);
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    if (state == P2pState::P2P_STATE_IDLE) {
        WifiManager::GetInstance().PushServiceCloseMsg(WifiCloseServiceCode::P2P_SERVICE_CLOSE);
    }
    if (state == P2pState::P2P_STATE_STARTED) {
        WifiConfigCenter::GetInstance().SetP2pMidState(WifiOprMidState::OPENING, WifiOprMidState::RUNNING);
    }
    if (state == P2pState::P2P_STATE_CLOSED) {
        bool ret = WifiConfigCenter::GetInstance().SetP2pMidState(WifiOprMidState::OPENING, WifiOprMidState::CLOSED);
        if (ret) {
            WIFI_LOGE("P2p start failed, stop wifi!");
            ForceStopWifi();
            cbMsg.msgCode = WIFI_CBK_MSG_STATE_CHANGE;
            cbMsg.msgData = static_cast<int>(WifiState::DISABLED);
            WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
        }
    }
    WifiCommonEventHelper::PublishP2pStateChangedEvent((int)state, "OnP2pStateChanged");
    return;
}

void WifiManager::DealP2pPeersChanged(const std::vector<WifiP2pDevice> &vPeers)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_PEER_CHANGE;
    cbMsg.device = vPeers;
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    WifiCommonEventHelper::PublishP2pPeersStateChangedEvent(vPeers.size(), "OnP2pPeersChanged");
    return;
}

void WifiManager::DealP2pServiceChanged(const std::vector<WifiP2pServiceInfo> &vServices)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_SERVICE_CHANGE;
    cbMsg.serviceInfo = vServices;
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    return;
}

void WifiManager::DealP2pConnectionChanged(const WifiP2pLinkedInfo &info)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_CONNECT_CHANGE;
    cbMsg.p2pInfo = info;
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    WifiCommonEventHelper::PublishP2pConnStateEvent((int)info.GetConnectState(), "OnP2pConnectStateChanged");
    return;
}

void WifiManager::DealP2pThisDeviceChanged(const WifiP2pDevice &info)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_THIS_DEVICE_CHANGE;
    cbMsg.p2pDevice = info;
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    WifiCommonEventHelper::PublishP2pCurrentDeviceStateChangedEvent(
        (int)info.GetP2pDeviceStatus(), "OnP2pThisDeviceChanged");
    return;
}

void WifiManager::DealP2pDiscoveryChanged(bool bState)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_DISCOVERY_CHANGE;
    cbMsg.msgData = static_cast<int>(bState);
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    return;
}

void WifiManager::DealP2pGroupsChanged()
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_PERSISTENT_GROUPS_CHANGE;
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    WifiCommonEventHelper::PublishP2pGroupStateChangedEvent(0, "OnP2pGroupStateChanged");
    return;
}

void WifiManager::DealP2pActionResult(P2pActionCallback action, ErrCode code)
{
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_P2P_ACTION_RESULT;
    cbMsg.p2pAction = action;
    cbMsg.msgData = static_cast<int>(code);
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    return;
}

void WifiManager::DealConfigChanged(CfgType type, char* data, int dataLen)
{
    if (data == nullptr || dataLen <= 0) {
        return;
    }
    WifiEventCallbackMsg cbMsg;
    cbMsg.msgCode = WIFI_CBK_MSG_CFG_CHANGE;
    CfgInfo* cfgInfoPtr = new (std::nothrow) CfgInfo();
    if (cfgInfoPtr == nullptr) {
        WIFI_LOGE("DealConfigChanged: new CfgInfo failed");
        return;
    }
    cfgInfoPtr->type = type;
    char* cfgData = new (std::nothrow) char[dataLen];
    if (cfgData == nullptr) {
        WIFI_LOGE("DealConfigChanged: new data failed");
        delete cfgInfoPtr;
        return;
    }
    if (memcpy_s(cfgData, dataLen, data, dataLen) != EOK) {
        WIFI_LOGE("DealConfigChanged: memcpy_s failed");
        delete cfgInfoPtr;
        delete[] cfgData;
        return;
    }
    cfgInfoPtr->data = cfgData;
    cfgInfoPtr->dataLen = dataLen;
    cbMsg.cfgInfo = cfgInfoPtr;
    WifiInternalEventDispatcher::GetInstance().AddBroadCastMsg(cbMsg);
    return;
}

void WifiManager::RegisterCfgMonitorCallback(WifiCfgMonitorEventCallback callback)
{
    cfgMonitorCallback = callback;
}
#endif

#ifndef OHOS_ARCH_LITE
void WifiManager::RegisterScreenEvent()
{
    OHOS::EventFwk::MatchingSkills matchingSkills;
    matchingSkills.AddEvent(OHOS::EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_ON);
    matchingSkills.AddEvent(OHOS::EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF);
    EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    screenEventSubscriber_ = std::make_shared<ScreenEventSubscriber>(subscriberInfo);
    if (!EventFwk::CommonEventManager::SubscribeCommonEvent(screenEventSubscriber_)) {
        WIFI_LOGE("ScreenEvent SubscribeCommonEvent() failed");
    } else {
        WIFI_LOGI("ScreenEvent SubscribeCommonEvent() OK");
    }
}

void WifiManager::UnRegisterScreenEvent()
{
    if (!EventFwk::CommonEventManager::UnSubscribeCommonEvent(screenEventSubscriber_)) {
        WIFI_LOGE("ScreenEvent UnSubscribeCommonEvent() failed");
    } else {
        WIFI_LOGI("ScreenEvent UnSubscribeCommonEvent() OK");
    }
    screenEventSubscriber_ = nullptr;
}

void ScreenEventSubscriber::OnReceiveEvent(const OHOS::EventFwk::CommonEventData &data)
{
    std::string action = data.GetWant().GetAction();
    WIFI_LOGI("ScreenEventSubscriber::OnReceiveEvent: %{public}s.", action.c_str());
    IStaService *pService = WifiServiceManager::GetInstance().GetStaServiceInst();
    if (pService == nullptr) {
        WIFI_LOGE("sta service is NOT start!");
        return;
    }

    int screenState = WifiSettings::GetInstance().GetScreenState();
    IScanService *pScanService = WifiServiceManager::GetInstance().GetScanServiceInst();
    if (pScanService == nullptr) {
        WIFI_LOGE("scan service is NOT start!");
        return;
    }
    if (action == OHOS::EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF &&
        screenState == MODE_STATE_OPEN) {
        WifiSettings::GetInstance().SetScreenState(MODE_STATE_CLOSE);
        if (pScanService->OnScreenStateChanged(MODE_STATE_CLOSE) != WIFI_OPT_SUCCESS) {
            WIFI_LOGE("OnScreenStateChanged failed");
        }
        /* Send suspend to wpa */
        if (pService->SetSuspendMode(true) != WIFI_OPT_SUCCESS) {
            WIFI_LOGE("SetSuspendMode failed");
        }
        return;
    }

    if (action == OHOS::EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_ON &&
        screenState == MODE_STATE_CLOSE) {
        WifiSettings::GetInstance().SetScreenState(MODE_STATE_OPEN);
        if (pScanService->OnScreenStateChanged(MODE_STATE_OPEN) != WIFI_OPT_SUCCESS) {
            WIFI_LOGE("OnScreenStateChanged failed");
        }
        /* Send resume to wpa */
        if (pService->SetSuspendMode(false) != WIFI_OPT_SUCCESS) {
            WIFI_LOGE("SetSuspendMode failed");
        }
        return;
    }
    WIFI_LOGW("ScreenEventSubscriber::OnReceiveEvent, screen state: %{public}d.", screenState);
}
#endif
}  // namespace Wifi
}  // namespace OHOS
