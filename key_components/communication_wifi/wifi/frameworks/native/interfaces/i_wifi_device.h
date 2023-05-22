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
#ifndef OHOS_I_WIFI_DEVICE_H
#define OHOS_I_WIFI_DEVICE_H

#include "wifi_errcode.h"
#ifndef OHOS_ARCH_LITE
#include "iremote_broker.h"
#include "message_parcel.h"
#include "message_option.h"
#endif
#include "i_wifi_device_callback.h"
#include "wifi_errcode.h"

namespace OHOS {
namespace Wifi {
#ifdef OHOS_ARCH_LITE
class IWifiDevice {
#else
class IWifiDevice : public IRemoteBroker {
#endif
public:
    virtual ~IWifiDevice()
    {}

    /**
     * @Description Turn on Wi-Fi
     *
     * @return ErrCode - operation result
     */
    virtual ErrCode EnableWifi() = 0;

    /**
     * @Description Turn off Wi-Fi
     *
     * @return ErrCode - operation result
     */
    virtual ErrCode DisableWifi() = 0;
    
    /**
     * @Description create the Wi-Fi protect.
     *
     * @param protectType - WifiProtectMode object
     * @param protectName - the protect name
     * @return ErrCode - operation result
     */
    virtual ErrCode InitWifiProtect(const WifiProtectType &protectType, const std::string &protectName) = 0;

    /**
     * @Description Acquire the Wi-Fi protect mode.
     *
     * @param protectMode - WifiProtectMode object
     * @param protectName - the protect name
     * @return ErrCode - operation result
     */
    virtual ErrCode GetWifiProtectRef(const WifiProtectMode &protectMode, const std::string &protectName) = 0;

    /**
     * @Description Release the Wi-Fi protect mode.
     *
     * @param protectName - the protect name
     * @return ErrCode - operation result
     */
    virtual ErrCode PutWifiProtectRef(const std::string &protectName) = 0;

    /**
     * @Description Remove the wifi candidate device config equals to input network id
     *
     * @param networkId - the candidate device network id
     * @return ErrCode - operation result
     */
    virtual ErrCode RemoveCandidateConfig(int networkId) = 0;

    /**
     * @Description Remove the wifi candidate device config by WifiDeviceConfig
     *
     * @param config - the candidate config to be removed
     * @return ErrCode - operation result
     */
    virtual ErrCode RemoveCandidateConfig(const WifiDeviceConfig &config) = 0;

    /**
     * @Description Add a wifi device configuration.
     *
     * @param config - WifiDeviceConfig object
     * @param result - the device configuration's network id
     * @param isCandidate - Whether is candidate
     * @return ErrCode - operation result
     */
    virtual ErrCode AddDeviceConfig(const WifiDeviceConfig &config, int &result, bool isCandidate) = 0;

    /**
     * @Description Update a wifi device configuration.
     *
     * @param config - WifiDeviceConfig object
     * @param result - the device configuration's network id after updated
     * @return ErrCode - operation result
     */
    virtual ErrCode UpdateDeviceConfig(const WifiDeviceConfig &config, int &result) = 0;

    /**
     * @Description Remove the wifi device config equals to input network id
     *
     * @param networkId - want to remove device config's network id
     * @return ErrCode - operation result
     */
    virtual ErrCode RemoveDevice(int networkId) = 0;

    /**
     * @Description Delete all device configs.
     *
     * @return ErrCode - operation result
     */
    virtual ErrCode RemoveAllDevice() = 0;

    /**
     * @Description Get all the device configs
     *
     * @param result - Get result vector of WifiDeviceConfig
     * @param isCandidate - Whether is candidate
     * @return ErrCode - operation result
     */
    virtual ErrCode GetDeviceConfigs(std::vector<WifiDeviceConfig> &result, bool isCandidate) = 0;

    /**
     * @Description Enable device config, when set attemptEnable, disable other device config
     *
     * @param networkId - need enable device config's network id
     * @param attemptEnable - if set true, disable other device config
     * @return ErrCode - operation result
     */
    virtual ErrCode EnableDeviceConfig(int networkId, bool attemptEnable) = 0;

    /**
     * @Description Disable Wi-Fi device configuration.
     *
     * @param networkId - device config's network id
     * @return ErrCode - operation result
     */
    virtual ErrCode DisableDeviceConfig(int networkId) = 0;

    /**
     * @Description Connecting to a Specified Network
     *
     * @param networkId - network id
     * @param isCandidate - Whether is candidate
     * @return ErrCode - operation result
     */
    virtual ErrCode ConnectToNetwork(int networkId, bool isCandidate) = 0;

    /**
     * @Description Connect To a network base WifiDeviceConfig object
     *
     * @param config - WifiDeviceConfig object
     * @return ErrCode - operation result
     */
    virtual ErrCode ConnectToDevice(const WifiDeviceConfig &config) = 0;

    /**
     * @Description Check whether Wi-Fi is connected.
     *
     * @param isConnected - true / false
     * @return bool - true: connected, false: not connected
     */
    virtual ErrCode IsConnected(bool &isConnected) = 0;

    /**
     * @Description Reconnect to the currently active network
     *
     * @return ErrCode - operation result
     */
    virtual ErrCode ReConnect() = 0;

    /**
     * @Description ReAssociate network
     *
     * @return ErrCode - operation result
     */
    virtual ErrCode ReAssociate(void) = 0;

    /**
     * @Description Disconnect
     *
     * @return ErrCode - operation result
     */
    virtual ErrCode Disconnect(void) = 0;

    /**
     * @Description Enable WPS connection
     *
     * @param config - WpsConfig object
     * @return ErrCode - operation result
     */
    virtual ErrCode StartWps(const WpsConfig &config) = 0;

    /**
     * @Description Close the WPS connection
     *
     * @return ErrCode - operation result
     */
    virtual ErrCode CancelWps(void) = 0;

    /**
     * @Description Check whether Wi-Fi is active
     *
     * @param bActive - active / inactive
     * @return ErrCode - operation result
     */
    virtual ErrCode IsWifiActive(bool &bActive) = 0;

    /**
     * @Description Get the Wifi State
     *
     * @param state - return current wifi state
     * @return ErrCode - operation result
     */
    virtual ErrCode GetWifiState(int &state) = 0;

    /**
     * @Description Obtains the current Wi-Fi connection information
     *
     * @param info - WifiLinkedInfo object
     * @return ErrCode - operation result
     */
    virtual ErrCode GetLinkedInfo(WifiLinkedInfo &info) = 0;

    /**
     * @Description Obtaining DHCP Request Information
     *
     * @param info - IpInfo object
     * @return ErrCode - operation result
     */
    virtual ErrCode GetIpInfo(IpInfo &info) = 0;

    /**
     * @Description Set the Country Code
     *
     * @param countryCode - country code
     * @return ErrCode - operation result
     */
    virtual ErrCode SetCountryCode(const std::string &countryCode) = 0;

    /**
     * @Description Obtains the country code
     *
     * @param countryCode - output the country code
     * @return ErrCode - operation result
     */
    virtual ErrCode GetCountryCode(std::string &countryCode) = 0;

    /**
     * @Description Register callback function.
     *
     * @param callback - IWifiDeviceCallBack object
     * @return ErrCode - operation result
     */
#ifdef OHOS_ARCH_LITE
    virtual ErrCode RegisterCallBack(const std::shared_ptr<IWifiDeviceCallBack> &callback) = 0;
#else
    virtual ErrCode RegisterCallBack(const sptr<IWifiDeviceCallBack> &callback) = 0;
#endif

    /**
     * @Description Get the Signal Level object
     *
     * @param rssi - rssi
     * @param band - band
     * @param level - return the level
     * @return ErrCode - operation result
     */
    virtual ErrCode GetSignalLevel(const int &rssi, const int &band, int &level) = 0;

    /**
     * @Description Get supported features
     *
     * @param features - return supported features
     * @return ErrCode - operation result
     */
    virtual ErrCode GetSupportedFeatures(long &features) = 0;

    /**
     * @Description  Get the device MAC address.
     *
     * @param result - Get device mac String
     * @return ErrCode - operation result
     */
    virtual ErrCode GetDeviceMacAddress(std::string &result) = 0;

    /**
     * @Description set low latency mode
     *
     * @param enabled - true: enable low latency, false: disable low latency
     * @return bool - operation result
     */
    virtual bool SetLowLatencyMode(bool enabled) = 0;

    /**
     * @Description Check whether service is died.
     *
     * @return bool - true: service is died, false: service is not died.
     */
    virtual bool IsRemoteDied(void) = 0;

    /**
     * @Description check wifi-band type is supported
     *
     * @param bandType - wifi band type
     * @param supported - supported / unsupported
     * @return ErrCode - operation result
     */
    virtual ErrCode IsBandTypeSupported(int bandType, bool &supported) = 0;

    /**
     * @Description get all 5g channellist
     *
     * @param result - get result vector of int
     * @return ErrCode - operation result
     */
    virtual ErrCode Get5GHzChannelList(std::vector<int> &result) = 0;

#ifndef OHOS_ARCH_LITE
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.wifi.IWifiDeviceService");
#endif
};
}  // namespace Wifi
}  // namespace OHOS
#endif