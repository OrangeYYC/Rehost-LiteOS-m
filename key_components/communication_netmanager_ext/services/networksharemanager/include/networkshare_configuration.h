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

#ifndef NETWORKSHARE_CONFIGURATION_H
#define NETWORKSHARE_CONFIGURATION_H

#include <fstream>
#include <map>
#include <vector>

#include "networkshare_configuration.h"

namespace OHOS {
namespace NetManagerStandard {
class NetworkShareConfiguration {
public:
    NetworkShareConfiguration();
    ~NetworkShareConfiguration() = default;

    /**
     * is surpport share network
     */
    bool IsNetworkSharingSupported() const;

    /**
     * is usb iface by regex
     */
    bool IsUsbIface(const std::string &iface);

    /**
     * is wifi iface by regex
     */
    bool IsWifiIface(const std::string &iface);

    /**
     * is bluetooth iface by regex
     */
    bool IsBluetoothIface(const std::string &iface);

    /**
     * get usb iface regex
     */
    const std::vector<std::string> &GetUsbIfaceRegexs();

    /**
     * get wifi iface regex
     */
    const std::vector<std::string> &GetWifiIfaceRegexs();

    /**
     * get bluetooth iface regex
     */
    const std::vector<std::string> &GetBluetoothIfaceRegexs();

    /**
     * get wifi hotspot set dhcp flag
     */
    bool GetWifiHotspotSetDhcpFlag() const;

    /**
     * get btpan default ipv4 address
     */
    std::string &GetBtpanIpv4Addr();

    /**
     * get wifi hotspot default ipv4 address
     */
    std::string &GetWifiHotspotIpv4Addr();

    /**
     * get usb rndis default ipv4 address
     */
    std::string &GetUsbRndisIpv4Addr();

    /**
     * get the default route suffix
     */
    std::string &GetRouteSuffix();

    /**
     * get the btpan dhcp server name
     */
    std::string &GetBtpanDhcpServerName();

    /**
     * get the wifi hotspot dhcp server name
     */
    std::string &GetWifiHotspotDhcpServerName();

    /**
     * get the usb rndis dhcp server name
     */
    std::string &GetUsbRndisDhcpServerName();

    /**
     * get the usb rndis iface name
     */
    std::string &GetUsbRndisIfaceName();

    /**
     * get default net mask
     */
    std::string &GetDefaultMask();

    /**
     * get dhcp endIP
     */
    std::string &GetDhcpEndIP();

private:
    int32_t LoadConfigData();
    bool MatchesDownstreamRegexs(const std::string &, std::vector<std::string> &regexs);
    std::vector<std::string> ReadConfigFile();
    void ParseLineData(std::string &strKey, std::string &strVal);
    void ParseRegexsData(std::vector<std::string> &regexs, std::string &strVal);

private:
    enum class Config_Value {
        CONFIG_VALUE_SHARE_SUPPORT,
        CONFIG_VALUE_USB_REGEXS,
        CONFIG_VALUE_WIFI_REGEXS,
        CONFIG_VALUE_BLUETOOTH_REGEXS,
        CONFIG_VALUE_BT_PAN_ADDR,
        CONFIG_VALUE_WIFI_HOTSPOT_ADDR,
        CONFIG_VALUE_USB_RNDIS_ADDR,
        CONFIG_VALUE_BT_PAN_DHCP_NAME,
        CONFIG_VALUE_WIFI_DHCP_NAME,
        CONFIG_VALUE_USB_DHCP_NAME,
        CONFIG_VALUE_USB_IFACE_NAME,
        CONFIG_VALUE_ROUTE_SUFFIX,
        CONFIG_VALUE_DHCP_ENDIP,
        CONFIG_VALUE_DEFAULT_MASK,
        CONFIG_VALUE_WIFI_SET_DHCP,
    };

    bool isWifiHotspotSetDhcp_ = false;
    bool supported_ = false;
    std::vector<std::string> usbRegexs_;
    std::vector<std::string> wifiRegexs_;
    std::vector<std::string> blueToothRegexs_;
    std::string btPanIpv4Str_;
    std::string wifiIpv4Str_;
    std::string usbIpv4Str_;
    std::string routeSuffix_;
    std::string btPanDhcpServerName_;
    std::string wifiDhcpServerName_;
    std::string usbDhcpServerName_;
    std::string usbIfaceName_;
    std::string defaultMask_;
    std::string dhcpEndIP_;
    std::map<std::string, Config_Value> configMap_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETWORKSHARE_CONFIGURATION_H
