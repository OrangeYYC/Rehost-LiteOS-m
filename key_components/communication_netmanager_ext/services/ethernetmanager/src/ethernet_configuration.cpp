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
#include "ethernet_configuration.h"

#include <arpa/inet.h>
#include <cerrno>
#include <cstdlib>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <limits>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <regex>

#include "net_manager_constants.h"
#include "netmanager_base_common_utils.h"
#include "netmgr_ext_log_wrapper.h"
#include "route.h"
#include "securec.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
const std::string IFACE_MATCH = "eth\\d";
const std::string CONFIG_KEY_ETH_COMPONENT_FLAG = "config_ethernet_interfaces";
const std::string CONFIG_KEY_ETH_IFACE = "iface";
const std::string CONFIG_KEY_ETH_CAPS = "caps";
const std::string CONFIG_KEY_ETH_IP = "ip";
const std::string CONFIG_KEY_ETH_GATEWAY = "gateway";
const std::string CONFIG_KEY_ETH_DNS = "dns";
const std::string CONFIG_KEY_ETH_NETMASK = "netmask";
const std::string CONFIG_KEY_ETH_ROUTE = "route";
const std::string CONFIG_KEY_ETH_ROUTE_MASK = "routemask";
constexpr int32_t MKDIR_ERR = -1;
constexpr int32_t USER_PATH_LEN = 25;
constexpr const char *FILE_OBLIQUE_LINE = "/";
constexpr const char *KEY_DEVICE = "DEVICE=";
constexpr const char *KEY_BOOTPROTO = "BOOTPROTO=";
constexpr const char *KEY_STATIC = "STATIC";
constexpr const char *KEY_DHCP = "DHCP";
constexpr const char *KEY_IPADDR = "IPADDR=";
constexpr const char *KEY_NETMASK = "NETMASK=";
constexpr const char *KEY_GATEWAY = "GATEWAY=";
constexpr const char *KEY_ROUTE = "ROUTE=";
constexpr const char *KEY_ROUTE_NETMASK = "ROUTE_NETMASK=";
constexpr const char *KEY_DNS = "DNS=";
constexpr const char *WRAP = "\n";
constexpr const char *DEFAULT_NET_ADDR = "0.0.0.0";
constexpr const char *EMPTY_NET_ADDR = "*";
constexpr const char *DNS_SEPARATOR = ",";
} // namespace

EthernetConfiguration::EthernetConfiguration()
{
    CreateDir(USER_CONFIG_DIR);
}

bool EthernetConfiguration::ReadSysteamConfiguration(std::map<std::string, std::set<NetCap>> &devCaps,
                                                     std::map<std::string, sptr<InterfaceConfiguration>> &devCfgs)
{
    const auto &jsonStr = ReadJsonFile(NETWORK_CONFIG_PATH);
    if (jsonStr.length() == 0) {
        NETMGR_EXT_LOG_E("ReadConfigData config file is return empty!");
        return false;
    }
    const auto &jsonCfg = nlohmann::json::parse(jsonStr);
    if (jsonCfg.find(CONFIG_KEY_ETH_COMPONENT_FLAG) == jsonCfg.end()) {
        NETMGR_EXT_LOG_E("ReadConfigData not find network_ethernet_component!");
        return false;
    }
    const auto &arrIface = jsonCfg.at(CONFIG_KEY_ETH_COMPONENT_FLAG);
    NETMGR_EXT_LOG_D("read ConfigData ethValue:%{public}s", arrIface.dump().c_str());
    for (const auto &item : arrIface) {
        const auto &iface = item[CONFIG_KEY_ETH_IFACE].get<std::string>();
        const auto &caps = item.at(CONFIG_KEY_ETH_CAPS).get<std::set<NetCap>>();
        if (!caps.empty()) {
            devCaps[iface] = caps;
        }
        const auto &fit = devCfgs.find(iface);
        if (fit != devCfgs.end()) {
            NETMGR_EXT_LOG_E("The iface=%{public}s device have set!", fit->first.c_str());
            continue;
        }
        sptr<InterfaceConfiguration> config = ConvertJsonToConfiguration(item);
        if (config == nullptr) {
            NETMGR_EXT_LOG_E("config is nullptr");
            return false;
        }
        std::regex re(IFACE_MATCH);
        if (!item[CONFIG_KEY_ETH_IP].empty() && std::regex_search(iface, re)) {
            devCfgs[iface] = config;
        }
    }
    return true;
}

sptr<InterfaceConfiguration> EthernetConfiguration::ConvertJsonToConfiguration(const nlohmann::json &jsonData)
{
    sptr<InterfaceConfiguration> config = new (std::nothrow) InterfaceConfiguration();
    if (config == nullptr) {
        NETMGR_EXT_LOG_E("config is nullptr");
        return nullptr;
    }
    config->mode_ = STATIC;
    config->ipStatic_.ipAddr_.address_ = jsonData[CONFIG_KEY_ETH_IP];
    config->ipStatic_.ipAddr_.netMask_ = jsonData[CONFIG_KEY_ETH_NETMASK];
    config->ipStatic_.ipAddr_.family_ = static_cast<uint8_t>(CommonUtils::GetAddrFamily(jsonData[CONFIG_KEY_ETH_IP]));
    int prefixLen = CommonUtils::GetMaskLength(jsonData[CONFIG_KEY_ETH_NETMASK]);
    if (config->ipStatic_.ipAddr_.family_ == AF_INET) {
        config->ipStatic_.ipAddr_.prefixlen_ = prefixLen;
    }
    config->ipStatic_.netMask_.address_ = jsonData[CONFIG_KEY_ETH_NETMASK];
    config->ipStatic_.gateway_.address_ = jsonData[CONFIG_KEY_ETH_GATEWAY];
    config->ipStatic_.gateway_.family_ =
        static_cast<uint8_t>(CommonUtils::GetAddrFamily(jsonData[CONFIG_KEY_ETH_GATEWAY]));
    if (config->ipStatic_.gateway_.family_ == AF_INET) {
        config->ipStatic_.gateway_.prefixlen_ = prefixLen;
    }
    config->ipStatic_.route_.address_ = jsonData[CONFIG_KEY_ETH_ROUTE];
    int routePrefixLen = 0;
    if (!jsonData[CONFIG_KEY_ETH_ROUTE_MASK].empty()) {
        routePrefixLen = CommonUtils::GetMaskLength(jsonData[CONFIG_KEY_ETH_ROUTE_MASK]);
    }
    config->ipStatic_.route_.family_ =
        static_cast<uint8_t>(CommonUtils::GetAddrFamily(jsonData[CONFIG_KEY_ETH_ROUTE]));
    if (config->ipStatic_.route_.family_ == AF_INET) {
        config->ipStatic_.route_.prefixlen_ = routePrefixLen;
    }
    std::vector<std::string> servers = CommonUtils::Split(jsonData[CONFIG_KEY_ETH_DNS], DNS_SEPARATOR);
    for (const auto &dns : servers) {
        INetAddr addr;
        addr.address_ = dns;
        config->ipStatic_.dnsServers_.push_back(addr);
    }
    return config;
}

bool EthernetConfiguration::ReadUserConfiguration(std::map<std::string, sptr<InterfaceConfiguration>> &devCfgs)
{
    DIR *dir = nullptr;
    dirent *ptr = nullptr;
    if ((dir = opendir(USER_CONFIG_DIR)) == nullptr) {
        NETMGR_EXT_LOG_E("Read user configuration open dir error dir=[%{public}s]", USER_CONFIG_DIR);
        return false;
    }
    std::string iface;
    while ((ptr = readdir(dir)) != nullptr) {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
            continue;
        }
        if (ptr->d_type == DT_REG) {
            std::string filePath = std::string(USER_CONFIG_DIR) + FILE_OBLIQUE_LINE + ptr->d_name;
            std::string fileContent;
            if (!ReadFile(filePath, fileContent)) {
                continue;
            }
            std::string().swap(iface);
            sptr<InterfaceConfiguration> cfg = new (std::nothrow) InterfaceConfiguration();
            if (cfg == nullptr) {
                NETMGR_EXT_LOG_E("cfg new failed for devname[%{public}s]", iface.c_str());
                continue;
            }
            ParserFileConfig(fileContent, iface, cfg);
            std::regex re(IFACE_MATCH);
            if (!iface.empty() && std::regex_search(iface, re)) {
                NETMGR_EXT_LOG_D("ReadFileList devname[%{public}s]", iface.c_str());
                devCfgs[iface] = cfg;
            }
        }
    }
    closedir(dir);
    return true;
}

bool EthernetConfiguration::WriteUserConfiguration(const std::string &iface, sptr<InterfaceConfiguration> &cfg)
{
    if (cfg == nullptr) {
        NETMGR_EXT_LOG_E("cfg is nullptr");
        return false;
    }
    if (!CreateDir(USER_CONFIG_DIR)) {
        NETMGR_EXT_LOG_E("create dir failed");
        return false;
    }
    if (cfg->mode_ == STATIC) {
        int prefixlen = 0;
        cfg->ipStatic_.ipAddr_.family_ =
            static_cast<uint8_t>(CommonUtils::GetAddrFamily(cfg->ipStatic_.ipAddr_.address_));
        if (cfg->ipStatic_.ipAddr_.family_ == AF_INET) {
            if (cfg->ipStatic_.netMask_.address_.empty()) {
                prefixlen = CommonUtils::GetMaskLength(cfg->ipStatic_.ipAddr_.netMask_);
            } else {
                prefixlen = CommonUtils::GetMaskLength(cfg->ipStatic_.netMask_.address_);
            }
        }
        cfg->ipStatic_.ipAddr_.prefixlen_ = prefixlen;
        cfg->ipStatic_.gateway_.family_ =
            static_cast<uint8_t>(CommonUtils::GetAddrFamily(cfg->ipStatic_.gateway_.address_));
        cfg->ipStatic_.gateway_.prefixlen_ = prefixlen;
        cfg->ipStatic_.route_.family_ =
            static_cast<uint8_t>(CommonUtils::GetAddrFamily(cfg->ipStatic_.route_.address_));
        int routePrefixLen = 0;
        if (!cfg->ipStatic_.route_.netMask_.empty()) {
            routePrefixLen = CommonUtils::GetMaskLength(cfg->ipStatic_.route_.netMask_);
        }
        cfg->ipStatic_.route_.prefixlen_ = routePrefixLen;
    }
    std::string fileContent;
    std::string filePath = std::string(USER_CONFIG_DIR) + FILE_OBLIQUE_LINE + iface;
    GenCfgContent(iface, cfg, fileContent);
    return WriteFile(filePath, fileContent);
}

bool EthernetConfiguration::ClearAllUserConfiguration()
{
    return DelDir(USER_CONFIG_DIR);
}

bool EthernetConfiguration::ConvertToConfiguration(const EthernetDhcpCallback::DhcpResult &dhcpResult,
                                                   sptr<StaticConfiguration> &config)
{
    if (config == nullptr) {
        NETMGR_EXT_LOG_E("Error ConvertToIpConfiguration config is null");
        return false;
    }
    const auto &emPrefixlen = 0;
    unsigned int prefixlen = 0;
    config->ipAddr_.address_ = dhcpResult.ipAddr;
    config->netMask_.address_ = dhcpResult.subNet;
    config->ipAddr_.family_ = static_cast<uint8_t>(CommonUtils::GetAddrFamily(dhcpResult.ipAddr));
    if (config->ipAddr_.family_ == AF_INET) {
        config->ipAddr_.prefixlen_ = static_cast<uint8_t>(CommonUtils::GetMaskLength(dhcpResult.subNet));
    }
    prefixlen = config->ipAddr_.prefixlen_;
    config->gateway_.address_ = dhcpResult.gateWay;
    config->gateway_.family_ = static_cast<uint8_t>(CommonUtils::GetAddrFamily(dhcpResult.gateWay));
    config->gateway_.prefixlen_ = prefixlen;
    if (dhcpResult.gateWay != dhcpResult.route1) {
        if (dhcpResult.route1 == EMPTY_NET_ADDR) {
            config->route_.address_ = DEFAULT_NET_ADDR;
            config->route_.prefixlen_ = emPrefixlen;
        } else {
            config->route_.address_ = dhcpResult.route1;
            config->route_.prefixlen_ = prefixlen;
        }
    }
    if (dhcpResult.gateWay != dhcpResult.route2) {
        if (dhcpResult.route2 == EMPTY_NET_ADDR) {
            config->route_.address_ = DEFAULT_NET_ADDR;
            config->route_.prefixlen_ = emPrefixlen;
        } else {
            config->route_.address_ = dhcpResult.route2;
            config->route_.prefixlen_ = prefixlen;
        }
    }
    config->route_.family_ = static_cast<uint8_t>(CommonUtils::GetAddrFamily(config->route_.address_));
    INetAddr dnsNet1;
    dnsNet1.address_ = dhcpResult.dns1;
    INetAddr dnsNet2;
    dnsNet2.address_ = dhcpResult.dns2;
    config->dnsServers_.push_back(dnsNet1);
    config->dnsServers_.push_back(dnsNet2);
    return true;
}

sptr<InterfaceConfiguration>
    EthernetConfiguration::MakeInterfaceConfiguration(const sptr<InterfaceConfiguration> &devCfg,
                                                      const sptr<NetLinkInfo> &devLinkInfo)
{
    if (devCfg == nullptr || devLinkInfo == nullptr) {
        NETMGR_EXT_LOG_E("param is nullptr");
        return nullptr;
    }
    sptr<InterfaceConfiguration> cfg = new (std::nothrow) InterfaceConfiguration();
    if (cfg == nullptr) {
        NETMGR_EXT_LOG_E("cfg new failed");
        return nullptr;
    }
    cfg->mode_ = devCfg->mode_;
    INetAddr addrNet = devLinkInfo->netAddrList_.back();
    Route route = devLinkInfo->routeList_.front();
    Route routeLocal = devLinkInfo->routeList_.back();
    cfg->ipStatic_.ipAddr_.address_ = addrNet.address_;
    cfg->ipStatic_.route_.address_ = route.destination_.address_;
    cfg->ipStatic_.gateway_.address_ = route.gateway_.address_;
    cfg->ipStatic_.netMask_.address_ = CommonUtils::GetMaskByLength(routeLocal.destination_.prefixlen_);
    cfg->ipStatic_.domain_ = devLinkInfo->domain_;
    for (const auto addr : devLinkInfo->dnsList_) {
        cfg->ipStatic_.dnsServers_.push_back(addr);
    }
    return cfg;
}

std::string EthernetConfiguration::ReadJsonFile(const std::string &filePath)
{
    std::ifstream infile;
    std::string strLine;
    std::string strAll;
    infile.open(filePath);
    if (!infile.is_open()) {
        NETMGR_EXT_LOG_E("ReadJsonFile filePath failed");
        return strAll;
    }
    while (getline(infile, strLine)) {
        strAll.append(strLine);
    }
    infile.close();
    return strAll;
}

bool EthernetConfiguration::IsDirExist(const std::string &dirPath)
{
    struct stat status;
    if (dirPath.empty()) {
        return false;
    }
    return (stat(dirPath.c_str(), &status) == 0);
}

bool EthernetConfiguration::CreateDir(const std::string &dirPath)
{
    if (IsDirExist(dirPath)) {
        return true;
    }
    if (mkdir(dirPath.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == MKDIR_ERR) {
        NETMGR_EXT_LOG_E("mkdir failed %{public}d: %{public}s", errno, strerror(errno));
        return false;
    }
    return true;
}

bool EthernetConfiguration::DelDir(const std::string &dirPath)
{
    DIR *dir = nullptr;
    dirent *entry = nullptr;
    struct stat statbuf;
    if ((dir = opendir(dirPath.c_str())) == nullptr) {
        NETMGR_EXT_LOG_E("EthernetConfiguration DelDir open user dir failed!");
        return false;
    }
    while ((entry = readdir(dir)) != nullptr) {
        std::string filePath = dirPath + FILE_OBLIQUE_LINE + entry->d_name;
        lstat(filePath.c_str(), &statbuf);
        if (S_ISREG(statbuf.st_mode)) {
            remove(filePath.c_str());
        }
    }
    closedir(dir);
    sync();
    return rmdir(dirPath.c_str()) >= 0;
}

bool EthernetConfiguration::IsFileExist(const std::string &filePath, std::string &realPath)
{
    char tmpPath[PATH_MAX] = {0};
    if (!realpath(filePath.c_str(), tmpPath)) {
        NETMGR_EXT_LOG_E("file name is error");
        return false;
    }
    if (strncmp(tmpPath, USER_CONFIG_DIR, USER_PATH_LEN) != 0) {
        NETMGR_EXT_LOG_E("file path is error");
        return false;
    }
    realPath = tmpPath;
    return true;
}

bool EthernetConfiguration::ReadFile(const std::string &filePath, std::string &fileContent)
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (filePath.empty()) {
        NETMGR_EXT_LOG_E("filePath empty.");
        return false;
    }
    std::string realPath;
    if (!IsFileExist(filePath, realPath)) {
        NETMGR_EXT_LOG_E("[%{public}s] not exist.", filePath.c_str());
        return false;
    }
    std::fstream file(realPath.c_str(), std::fstream::in);
    if (!file.is_open()) {
        NETMGR_EXT_LOG_E("EthernetConfiguration read file failed.err %{public}d %{public}s", errno, strerror(errno));
        return false;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    fileContent = buffer.str();
    file.close();
    return true;
}

bool EthernetConfiguration::WriteFile(const std::string &filePath, const std::string &fileContent)
{
    std::fstream file(filePath.c_str(), std::fstream::out | std::fstream::trunc);
    if (!file.is_open()) {
        NETMGR_EXT_LOG_E("EthernetConfiguration write file=%{public}s fstream failed. err %{public}d %{public}s",
                         filePath.c_str(), errno, strerror(errno));
        return false;
    }
    file << fileContent.c_str();
    file.close();
    sync();
    return true;
}

void EthernetConfiguration::ParserFileConfig(const std::string &fileContent, std::string &iface,
                                             sptr<InterfaceConfiguration> cfg)
{
    std::string::size_type pos = fileContent.find(KEY_DEVICE) + strlen(KEY_DEVICE);
    const auto &device = fileContent.substr(pos, fileContent.find(WRAP, pos) - pos);
    pos = fileContent.find(KEY_BOOTPROTO) + strlen(KEY_BOOTPROTO);
    const auto &bootProto = fileContent.substr(pos, fileContent.find(WRAP, pos) - pos);
    iface = device;
    if (bootProto == KEY_STATIC) {
        cfg->mode_ = STATIC;
        pos = fileContent.find(KEY_IPADDR) + strlen(KEY_IPADDR);
        const auto &ipAddr = fileContent.substr(pos, fileContent.find(WRAP, pos) - pos);
        pos = fileContent.find(KEY_NETMASK) + strlen(KEY_NETMASK);
        const auto &netMask = fileContent.substr(pos, fileContent.find(WRAP, pos) - pos);
        pos = fileContent.find(KEY_GATEWAY) + strlen(KEY_GATEWAY);
        const auto &gatway = fileContent.substr(pos, fileContent.find(WRAP, pos) - pos);
        pos = fileContent.find(KEY_ROUTE) + strlen(KEY_ROUTE);
        const auto &route = fileContent.substr(pos, fileContent.find(WRAP, pos) - pos);
        pos = fileContent.find(KEY_ROUTE_NETMASK) + strlen(KEY_ROUTE_NETMASK);
        const auto &routeNetmask = fileContent.substr(pos, fileContent.find(WRAP, pos) - pos);
        pos = fileContent.find(KEY_DNS) + strlen(KEY_DNS);
        const auto &dnsServers = fileContent.substr(pos, fileContent.find(WRAP, pos) - pos);
        cfg->ipStatic_.ipAddr_.address_ = ipAddr;
        cfg->ipStatic_.ipAddr_.netMask_ = netMask;
        cfg->ipStatic_.ipAddr_.family_ = CommonUtils::GetAddrFamily(ipAddr);
        int prefixLen = CommonUtils::GetMaskLength(netMask);
        if (cfg->ipStatic_.ipAddr_.family_ == AF_INET) {
            cfg->ipStatic_.ipAddr_.prefixlen_ = prefixLen;
        }
        cfg->ipStatic_.netMask_.address_ = netMask;
        cfg->ipStatic_.gateway_.address_ = gatway;
        cfg->ipStatic_.gateway_.family_ = CommonUtils::GetAddrFamily(gatway);
        if (cfg->ipStatic_.gateway_.family_ == AF_INET) {
            cfg->ipStatic_.gateway_.prefixlen_ = prefixLen;
        }
        cfg->ipStatic_.route_.address_ = !route.empty() ? route : DEFAULT_NET_ADDR;
        int routePrefixLen = 0;
        if (!routeNetmask.empty()) {
            routePrefixLen = CommonUtils::GetMaskLength(routeNetmask);
        }
        cfg->ipStatic_.route_.family_ = CommonUtils::GetAddrFamily(cfg->ipStatic_.route_.address_);
        if (cfg->ipStatic_.route_.family_ == AF_INET) {
            cfg->ipStatic_.route_.prefixlen_ = routePrefixLen;
        }
        for (const auto &dns : CommonUtils::Split(dnsServers, DNS_SEPARATOR)) {
            INetAddr addr;
            addr.address_ = dns;
            cfg->ipStatic_.dnsServers_.push_back(addr);
        }
    } else if (bootProto == KEY_DHCP) {
        cfg->mode_ = DHCP;
    }
}

void EthernetConfiguration::GenCfgContent(const std::string &iface, sptr<InterfaceConfiguration> cfg,
                                          std::string &fileContent)
{
    if (cfg == nullptr) {
        NETMGR_EXT_LOG_E("cfg is nullptr");
        return;
    }
    std::string().swap(fileContent);
    fileContent = fileContent + KEY_DEVICE + iface + WRAP;
    if (cfg->mode_ == STATIC) {
        fileContent = fileContent + KEY_BOOTPROTO + KEY_STATIC + WRAP;
        fileContent = fileContent + KEY_IPADDR + cfg->ipStatic_.ipAddr_.address_ + WRAP;
        if (cfg->ipStatic_.netMask_.address_.empty()) {
            fileContent = fileContent + KEY_NETMASK + cfg->ipStatic_.ipAddr_.netMask_ + WRAP;
        } else {
            fileContent = fileContent + KEY_NETMASK + cfg->ipStatic_.netMask_.address_ + WRAP;
        }
        fileContent = fileContent + KEY_GATEWAY + cfg->ipStatic_.gateway_.address_ + WRAP;
        fileContent = fileContent + KEY_ROUTE + cfg->ipStatic_.route_.address_ + WRAP;
        fileContent = fileContent + KEY_ROUTE_NETMASK + cfg->ipStatic_.route_.netMask_ + WRAP;
        fileContent = fileContent + KEY_DNS;
        for (uint32_t i = 0; i < cfg->ipStatic_.dnsServers_.size(); i++) {
            fileContent = fileContent + cfg->ipStatic_.dnsServers_[i].address_;
            if (cfg->ipStatic_.dnsServers_.size() - i > 1) {
                fileContent = fileContent + DNS_SEPARATOR;
            }
        }
        fileContent = fileContent + WRAP;
    } else {
        fileContent = fileContent + KEY_BOOTPROTO + KEY_DHCP + WRAP;
    }
}
} // namespace NetManagerStandard
} // namespace OHOS
