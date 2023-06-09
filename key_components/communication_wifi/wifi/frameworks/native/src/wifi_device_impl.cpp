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

#include "wifi_device_impl.h"
#include <new>
#ifndef OHOS_ARCH_LITE
#include "iremote_broker.h"
#include "iremote_object.h"
#include "iservice_registry.h"
#endif
#include "wifi_device_proxy.h"
#include "wifi_logger.h"

DEFINE_WIFILOG_LABEL("WifiDeviceImpl");

namespace OHOS {
namespace Wifi {
#define RETURN_IF_FAIL(cond)                          \
    do {                                              \
        if (!(cond)) {                                \
            WIFI_LOGI("'%{public}s' failed.", #cond); \
            return WIFI_OPT_FAILED;                   \
        }                                             \
    } while (0)

WifiDeviceImpl::WifiDeviceImpl(int systemAbilityId) : systemAbilityId_(systemAbilityId), client_(nullptr)
{}

WifiDeviceImpl::~WifiDeviceImpl()
{
#ifdef OHOS_ARCH_LITE
    WifiDeviceProxy::ReleaseInstance();
#endif
}

bool WifiDeviceImpl::Init()
{
#ifdef OHOS_ARCH_LITE
    WifiDeviceProxy *deviceProxy = WifiDeviceProxy::GetInstance();
    if (deviceProxy == nullptr) {
        WIFI_LOGE("get wifi device proxy failed.");
        return false;
    }
    if (deviceProxy->Init() != WIFI_OPT_SUCCESS) {
        WIFI_LOGE("wifi device proxy init failed.");
        WifiDeviceProxy::ReleaseInstance();
        return false;
    }
    client_ = deviceProxy;
    return true;
#else
    return GetWifiDeviceProxy();
#endif
}

bool WifiDeviceImpl::GetWifiDeviceProxy()
{
#ifdef OHOS_ARCH_LITE
    return (client_ != nullptr);
#else
    if (IsRemoteDied() == false) {
        return true;
    }

    WIFI_LOGI("GetWifiDeviceProxy, get new sa from remote!");
    sptr<ISystemAbilityManager> sa_mgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (sa_mgr == nullptr) {
        WIFI_LOGE("failed to get SystemAbilityManager");
        return false;
    }

    sptr<IRemoteObject> object = sa_mgr->GetSystemAbility(systemAbilityId_);
    if (object == nullptr) {
        WIFI_LOGE("failed to get DEVICE_SERVICE");
        return false;
    }

    client_ = iface_cast<IWifiDevice>(object);
    if (client_ == nullptr) {
        client_ = new (std::nothrow) WifiDeviceProxy(object);
    }

    if (client_ == nullptr) {
        WIFI_LOGE("wifi device init failed. %{public}d", systemAbilityId_);
        return false;
    }
    return true;
#endif
}

ErrCode WifiDeviceImpl::EnableWifi()
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->EnableWifi();
}

ErrCode WifiDeviceImpl::DisableWifi()
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->DisableWifi();
}

ErrCode WifiDeviceImpl::InitWifiProtect(const WifiProtectType &protectType, const std::string &protectName)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->InitWifiProtect(protectType, protectName);
}

ErrCode WifiDeviceImpl::GetWifiProtectRef(const WifiProtectMode &protectMode, const std::string &protectName)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->GetWifiProtectRef(protectMode, protectName);
}

ErrCode WifiDeviceImpl::PutWifiProtectRef(const std::string &protectName)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->PutWifiProtectRef(protectName);
}

ErrCode WifiDeviceImpl::RemoveCandidateConfig(int networkId)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->RemoveCandidateConfig(networkId);
}

ErrCode WifiDeviceImpl::RemoveCandidateConfig(const WifiDeviceConfig &config)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->RemoveCandidateConfig(config);
}

ErrCode WifiDeviceImpl::AddDeviceConfig(const WifiDeviceConfig &config, int &result, bool isCandidate)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->AddDeviceConfig(config, result, isCandidate);
}

ErrCode WifiDeviceImpl::UpdateDeviceConfig(const WifiDeviceConfig &config, int &result)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->UpdateDeviceConfig(config, result);
}

ErrCode WifiDeviceImpl::RemoveDevice(int networkId)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->RemoveDevice(networkId);
}

ErrCode WifiDeviceImpl::RemoveAllDevice()
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->RemoveAllDevice();
}

ErrCode WifiDeviceImpl::GetDeviceConfigs(std::vector<WifiDeviceConfig> &result, bool isCandidate)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->GetDeviceConfigs(result, isCandidate);
}

ErrCode WifiDeviceImpl::EnableDeviceConfig(int networkId, bool attemptEnable)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->EnableDeviceConfig(networkId, attemptEnable);
}

ErrCode WifiDeviceImpl::DisableDeviceConfig(int networkId)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->DisableDeviceConfig(networkId);
}

ErrCode WifiDeviceImpl::ConnectToNetwork(int networkId, bool isCandidate)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->ConnectToNetwork(networkId, isCandidate);
}

ErrCode WifiDeviceImpl::ConnectToDevice(const WifiDeviceConfig &config)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->ConnectToDevice(config);
}

ErrCode WifiDeviceImpl::IsConnected(bool &isConnected)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->IsConnected(isConnected);
}

ErrCode WifiDeviceImpl::ReConnect()
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->ReConnect();
}

ErrCode WifiDeviceImpl::ReAssociate()
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->ReAssociate();
}

ErrCode WifiDeviceImpl::Disconnect()
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->Disconnect();
}

ErrCode WifiDeviceImpl::StartWps(const WpsConfig &config)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->StartWps(config);
}

ErrCode WifiDeviceImpl::CancelWps()
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->CancelWps();
}

ErrCode WifiDeviceImpl::IsWifiActive(bool &bActive)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->IsWifiActive(bActive);
}

ErrCode WifiDeviceImpl::GetWifiState(int &state)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->GetWifiState(state);
}

ErrCode WifiDeviceImpl::GetLinkedInfo(WifiLinkedInfo &info)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->GetLinkedInfo(info);
}

ErrCode WifiDeviceImpl::GetIpInfo(IpInfo &info)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->GetIpInfo(info);
}

ErrCode WifiDeviceImpl::SetCountryCode(const std::string &countryCode)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->SetCountryCode(countryCode);
}

ErrCode WifiDeviceImpl::GetCountryCode(std::string &countryCode)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->GetCountryCode(countryCode);
}

ErrCode WifiDeviceImpl::GetSignalLevel(const int &rssi, const int &band, int &level)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->GetSignalLevel(rssi, band, level);
}

#ifdef OHOS_ARCH_LITE
ErrCode WifiDeviceImpl::RegisterCallBack(const std::shared_ptr<IWifiDeviceCallBack> &callback)
#else
ErrCode WifiDeviceImpl::RegisterCallBack(const sptr<IWifiDeviceCallBack> &callback)
#endif
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->RegisterCallBack(callback);
}

ErrCode WifiDeviceImpl::GetSupportedFeatures(long &features)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->GetSupportedFeatures(features);
}

ErrCode WifiDeviceImpl::IsFeatureSupported(long feature, bool &isSupported)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    long tmpFeatures = 0;
    isSupported = false;
    ErrCode ret = client_->GetSupportedFeatures(tmpFeatures);
    if (ret != WIFI_OPT_SUCCESS) {
        return ret;
    }
    isSupported = ((tmpFeatures & feature) == feature);
    return WIFI_OPT_SUCCESS;
}

ErrCode WifiDeviceImpl::GetDeviceMacAddress(std::string &result)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->GetDeviceMacAddress(result);
}

ErrCode WifiDeviceImpl::IsBandTypeSupported(int bandType, bool &supported)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->IsBandTypeSupported(bandType, supported);
}

ErrCode WifiDeviceImpl::Get5GHzChannelList(std::vector<int> &result)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->Get5GHzChannelList(result);
}

bool WifiDeviceImpl::SetLowLatencyMode(bool enabled)
{
    RETURN_IF_FAIL(GetWifiDeviceProxy());
    return client_->SetLowLatencyMode(enabled);
}

bool WifiDeviceImpl::IsRemoteDied(void)
{
    return (client_ == nullptr) ? true : client_->IsRemoteDied();
}
}  // namespace Wifi
}  // namespace OHOS
