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

#include "networkshare_configuration.h"

#include <regex>

#include "netmgr_ext_log_wrapper.h"
#include "net_manager_constants.h"
#include "networkshare_constants.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr const char *CONFIG_KEY_SHARE_SUPPORT = "share_support";
constexpr const char *CONFIG_KEY_USB_REGEXS = "usb_regexs";
constexpr const char *CONFIG_KEY_WIFI_REGEXS = "wifi_regexs";
constexpr const char *CONFIG_KEY_BLUETOOTH_REGEXS = "bluetooth_regexs";
constexpr const char *CONFIG_KEY_BT_PAN_ADDR = "bt_pan_ipv4_addr";
constexpr const char *CONFIG_KEY_WIFI_HOTSPOT_ADDR = "wifi_hotspot_ipv4_addr";
constexpr const char *CONFIG_KEY_USB_RNDIS_ADDR = "usb_rndis_ipv4_addr";
constexpr const char *CONFIG_KEY_BT_PAN_DHCP_NAME = "bt_pan_dhcp_server_name";
constexpr const char *CONFIG_KEY_WIFI_DHCP_NAME = "wifi_hotspot_dhcp_server_name";
constexpr const char *CONFIG_KEY_USB_DHCP_NAME = "usb_rndis_dhcp_server_name";
constexpr const char *CONFIG_KEY_USB_IFACE_NAME = "usb_iface_name";
constexpr const char *CONFIG_KEY_ROUTE_SUFFIX = "route_suffix";
constexpr const char *CONFIG_KEY_DHCP_ENDIP = "dhcp_endip";
constexpr const char *CONFIG_KEY_DEFAULT_MASK = "default_mask";
constexpr const char *CONFIG_KEY_WIFI_SET_DHCP = "wifi_hotspot_set_dhcp";
constexpr const char *SPLIT_SYMBOL_1 = ":";
constexpr const char *SPLIT_SYMBOL_2 = ",";
constexpr const char *VALUE_SUPPORT_TRUE = "true";
} // namespace

NetworkShareConfiguration::NetworkShareConfiguration()
{
    configMap_[CONFIG_KEY_SHARE_SUPPORT] = Config_Value::CONFIG_VALUE_SHARE_SUPPORT;
    configMap_[CONFIG_KEY_USB_REGEXS] = Config_Value::CONFIG_VALUE_USB_REGEXS;
    configMap_[CONFIG_KEY_WIFI_REGEXS] = Config_Value::CONFIG_VALUE_WIFI_REGEXS;
    configMap_[CONFIG_KEY_BLUETOOTH_REGEXS] = Config_Value::CONFIG_VALUE_BLUETOOTH_REGEXS;
    configMap_[CONFIG_KEY_BT_PAN_ADDR] = Config_Value::CONFIG_VALUE_BT_PAN_ADDR;
    configMap_[CONFIG_KEY_WIFI_HOTSPOT_ADDR] = Config_Value::CONFIG_VALUE_WIFI_HOTSPOT_ADDR;
    configMap_[CONFIG_KEY_USB_RNDIS_ADDR] = Config_Value::CONFIG_VALUE_USB_RNDIS_ADDR;
    configMap_[CONFIG_KEY_BT_PAN_DHCP_NAME] = Config_Value::CONFIG_VALUE_BT_PAN_DHCP_NAME;
    configMap_[CONFIG_KEY_WIFI_DHCP_NAME] = Config_Value::CONFIG_VALUE_WIFI_DHCP_NAME;
    configMap_[CONFIG_KEY_USB_DHCP_NAME] = Config_Value::CONFIG_VALUE_USB_DHCP_NAME;
    configMap_[CONFIG_KEY_USB_IFACE_NAME] = Config_Value::CONFIG_VALUE_USB_IFACE_NAME;
    configMap_[CONFIG_KEY_ROUTE_SUFFIX] = Config_Value::CONFIG_VALUE_ROUTE_SUFFIX;
    configMap_[CONFIG_KEY_DHCP_ENDIP] = Config_Value::CONFIG_VALUE_DHCP_ENDIP;
    configMap_[CONFIG_KEY_DEFAULT_MASK] = Config_Value::CONFIG_VALUE_DEFAULT_MASK;
    configMap_[CONFIG_KEY_WIFI_SET_DHCP] = Config_Value::CONFIG_VALUE_WIFI_SET_DHCP;
    LoadConfigData();
}

bool NetworkShareConfiguration::IsNetworkSharingSupported() const
{
    return supported_;
}

bool NetworkShareConfiguration::IsUsbIface(const std::string &iface)
{
    return MatchesDownstreamRegexs(iface, usbRegexs_);
}

bool NetworkShareConfiguration::IsWifiIface(const std::string &iface)
{
    return MatchesDownstreamRegexs(iface, wifiRegexs_);
}

bool NetworkShareConfiguration::IsBluetoothIface(const std::string &iface)
{
    return MatchesDownstreamRegexs(iface, blueToothRegexs_);
}

const std::vector<std::string> &NetworkShareConfiguration::GetUsbIfaceRegexs()
{
    return usbRegexs_;
}

const std::vector<std::string> &NetworkShareConfiguration::GetWifiIfaceRegexs()
{
    return wifiRegexs_;
}

const std::vector<std::string> &NetworkShareConfiguration::GetBluetoothIfaceRegexs()
{
    return blueToothRegexs_;
}

std::string &NetworkShareConfiguration::GetBtpanIpv4Addr()
{
    return btPanIpv4Str_;
}

std::string &NetworkShareConfiguration::GetWifiHotspotIpv4Addr()
{
    return wifiIpv4Str_;
}

std::string &NetworkShareConfiguration::GetUsbRndisIpv4Addr()
{
    return usbIpv4Str_;
}

std::string &NetworkShareConfiguration::GetRouteSuffix()
{
    return routeSuffix_;
}

std::string &NetworkShareConfiguration::GetBtpanDhcpServerName()
{
    return btPanDhcpServerName_;
}

std::string &NetworkShareConfiguration::GetWifiHotspotDhcpServerName()
{
    return wifiDhcpServerName_;
}

std::string &NetworkShareConfiguration::GetUsbRndisDhcpServerName()
{
    return usbDhcpServerName_;
}

std::string &NetworkShareConfiguration::GetUsbRndisIfaceName()
{
    return usbIfaceName_;
}

std::string &NetworkShareConfiguration::GetDefaultMask()
{
    return defaultMask_;
}

std::string &NetworkShareConfiguration::GetDhcpEndIP()
{
    return dhcpEndIP_;
}

bool NetworkShareConfiguration::GetWifiHotspotSetDhcpFlag() const
{
    return isWifiHotspotSetDhcp_;
}

bool NetworkShareConfiguration::MatchesDownstreamRegexs(const std::string &iface, std::vector<std::string> &regexs)
{
    for (std::vector<std::string>::iterator iter = regexs.begin(); iter != regexs.end(); iter++) {
        std::regex str_regex(*iter);
        if (std::regex_match(iface.c_str(), str_regex)) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> NetworkShareConfiguration::ReadConfigFile()
{
    NETMGR_EXT_LOG_I("filePath[%{private}s]", NETWORK_SHARING_CONFIG_PATH);
    std::vector<std::string> strAll;
    std::ifstream infile;
    infile.open(NETWORK_SHARING_CONFIG_PATH);
    if (!infile.is_open()) {
        NETMGR_EXT_LOG_E("filePath[%{private}s] open failed.", NETWORK_SHARING_CONFIG_PATH);
        return strAll;
    }
    std::string strLine;
    while (getline(infile, strLine)) {
        strAll.push_back(strLine);
    }
    infile.close();
    return strAll;
}

void NetworkShareConfiguration::ParseRegexsData(std::vector<std::string> &regexs, std::string &strVal)
{
    std::size_t pos = strVal.find(SPLIT_SYMBOL_2);
    while (std::string::npos != pos) {
        std::string single = strVal.substr(0, pos);
        strVal = strVal.substr(pos + 1, strVal.size());
        pos = strVal.find(SPLIT_SYMBOL_2);
        if (!single.empty()) {
            regexs.push_back(single);
        }
    }
    if (!strVal.empty()) {
        regexs.push_back(strVal);
    }
}

void NetworkShareConfiguration::ParseLineData(std::string &strKey, std::string &strVal)
{
    switch (configMap_[strKey]) {
        case Config_Value::CONFIG_VALUE_SHARE_SUPPORT:
            if (strVal == VALUE_SUPPORT_TRUE) {
                supported_ = true;
            }
            break;
        case Config_Value::CONFIG_VALUE_USB_REGEXS:
            ParseRegexsData(usbRegexs_, strVal);
            break;
        case Config_Value::CONFIG_VALUE_WIFI_REGEXS:
            ParseRegexsData(wifiRegexs_, strVal);
            break;
        case Config_Value::CONFIG_VALUE_BLUETOOTH_REGEXS:
            ParseRegexsData(blueToothRegexs_, strVal);
            break;
        case Config_Value::CONFIG_VALUE_BT_PAN_ADDR:
            btPanIpv4Str_ = strVal;
            break;
        case Config_Value::CONFIG_VALUE_WIFI_HOTSPOT_ADDR:
            wifiIpv4Str_ = strVal;
            break;
        case Config_Value::CONFIG_VALUE_USB_RNDIS_ADDR:
            usbIpv4Str_ = strVal;
            break;
        case Config_Value::CONFIG_VALUE_BT_PAN_DHCP_NAME:
            btPanDhcpServerName_ = strVal;
            break;
        case Config_Value::CONFIG_VALUE_WIFI_DHCP_NAME:
            wifiDhcpServerName_ = strVal;
            break;
        case Config_Value::CONFIG_VALUE_USB_DHCP_NAME:
            usbDhcpServerName_ = strVal;
            break;
        case Config_Value::CONFIG_VALUE_USB_IFACE_NAME:
            usbIfaceName_ = strVal;
            break;
        case Config_Value::CONFIG_VALUE_ROUTE_SUFFIX:
            routeSuffix_ = strVal;
            break;
        case Config_Value::CONFIG_VALUE_DHCP_ENDIP:
            dhcpEndIP_ = strVal;
            break;
        case Config_Value::CONFIG_VALUE_DEFAULT_MASK:
            defaultMask_ = strVal;
            break;
        case Config_Value::CONFIG_VALUE_WIFI_SET_DHCP:
            if (strVal == VALUE_SUPPORT_TRUE) {
                isWifiHotspotSetDhcp_ = true;
            }
            break;
        default:
            NETMGR_EXT_LOG_E("strKey:%{public}s is unknown data.", strKey.c_str());
            break;
    }
}

int32_t NetworkShareConfiguration::LoadConfigData()
{
    isWifiHotspotSetDhcp_ = false;
    supported_ = false;
    usbRegexs_.clear();
    wifiRegexs_.clear();
    blueToothRegexs_.clear();

    std::vector<std::string> strVec = ReadConfigFile();
    if (strVec.size() == 0) {
        return NETWORKSHARE_ERROR_IFACE_CFG_ERROR;
    }

    for_each(strVec.begin(), strVec.end(), [this](std::string &line) {
        std::size_t pos = line.find(SPLIT_SYMBOL_1);
        if (std::string::npos != pos) {
            std::string strKey = line.substr(0, pos);
            std::string strValue = line.substr(pos + 1, line.size());
            NETMGR_EXT_LOG_I("strKey:%{public}s, strValue:%{private}s.", strKey.c_str(), strValue.c_str());
            this->ParseLineData(strKey, strValue);
        }
    });
    return NETMANAGER_EXT_SUCCESS;
}
} // namespace NetManagerStandard
} // namespace OHOS
