/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "interface_manager.h"

#include <arpa/inet.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <linux/if_ether.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <system_error>
#include <unistd.h>

#include "netlink_manager.h"
#include "netlink_msg.h"
#include "netlink_socket.h"
#include "netmanager_base_common_utils.h"
#include "netnative_log_wrapper.h"
#include "securec.h"

namespace OHOS {
namespace nmd {
using namespace NetManagerStandard::CommonUtils;

namespace {
constexpr const char *SYS_NET_PATH = "/sys/class/net/";
constexpr const char *MTU_PATH = "/mtu";
constexpr int32_t FILE_PERMISSION = 0666;
constexpr uint32_t ARRAY_OFFSET_1_INDEX = 1;
constexpr uint32_t ARRAY_OFFSET_2_INDEX = 2;
constexpr uint32_t ARRAY_OFFSET_3_INDEX = 3;
constexpr uint32_t ARRAY_OFFSET_4_INDEX = 4;
constexpr uint32_t ARRAY_OFFSET_5_INDEX = 5;
constexpr uint32_t MOVE_BIT_LEFT31 = 31;
constexpr uint32_t BIT_MAX = 32;
constexpr uint32_t IOCTL_RETRY_TIME = 32;
constexpr int32_t MAX_MTU_LEN = 11;

bool CheckFilePath(const std::string &fileName, std::string &realPath)
{
    char tmpPath[PATH_MAX] = {0};
    if (!realpath(fileName.c_str(), tmpPath)) {
        NETNATIVE_LOGE("file name is illegal");
        return false;
    }
    realPath = tmpPath;
    return true;
}
} // namespace

int InterfaceManager::GetMtu(const char *interfaceName)
{
    if (!CheckIfaceName(interfaceName)) {
        NETNATIVE_LOGE("InterfaceManager::GetMtu isIfaceName fail %{public}d", errno);
        return -1;
    }
    std::string mtuPath = std::string(SYS_NET_PATH).append(interfaceName).append(MTU_PATH);
    std::string realPath;
    if (!CheckFilePath(mtuPath, realPath)) {
        NETNATIVE_LOGE("file does not exist! ");
        return -1;
    }
    int fd = open(realPath.c_str(), 0, FILE_PERMISSION);
    if (fd == -1) {
        NETNATIVE_LOGE("InterfaceManager::GetMtu open fail %{public}d", errno);
        return -1;
    }

    char originMtuValue[MAX_MTU_LEN] = {0};
    int nread = read(fd, originMtuValue, (sizeof(char) * (MAX_MTU_LEN - 1)));
    if (nread == -1 || nread == 0) {
        NETNATIVE_LOGE("InterfaceManager::GetMtu read fail %{public}d", errno);
        close(fd);
        return -1;
    }
    close(fd);

    int32_t mtu = -1;
    (void)NetManagerStandard::CommonUtils::ParseInt(originMtuValue, &mtu);
    return mtu;
}

int InterfaceManager::SetMtu(const char *interfaceName, const char *mtuValue)
{
    if (!CheckIfaceName(interfaceName)) {
        NETNATIVE_LOGE("InterfaceManager::SetMtu isIfaceName fail %{public}d", errno);
        return -1;
    }
    std::string mtuPath = std::string(SYS_NET_PATH).append(interfaceName).append(MTU_PATH);
    int fd = open(mtuPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_CLOEXEC, FILE_PERMISSION);
    if (fd == -1) {
        NETNATIVE_LOGE("InterfaceManager::SetMtu open fail %{public}d", errno);
        return -1;
    }
    int nwrite = write(fd, mtuValue, strlen(mtuValue));
    if (nwrite == -1) {
        NETNATIVE_LOGE("InterfaceManager::SetMtu write fail %{public}d", errno);
        close(fd);
        return -1;
    }
    close(fd);

    return 0;
}

std::vector<std::string> InterfaceManager::GetInterfaceNames()
{
    std::vector<std::string> ifaceNames;
    DIR *dir(nullptr);
    struct dirent *de(nullptr);

    dir = opendir(SYS_NET_PATH);
    if (dir == nullptr) {
        NETNATIVE_LOGE("InterfaceManager::GetInterfaceNames opendir fail %{public}d", errno);
        return ifaceNames;
    }

    de = readdir(dir);
    while (de != nullptr) {
        if ((de->d_name[0] != '.') && ((de->d_type == DT_DIR) || (de->d_type == DT_LNK))) {
            ifaceNames.push_back(std::string(de->d_name));
        }
        de = readdir(dir);
    }
    closedir(dir);

    return ifaceNames;
}

int InterfaceManager::ModifyAddress(uint32_t action, const char *interfaceName, const char *addr, int prefixLen)
{
    if (interfaceName == nullptr) {
        return -1;
    }
    uint32_t index = if_nametoindex(interfaceName);
    if (index == 0) {
        NETNATIVE_LOGE("InterfaceManager::ModifyAddress, if_nametoindex error %{public}d", errno);
        return -errno;
    }

    nmd::NetlinkMsg nlmsg(NLM_F_CREATE | NLM_F_EXCL, nmd::NETLINK_MAX_LEN, getpid());

    struct ifaddrmsg ifm = {0};
    ifm.ifa_family = AF_INET;
    ifm.ifa_index = index;
    ifm.ifa_scope = 0;
    ifm.ifa_prefixlen = static_cast<uint32_t>(prefixLen);

    nlmsg.AddAddress(action, ifm);

    struct in_addr inAddr;
    int ret = inet_pton(AF_INET, addr, &inAddr);
    if (ret == -1) {
        NETNATIVE_LOGE("InterfaceManager::ModifyAddress, inet_pton error %{public}d", errno);
        return -errno;
    }

    nlmsg.AddAttr(IFA_LOCAL, &inAddr, sizeof(inAddr));

    if (action == RTM_NEWADDR) {
        inAddr.s_addr |= htonl((1U << (BIT_MAX - prefixLen)) - 1);
        nlmsg.AddAttr(IFA_BROADCAST, &inAddr, sizeof(inAddr));
    }

    NETNATIVE_LOGI("InterfaceManager::ModifyAddress:%{public}u %{public}s %{public}s %{public}d", action, interfaceName,
                   ToAnonymousIp(addr).c_str(), prefixLen);

    ret = SendNetlinkMsgToKernel(nlmsg.GetNetLinkMessage());
    if (ret < 0) {
        return -EIO;
    }

    return 0;
}

int InterfaceManager::AddAddress(const char *interfaceName, const char *addr, int prefixLen)
{
    return ModifyAddress(RTM_NEWADDR, interfaceName, addr, prefixLen);
}

int InterfaceManager::DelAddress(const char *interfaceName, const char *addr, int prefixLen)
{
    return ModifyAddress(RTM_DELADDR, interfaceName, addr, prefixLen);
}

int Ipv4NetmaskToPrefixLength(in_addr_t mask)
{
    int prefixLength = 0;
    uint32_t m = ntohl(mask);
    const uint32_t referenceValue = 1;
    while (m & (referenceValue << MOVE_BIT_LEFT31)) {
        prefixLength++;
        m = m << referenceValue;
    }
    return prefixLength;
}

std::string HwAddrToStr(char *hwaddr)
{
    char buf[64] = {'\0'};
    if (hwaddr != nullptr) {
        errno_t result =
            sprintf_s(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x", hwaddr[0], hwaddr[ARRAY_OFFSET_1_INDEX],
                      hwaddr[ARRAY_OFFSET_2_INDEX], hwaddr[ARRAY_OFFSET_3_INDEX], hwaddr[ARRAY_OFFSET_4_INDEX],
                      hwaddr[ARRAY_OFFSET_5_INDEX]);
        if (result != 0) {
            NETNATIVE_LOGE("[hwAddrToStr]: result %{public}d", result);
        }
    }
    return std::string(buf);
}

void UpdateIfaceConfigFlags(unsigned flags, nmd::InterfaceConfigurationParcel &ifaceConfig)
{
    ifaceConfig.flags.emplace_back(flags & IFF_UP ? "up" : "down");
    if (flags & IFF_BROADCAST) {
        ifaceConfig.flags.emplace_back("broadcast");
    }
    if (flags & IFF_LOOPBACK) {
        ifaceConfig.flags.emplace_back("loopback");
    }
    if (flags & IFF_POINTOPOINT) {
        ifaceConfig.flags.emplace_back("point-to-point");
    }
    if (flags & IFF_RUNNING) {
        ifaceConfig.flags.emplace_back("running");
    }
    if (flags & IFF_MULTICAST) {
        ifaceConfig.flags.emplace_back("multicast");
    }
}

InterfaceConfigurationParcel InterfaceManager::GetIfaceConfig(const std::string &ifName)
{
    NETNATIVE_LOGI("GetIfaceConfig in. ifName %{public}s", ifName.c_str());
    struct in_addr addr = {};
    nmd::InterfaceConfigurationParcel ifaceConfig;

    int fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
    struct ifreq ifr = {};
    strncpy_s(ifr.ifr_name, IFNAMSIZ, ifName.c_str(), ifName.length());

    ifaceConfig.ifName = ifName;
    if (ioctl(fd, SIOCGIFADDR, &ifr) != -1) {
        addr.s_addr = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
        ifaceConfig.ipv4Addr = std::string(inet_ntoa(addr));
    }
    if (ioctl(fd, SIOCGIFNETMASK, &ifr) != -1) {
        ifaceConfig.prefixLength = Ipv4NetmaskToPrefixLength(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr);
    }
    if (ioctl(fd, SIOCGIFFLAGS, &ifr) != -1) {
        UpdateIfaceConfigFlags(ifr.ifr_flags, ifaceConfig);
    }
    if (ioctl(fd, SIOCGIFHWADDR, &ifr) != -1) {
        ifaceConfig.hwAddr = HwAddrToStr(ifr.ifr_hwaddr.sa_data);
    }
    close(fd);
    return ifaceConfig;
}

int InterfaceManager::SetIfaceConfig(const nmd::InterfaceConfigurationParcel &ifaceConfig)
{
    struct ifreq ifr = {};
    if (strncpy_s(ifr.ifr_name, IFNAMSIZ, ifaceConfig.ifName.c_str(), ifaceConfig.ifName.length()) != 0) {
        NETNATIVE_LOGE("ifaceConfig strncpy_s error.");
        return -1;
    }

    if (ifaceConfig.flags.empty()) {
        NETNATIVE_LOGE("ifaceConfig flags is empty.");
        return -1;
    }
    int fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, 0);
    if (fd < 0) {
        NETNATIVE_LOGE("ifaceConfig socket error, errno[%{public}d]", errno);
        return -1;
    }
    if (ioctl(fd, SIOCGIFFLAGS, &ifr) == -1) {
        char errmsg[INTERFACE_ERR_MAX_LEN] = {0};
        strerror_r(errno, errmsg, INTERFACE_ERR_MAX_LEN);
        NETNATIVE_LOGE("fail to set interface config. strerror[%{public}s]", errmsg);
        close(fd);
        return -1;
    }
    short flags = ifr.ifr_flags;
    auto fit = std::find(ifaceConfig.flags.begin(), ifaceConfig.flags.end(), "up");
    if (fit != std::end(ifaceConfig.flags)) {
        uint16_t ifrFlags = static_cast<uint16_t>(ifr.ifr_flags);
        ifrFlags = ifrFlags | IFF_UP;
        ifr.ifr_flags = static_cast<short>(ifrFlags);
    }
    fit = std::find(ifaceConfig.flags.begin(), ifaceConfig.flags.end(), "down");
    if (fit != std::end(ifaceConfig.flags)) {
        ifr.ifr_flags = (short)((uint16_t)ifr.ifr_flags & (~IFF_UP));
    }
    if (ifr.ifr_flags == flags) {
        close(fd);
        return 1;
    }
    int retry = 0;
    do {
        if (ioctl(fd, SIOCSIFFLAGS, &ifr) != -1) {
            break;
        }
        ++retry;
    } while (errno == ETIMEDOUT && retry < IOCTL_RETRY_TIME);
    NETNATIVE_LOGI("set ifr flags=[%{public}d] strerror=[%{public}s] retry=[%{public}d]", ifr.ifr_flags,
                   strerror(errno), retry);
    close(fd);
    return 1;
}

int InterfaceManager::SetIpAddress(const std::string &ifaceName, const std::string &ipAddress)
{
    struct ifreq ifr;
    struct in_addr ipv4Addr = {INADDR_ANY};

    if (memset_s(&ifr, sizeof(ifr), 0, sizeof(ifr)) != EOK) {
        NETNATIVE_LOGE("memset is false");
        return -1;
    }
    if (strncpy_s(ifr.ifr_name, IFNAMSIZ, ifaceName.c_str(), strlen(ifaceName.c_str())) != EOK) {
        NETNATIVE_LOGE("strncpy is false");
        return -1;
    }
    if (inet_aton(ipAddress.c_str(), &ipv4Addr) == 0) {
        NETNATIVE_LOGE("set net ip is false");
        return -1;
    }
    sockaddr_in *sin = reinterpret_cast<struct sockaddr_in *>(&ifr.ifr_addr);
    sin->sin_family = AF_INET;
    sin->sin_port = 0;
    sin->sin_addr = ipv4Addr;
    int32_t inetSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (ioctl(inetSocket, SIOCSIFADDR, &ifr) < 0) {
        NETNATIVE_LOGE("set ip address ioctl SIOCSIFADDR error: %{public}s", strerror(errno));
        close(inetSocket);
        return -1;
    }
    close(inetSocket);
    return 0;
}

int InterfaceManager::SetIffUp(const std::string &ifaceName)
{
    struct ifreq ifr;

    if (memset_s(&ifr, sizeof(ifr), 0, sizeof(ifr)) != EOK) {
        NETNATIVE_LOGE("memset is false");
        return -1;
    }
    if (strncpy_s(ifr.ifr_name, IFNAMSIZ, ifaceName.c_str(), strlen(ifaceName.c_str())) != EOK) {
        NETNATIVE_LOGE("strncpy is false");
        return -1;
    }
    uint32_t flagVal = (IFF_UP | IFF_MULTICAST);
    ifr.ifr_flags = static_cast<short int>(flagVal);

    int32_t inetSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (ioctl(inetSocket, SIOCSIFFLAGS, &ifr) < 0) {
        NETNATIVE_LOGE("set iface up ioctl SIOCSIFFLAGS error: %{public}s", strerror(errno));
        close(inetSocket);
        return -1;
    }
    close(inetSocket);
    return 0;
}
} // namespace nmd
} // namespace OHOS
