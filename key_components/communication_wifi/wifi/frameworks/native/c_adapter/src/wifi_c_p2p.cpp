/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "../../../interfaces/kits/c/wifi_p2p.h"
#include "../../../interfaces/kits/c/wifi_hid2d.h"
#include "wifi_logger.h"
#include "../../../frameworks/native/include/wifi_p2p.h"
#include "wifi_c_utils.h"
#include "wifi_common_util.h"

constexpr int INVALID_VALUE = -1;

DEFINE_WIFILOG_LABEL("WifiCP2P");
std::unique_ptr<OHOS::Wifi::WifiP2p> wifiP2pPtr = OHOS::Wifi::WifiP2p::GetInstance(WIFI_P2P_ABILITY_ID);

NO_SANITIZE("cfi") WifiErrorCode EnableP2p()
{
    CHECK_PTR_RETURN(wifiP2pPtr, ERROR_WIFI_NOT_AVAILABLE);
    return GetCErrorCode(wifiP2pPtr->EnableP2p());
}

NO_SANITIZE("cfi") WifiErrorCode DisableP2p()
{
    CHECK_PTR_RETURN(wifiP2pPtr, ERROR_WIFI_NOT_AVAILABLE);
    return GetCErrorCode(wifiP2pPtr->DisableP2p());
}

NO_SANITIZE("cfi") WifiErrorCode GetP2pEnableStatus(P2pState* state)
{
    CHECK_PTR_RETURN(state, ERROR_WIFI_INVALID_ARGS);
    CHECK_PTR_RETURN(wifiP2pPtr, ERROR_WIFI_NOT_AVAILABLE);

    int p2pEnableStatus = INVALID_VALUE;
    OHOS::Wifi::ErrCode ret = wifiP2pPtr->GetP2pEnableStatus(p2pEnableStatus);
    *state = P2pState(p2pEnableStatus);
    return GetCErrorCode(ret);
}

NO_SANITIZE("cfi") WifiErrorCode DiscoverDevices()
{
    CHECK_PTR_RETURN(wifiP2pPtr, ERROR_WIFI_NOT_AVAILABLE);
    return GetCErrorCode(wifiP2pPtr->DiscoverDevices());
}

NO_SANITIZE("cfi") WifiErrorCode StopDiscoverDevices()
{
    CHECK_PTR_RETURN(wifiP2pPtr, ERROR_WIFI_NOT_AVAILABLE);
    return GetCErrorCode(wifiP2pPtr->StopDiscoverDevices());
}

NO_SANITIZE("cfi") WifiErrorCode DiscoverServices()
{
    CHECK_PTR_RETURN(wifiP2pPtr, ERROR_WIFI_NOT_AVAILABLE);
    return GetCErrorCode(wifiP2pPtr->DiscoverServices());
}

NO_SANITIZE("cfi") WifiErrorCode StopDiscoverServices()
{
    CHECK_PTR_RETURN(wifiP2pPtr, ERROR_WIFI_NOT_AVAILABLE);
    return GetCErrorCode(wifiP2pPtr->StopDiscoverServices());
}

NO_SANITIZE("cfi") WifiErrorCode StartP2pListen(int period, int interval)
{
    CHECK_PTR_RETURN(wifiP2pPtr, ERROR_WIFI_NOT_AVAILABLE);
    return GetCErrorCode(wifiP2pPtr->StartP2pListen(period, interval));
}

NO_SANITIZE("cfi") WifiErrorCode StopP2pListen()
{
    CHECK_PTR_RETURN(wifiP2pPtr, ERROR_WIFI_NOT_AVAILABLE);
    return GetCErrorCode(wifiP2pPtr->StopP2pListen());
}

static void ConvertConfigCToCpp(const WifiP2pConfig* config, OHOS::Wifi::WifiP2pConfig& cppConfig)
{
    CHECK_PTR_RETURN_VOID(config);
    cppConfig.SetDeviceAddress(OHOS::Wifi::MacArrayToStr(config->devAddr));
    cppConfig.SetGoBand(OHOS::Wifi::GroupOwnerBand(static_cast<int>(config->goBand)));
    cppConfig.SetNetId(config->netId);
    cppConfig.SetPassphrase(config->passphrase);
    cppConfig.SetGroupOwnerIntent(config->groupOwnerIntent);
    cppConfig.SetGroupName(config->groupName);
}

NO_SANITIZE("cfi") WifiErrorCode CreateGroup(const WifiP2pConfig* config)
{
    CHECK_PTR_RETURN(wifiP2pPtr, ERROR_WIFI_NOT_AVAILABLE);
    CHECK_PTR_RETURN(config, ERROR_WIFI_INVALID_ARGS);
    OHOS::Wifi::WifiP2pConfig cppConfig;
    ConvertConfigCToCpp(config, cppConfig);
    return GetCErrorCode(wifiP2pPtr->CreateGroup(cppConfig));
}

NO_SANITIZE("cfi") WifiErrorCode RemoveGroup()
{
    CHECK_PTR_RETURN(wifiP2pPtr, ERROR_WIFI_NOT_AVAILABLE);
    return GetCErrorCode(wifiP2pPtr->RemoveGroup());
}

static void ConvertP2PDeviceCToCpp(const WifiP2pDevice& p2pDevice, OHOS::Wifi::WifiP2pDevice& cppDevice)
{
    cppDevice.SetDeviceName(p2pDevice.deviceName);
    cppDevice.SetDeviceAddress(OHOS::Wifi::MacArrayToStr(p2pDevice.devAddr));
    cppDevice.SetPrimaryDeviceType(p2pDevice.primaryDeviceType);
    cppDevice.SetSecondaryDeviceType(p2pDevice.secondaryDeviceType);
    cppDevice.SetP2pDeviceStatus(OHOS::Wifi::P2pDeviceStatus(static_cast<int>(p2pDevice.status)));

    OHOS::Wifi::WifiP2pWfdInfo wfdInfo;
    wfdInfo.SetWfdEnabled((bool)p2pDevice.wfdInfo.wfdEnabled);
    wfdInfo.SetDeviceInfo(p2pDevice.wfdInfo.deviceInfo);
    wfdInfo.SetCtrlPort(p2pDevice.wfdInfo.ctrlPort);
    wfdInfo.SetMaxThroughput(p2pDevice.wfdInfo.maxThroughput);
    cppDevice.SetWfdInfo(wfdInfo);

    cppDevice.SetWpsConfigMethod(p2pDevice.supportWpsConfigMethods);
    cppDevice.SetDeviceCapabilitys(p2pDevice.deviceCapabilitys);
    cppDevice.SetGroupCapabilitys(p2pDevice.groupCapabilitys);
}

static void ConvertGroupInfoCToCpp(const WifiP2pGroupInfo* group, OHOS::Wifi::WifiP2pGroupInfo& cppGroup)
{
    CHECK_PTR_RETURN_VOID(group);
    OHOS::Wifi::WifiP2pDevice owner;
    ConvertP2PDeviceCToCpp(group->owner, owner);
    cppGroup.SetOwner(owner);
    cppGroup.SetIsGroupOwner((bool)group->isP2pGroupOwner);
    cppGroup.SetPassphrase(group->passphrase);
    cppGroup.SetInterface(group->interface);
    cppGroup.SetGroupName(group->groupName);
    cppGroup.SetNetworkId(group->networkId);
    cppGroup.SetFrequency(group->frequency);
    cppGroup.SetIsPersistent((bool)group->isP2pPersistent);
    cppGroup.SetP2pGroupStatus(OHOS::Wifi::P2pGroupStatus(static_cast<int>(group->groupStatus)));
    std::vector<OHOS::Wifi::WifiP2pDevice> clientDevices;
    for (int i = 0; i != group->clientDevicesSize && i < MAX_DEVICES_NUM; ++i) {
        OHOS::Wifi::WifiP2pDevice p2pDevice;
        ConvertP2PDeviceCToCpp(group->clientDevices[i], p2pDevice);
        clientDevices.emplace_back(p2pDevice);
    }
    cppGroup.SetClientDevices(clientDevices);
    cppGroup.SetGoIpAddress(group->goIpAddress);
}

NO_SANITIZE("cfi") WifiErrorCode DeleteGroup(const WifiP2pGroupInfo* group)
{
    CHECK_PTR_RETURN(wifiP2pPtr, ERROR_WIFI_NOT_AVAILABLE);
    CHECK_PTR_RETURN(group, ERROR_WIFI_INVALID_ARGS);
    OHOS::Wifi::WifiP2pGroupInfo groupInfo;
    ConvertGroupInfoCToCpp(group, groupInfo);
    return GetCErrorCode(wifiP2pPtr->DeleteGroup(groupInfo));
}

NO_SANITIZE("cfi") WifiErrorCode P2pConnect(const WifiP2pConfig* config)
{
    CHECK_PTR_RETURN(wifiP2pPtr, ERROR_WIFI_NOT_AVAILABLE);
    CHECK_PTR_RETURN(config, ERROR_WIFI_INVALID_ARGS);
    OHOS::Wifi::WifiP2pConfig deviceConfig;
    ConvertConfigCToCpp(config, deviceConfig);
    return GetCErrorCode(wifiP2pPtr->P2pConnect(deviceConfig));
}

NO_SANITIZE("cfi") WifiErrorCode P2pCancelConnect()
{
    CHECK_PTR_RETURN(wifiP2pPtr, ERROR_WIFI_NOT_AVAILABLE);
    return GetCErrorCode(wifiP2pPtr->P2pCancelConnect());
}

static OHOS::Wifi::ErrCode ConvertP2PDeviceCppToC(const OHOS::Wifi::WifiP2pDevice& cppDevice, WifiP2pDevice* p2pDevice)
{
    CHECK_PTR_RETURN(p2pDevice, OHOS::Wifi::WIFI_OPT_INVALID_PARAM);
    if (memcpy_s(p2pDevice->deviceName, P2P_NAME_LENGTH,
        cppDevice.GetDeviceName().c_str(), cppDevice.GetDeviceName().size() + 1) != EOK) {
        WIFI_LOGE("memcpy_s device name failed!");
        return OHOS::Wifi::WIFI_OPT_FAILED;
    }
    if (OHOS::Wifi::MacStrToArray(cppDevice.GetDeviceAddress(), p2pDevice->devAddr) != EOK) {
        WIFI_LOGE("Mac str to array failed!");
        return OHOS::Wifi::WIFI_OPT_FAILED;
    }
    if (memcpy_s(p2pDevice->primaryDeviceType, DEVICE_TYPE_LENGTH,
        cppDevice.GetPrimaryDeviceType().c_str(), cppDevice.GetPrimaryDeviceType().size() + 1) != EOK) {
        WIFI_LOGE("memcpy_s primary device type failed!");
        return OHOS::Wifi::WIFI_OPT_FAILED;
    }
    if (memcpy_s(p2pDevice->secondaryDeviceType, DEVICE_TYPE_LENGTH,
        cppDevice.GetSecondaryDeviceType().c_str(), cppDevice.GetSecondaryDeviceType().size() + 1) != EOK) {
        WIFI_LOGE("memcpy_s secondary device type failed!");
        return OHOS::Wifi::WIFI_OPT_FAILED;
    }

    p2pDevice->status = P2pDeviceStatus(static_cast<int>(cppDevice.GetP2pDeviceStatus()));
    p2pDevice->wfdInfo.wfdEnabled = cppDevice.GetWfdInfo().GetWfdEnabled();
    p2pDevice->wfdInfo.deviceInfo = cppDevice.GetWfdInfo().GetDeviceInfo();
    p2pDevice->wfdInfo.ctrlPort = cppDevice.GetWfdInfo().GetCtrlPort();
    p2pDevice->wfdInfo.maxThroughput = cppDevice.GetWfdInfo().GetMaxThroughput();
    p2pDevice->supportWpsConfigMethods = cppDevice.GetWpsConfigMethod();
    p2pDevice->deviceCapabilitys = cppDevice.GetDeviceCapabilitys();
    p2pDevice->groupCapabilitys = cppDevice.GetGroupCapabilitys();
    return OHOS::Wifi::WIFI_OPT_SUCCESS;
}

static OHOS::Wifi::ErrCode ConvertGroupInfoCppToC(const OHOS::Wifi::WifiP2pGroupInfo& cppGroup, WifiP2pGroupInfo* group)
{
    CHECK_PTR_RETURN(group, OHOS::Wifi::WIFI_OPT_INVALID_PARAM);
    if (ConvertP2PDeviceCppToC(cppGroup.GetOwner(), &group->owner) != OHOS::Wifi::WIFI_OPT_SUCCESS) {
        return OHOS::Wifi::WIFI_OPT_FAILED;
    }
    group->isP2pGroupOwner = cppGroup.IsGroupOwner();
    if (memcpy_s(group->passphrase, PASSPHRASE_LENGTH,
        cppGroup.GetPassphrase().c_str(), cppGroup.GetPassphrase().size() + 1) != EOK) {
        WIFI_LOGE("memcpy_s passphrase failed!");
        return OHOS::Wifi::WIFI_OPT_FAILED;
    }
    if (memcpy_s(group->interface, INTERFACE_LENGTH,
        cppGroup.GetInterface().c_str(), cppGroup.GetInterface().size() + 1) != EOK) {
        WIFI_LOGE("memcpy_s interface failed!");
        return OHOS::Wifi::WIFI_OPT_FAILED;
    }
    if (memcpy_s(group->groupName, P2P_NAME_LENGTH,
        cppGroup.GetGroupName().c_str(), cppGroup.GetGroupName().size() + 1) != EOK) {
        WIFI_LOGE("memcpy_s group name failed!");
        return OHOS::Wifi::WIFI_OPT_FAILED;
    }
    group->networkId = cppGroup.GetNetworkId();
    group->frequency = cppGroup.GetFrequency();
    group->isP2pPersistent = cppGroup.IsPersistent();
    group->groupStatus = P2pGroupStatus(static_cast<int>(cppGroup.GetP2pGroupStatus()));
    const std::vector<OHOS::Wifi::WifiP2pDevice>& vecDevices = cppGroup.GetClientDevices();
    for (size_t i = 0; i != vecDevices.size() && i < MAX_DEVICES_NUM; ++i) {
        if (ConvertP2PDeviceCppToC(vecDevices[i], &group->clientDevices[i]) != OHOS::Wifi::WIFI_OPT_SUCCESS) {
            WIFI_LOGE("convert p2p device failed!");
            return OHOS::Wifi::WIFI_OPT_FAILED;
        }
    }
    group->clientDevicesSize = (int)vecDevices.size();
    if (memcpy_s(group->goIpAddress, IP_ADDR_STR_LEN,
        cppGroup.GetGoIpAddress().c_str(), cppGroup.GetGoIpAddress().size() + 1) != EOK) {
        WIFI_LOGE("memcpy_s interface failed!");
        return OHOS::Wifi::WIFI_OPT_FAILED;
    }
    return OHOS::Wifi::WIFI_OPT_SUCCESS;
}

NO_SANITIZE("cfi") WifiErrorCode GetCurrentGroup(WifiP2pGroupInfo* groupInfo)
{
    CHECK_PTR_RETURN(wifiP2pPtr, ERROR_WIFI_NOT_AVAILABLE);
    CHECK_PTR_RETURN(groupInfo, ERROR_WIFI_INVALID_ARGS);
    OHOS::Wifi::WifiP2pGroupInfo cppGroupInfo;
    OHOS::Wifi::ErrCode ret = wifiP2pPtr->GetCurrentGroup(cppGroupInfo);
    if (ret != OHOS::Wifi::WIFI_OPT_SUCCESS) {
        WIFI_LOGE("get current group info failed!");
        return ERROR_WIFI_NOT_AVAILABLE;
    }
    return GetCErrorCode(ConvertGroupInfoCppToC(cppGroupInfo, groupInfo));
}

NO_SANITIZE("cfi") WifiErrorCode GetP2pConnectedStatus(int* status)
{
    CHECK_PTR_RETURN(wifiP2pPtr, ERROR_WIFI_NOT_AVAILABLE);
    CHECK_PTR_RETURN(status, ERROR_WIFI_INVALID_ARGS);
    int p2pStatus = -1;
    OHOS::Wifi::ErrCode ret = wifiP2pPtr->GetP2pConnectedStatus(p2pStatus);
    if (ret != OHOS::Wifi::WIFI_OPT_SUCCESS) {
        WIFI_LOGE("get p2p status failed!");
    }
    *status = p2pStatus;
    return GetCErrorCode(ret);
}

NO_SANITIZE("cfi") WifiErrorCode QueryP2pLocalDevice(WifiP2pDevice* deviceInfo)
{
    CHECK_PTR_RETURN(wifiP2pPtr, ERROR_WIFI_NOT_AVAILABLE);
    CHECK_PTR_RETURN(deviceInfo, ERROR_WIFI_INVALID_ARGS);
    OHOS::Wifi::WifiP2pDevice cppDeviceInfo;
    OHOS::Wifi::ErrCode ret = wifiP2pPtr->QueryP2pLocalDevice(cppDeviceInfo);
    if (ret != OHOS::Wifi::WIFI_OPT_SUCCESS) {
        WIFI_LOGE("QueryP2pLocalDevice return failed!");
    }
    return GetCErrorCode(ConvertP2PDeviceCppToC(cppDeviceInfo, deviceInfo));
}

NO_SANITIZE("cfi") WifiErrorCode QueryP2pDevices(WifiP2pDevice* clientDevices, int size, int* retSize)
{
    CHECK_PTR_RETURN(wifiP2pPtr, ERROR_WIFI_NOT_AVAILABLE);
    CHECK_PTR_RETURN(clientDevices, ERROR_WIFI_INVALID_ARGS);
    CHECK_PTR_RETURN(retSize, ERROR_WIFI_INVALID_ARGS);
    std::vector<OHOS::Wifi::WifiP2pDevice> vecDevices;
    OHOS::Wifi::ErrCode ret = wifiP2pPtr->QueryP2pDevices(vecDevices);
    if (ret != OHOS::Wifi::WIFI_OPT_SUCCESS) {
        WIFI_LOGE("query p2p devices failed!");
        return ERROR_WIFI_UNKNOWN;
    }

    for (int i = 0; i != (int)vecDevices.size() && i < size; ++i) {
        if (ConvertP2PDeviceCppToC(vecDevices[i], clientDevices++) != OHOS::Wifi::WIFI_OPT_SUCCESS) {
            WIFI_LOGE("convert p2p device failed!");
            return ERROR_WIFI_UNKNOWN;
        }
    }
    *retSize = std::min(size, (int)vecDevices.size());
    return WIFI_SUCCESS;
}

NO_SANITIZE("cfi") WifiErrorCode QueryP2pGroups(WifiP2pGroupInfo* groupInfo, int size)
{
    CHECK_PTR_RETURN(wifiP2pPtr, ERROR_WIFI_NOT_AVAILABLE);
    CHECK_PTR_RETURN(groupInfo, ERROR_WIFI_INVALID_ARGS);
    std::vector<OHOS::Wifi::WifiP2pGroupInfo> groups;
    OHOS::Wifi::ErrCode ret = wifiP2pPtr->QueryP2pGroups(groups);
    if (ret != OHOS::Wifi::WIFI_OPT_SUCCESS) {
        WIFI_LOGE("query p2p devices failed!");
        return ERROR_WIFI_UNKNOWN;
    }

    for (int i = 0; i != (int)groups.size() && i < size; ++i) {
        ret = ConvertGroupInfoCppToC(groups[i], groupInfo++);
        if (ret != OHOS::Wifi::WIFI_OPT_SUCCESS) {
            WIFI_LOGE("convert group info failed!");
            return ERROR_WIFI_UNKNOWN;
        }
    }
    return WIFI_SUCCESS;
}

class WifiP2pCEventCallback : public OHOS::Wifi::IWifiP2pCallback
{
public:
    void OnP2pStateChanged(int state) override
    {
        WIFI_LOGI("received state changed event: %{public}d", state);
        if (stateChangeCb != nullptr) {
            stateChangeCb(P2pState(state));
        }
    }

    void OnP2pPersistentGroupsChanged(void) override
    {
        WIFI_LOGI("received group changed event");
        if (groupChangeCb != nullptr) {
            groupChangeCb();
        }
    }

    void OnP2pThisDeviceChanged(const OHOS::Wifi::WifiP2pDevice &device) override
    {
        WIFI_LOGI("%{public}s, received this device changed event", __func__);
    }

    void OnP2pPeersChanged(const std::vector<OHOS::Wifi::WifiP2pDevice> &devices) override
    {
        WIFI_LOGI("received peers changed event: %{public}d", (int)devices.size());
        WifiP2pDevice *devicePtr = nullptr;
        if (!devices.empty()) {
            devicePtr = new (std::nothrow) WifiP2pDevice[(int)devices.size()];
            if (devicePtr == nullptr) {
                WIFI_LOGE("new WifiP2pDevice failed!");
                return;
            }
            WifiP2pDevice *p = devicePtr;
            for (auto& each : devices) {
                if (ConvertP2PDeviceCppToC(each, p++) != OHOS::Wifi::WIFI_OPT_SUCCESS) {
                    WIFI_LOGE("peers changed convert p2p device failed!");
                    delete[] devicePtr;
                    return;
                }
            }
        }
        if (peersChangeCb != nullptr) {
            peersChangeCb(devicePtr, (int)devices.size());
        }
        if (devicePtr != nullptr) {
            delete[] devicePtr;
            devicePtr = nullptr;
        }
    }

    void OnP2pServicesChanged(const std::vector<OHOS::Wifi::WifiP2pServiceInfo> &srvInfo) override
    {
        WIFI_LOGI("%{public}s, received p2p services changed event", __func__);
    }

    void OnP2pConnectionChanged(const OHOS::Wifi::WifiP2pLinkedInfo &info) override
    {
        WIFI_LOGI("received connection changed event");
        if (connectionChangeCb != nullptr) {
            connectionChangeCb(ConvertP2pLinkedInfo(info));
        }
    }

    void OnP2pDiscoveryChanged(bool isChange) override
    {
        WIFI_LOGI("%{public}s, received p2p discovery changed event", __func__);
    }

    void OnP2pActionResult(OHOS::Wifi::P2pActionCallback action, OHOS::Wifi::ErrCode code) override
    {
        WIFI_LOGI("%{public}s, received p2p action results event", __func__);
    }

    void OnConfigChanged(OHOS::Wifi::CfgType type, char* data, int dataLen) override
    {
        WIFI_LOGI("received config change event: %{public}d", static_cast<int>(type));
        if (cfgChangeCallback != nullptr) {
            cfgChangeCallback(CfgType(type), data, dataLen);
        }
    }

    OHOS::sptr<OHOS::IRemoteObject> AsObject() override
    {
        return nullptr;
    }

public:
    WifiP2pCEventCallback() {
        stateChangeCb = nullptr;
        groupChangeCb = nullptr;
        connectionChangeCb = nullptr;
        peersChangeCb = nullptr;
        cfgChangeCallback = nullptr;
    }

    virtual ~WifiP2pCEventCallback() {
    }

public:
    P2pStateChangedCallback stateChangeCb;
    P2pPersistentGroupsChangedCallback groupChangeCb;
    P2pConnectionChangedCallback connectionChangeCb;
    P2pPeersChangedCallback peersChangeCb;
    WifiCfgChangCallback cfgChangeCallback;

private:
    WifiP2pLinkedInfo ConvertP2pLinkedInfo(const OHOS::Wifi::WifiP2pLinkedInfo& linkedInfo) {
        WifiP2pLinkedInfo info;
        info.connectState = P2pConnectionState(static_cast<int>(linkedInfo.GetConnectState()));
        info.isP2pGroupOwner = linkedInfo.IsGroupOwner();
        OHOS::Wifi::MacStrToArray(linkedInfo.GetGroupOwnerAddress(), info.groupOwnerAddress);
        return info;
    }
};

OHOS::sptr<WifiP2pCEventCallback> sptrCallback =
    OHOS::sptr<WifiP2pCEventCallback>(new (std::nothrow) WifiP2pCEventCallback());

NO_SANITIZE("cfi") WifiErrorCode RegisterP2pStateChangedCallback(const P2pStateChangedCallback callback)
{
    CHECK_PTR_RETURN(callback, ERROR_WIFI_INVALID_ARGS);
    CHECK_PTR_RETURN(wifiP2pPtr, ERROR_WIFI_NOT_AVAILABLE);
    CHECK_PTR_RETURN(sptrCallback, ERROR_WIFI_NOT_AVAILABLE);
    sptrCallback->stateChangeCb = callback;
    wifiP2pPtr->RegisterCallBack(sptrCallback);
    return WIFI_SUCCESS;
}

NO_SANITIZE("cfi")
WifiErrorCode RegisterP2pPersistentGroupsChangedCallback(const P2pPersistentGroupsChangedCallback callback)
{
    CHECK_PTR_RETURN(callback, ERROR_WIFI_INVALID_ARGS);
    CHECK_PTR_RETURN(wifiP2pPtr, ERROR_WIFI_NOT_AVAILABLE);
    CHECK_PTR_RETURN(sptrCallback, ERROR_WIFI_NOT_AVAILABLE);
    sptrCallback->groupChangeCb = callback;
    wifiP2pPtr->RegisterCallBack(sptrCallback);
    return WIFI_SUCCESS;
}

NO_SANITIZE("cfi") WifiErrorCode RegisterP2pConnectionChangedCallback(const P2pConnectionChangedCallback callback)
{
    CHECK_PTR_RETURN(callback, ERROR_WIFI_INVALID_ARGS);
    CHECK_PTR_RETURN(wifiP2pPtr, ERROR_WIFI_NOT_AVAILABLE);
    CHECK_PTR_RETURN(sptrCallback, ERROR_WIFI_NOT_AVAILABLE);
    sptrCallback->connectionChangeCb = callback;
    wifiP2pPtr->RegisterCallBack(sptrCallback);
    return WIFI_SUCCESS;
}

NO_SANITIZE("cfi") WifiErrorCode RegisterP2pPeersChangedCallback(const P2pPeersChangedCallback callback)
{
    CHECK_PTR_RETURN(callback, ERROR_WIFI_INVALID_ARGS);
    CHECK_PTR_RETURN(wifiP2pPtr, ERROR_WIFI_NOT_AVAILABLE);
    CHECK_PTR_RETURN(sptrCallback, ERROR_WIFI_NOT_AVAILABLE);
    sptrCallback->peersChangeCb = callback;
    wifiP2pPtr->RegisterCallBack(sptrCallback);
    return WIFI_SUCCESS;
}

NO_SANITIZE("cfi") WifiErrorCode RegisterCfgChangCallback(const WifiCfgChangCallback callback)
{
    CHECK_PTR_RETURN(callback, ERROR_WIFI_INVALID_ARGS);
    CHECK_PTR_RETURN(wifiP2pPtr, ERROR_WIFI_NOT_AVAILABLE);
    CHECK_PTR_RETURN(sptrCallback, ERROR_WIFI_NOT_AVAILABLE);
    sptrCallback->cfgChangeCallback = callback;
    wifiP2pPtr->RegisterCallBack(sptrCallback);
    return WIFI_SUCCESS;
}

NO_SANITIZE("cfi") WifiErrorCode UnregisterCfgChangCallback(void)
{
    CHECK_PTR_RETURN(sptrCallback, ERROR_WIFI_NOT_AVAILABLE);
    sptrCallback->cfgChangeCallback = nullptr;
    return WIFI_SUCCESS;
}