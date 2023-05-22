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

#include "mock_netsys_native_client.h"

#include <ctime>
#include <dirent.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/route.h>
#include <netdb.h>

#include "securec.h"

#include "net_conn_constants.h"
#include "net_mgr_log_wrapper.h"
#include "netmanager_base_common_utils.h"

using namespace OHOS::NetManagerStandard::CommonUtils;
namespace OHOS {
namespace NetManagerStandard {
namespace {
const std::string INTERFACE_LIST_DIR = "/sys/class/net/";
const std::string UID_LIST_DIR = "/data/data/uid/";
const std::string UID_TRAFFIC_BPF_PATH = "/dev/socket/traffic";
const std::string TEST_CELL_RX = "/data/cell_rx";
const std::string TEST_CELL_TX = "/data/cell_tx";
const std::string TEST_IFACE_RX = "/data/iface_rx";
const std::string TEST_IFACE_TX = "/data/iface_tx";
const std::string TEST_IFACE_RX_P = "/data/iface_rx_p";
const std::string TEST_IFACE_TX_P = "/data/iface_tx_p";
const std::string TEST_ALL_RX = "/data/all_tx";
const std::string TEST_ALL_TX = "/data/all_tx";
const std::string NET_STATS_FILE_RX_BYTES = "rx_bytes";
const std::string NET_STATS_FILE_TX_BYTES = "tx_bytes";
const std::string NET_STATS_FILE_RX_PACKETS = "rx_packets";
const std::string NET_STATS_FILE_TX_PACKETS = "tx_packets";
constexpr int32_t MOCK_MODULO_LAST_SIX_DIGITS = 100000;

bool CheckFilePath(const std::string &fileName, std::string &realPath)
{
    char tmpPath[PATH_MAX] = {0};
    if (!realpath(fileName.c_str(), tmpPath)) {
        NETMGR_LOG_E("file name is illegal");
        return false;
    }
    realPath = tmpPath;
    return true;
}
} // namespace

MockNetsysNativeClient::MockNetsysNativeClient()
{
    Init();
}

MockNetsysNativeClient::~MockNetsysNativeClient() {}

void MockNetsysNativeClient::Init()
{
    return;
}

void MockNetsysNativeClient::RegisterMockApi()
{
    mockApi_.insert(MOCK_INTERFACECLEARADDRS_API);
    mockApi_.insert(MOCK_GETCELLULARRXBYTES_API);
    mockApi_.insert(MOCK_GETCELLULARTXBYTES_API);
    mockApi_.insert(MOCK_GETALLRXBYTES_API);
    mockApi_.insert(MOCK_GETALLTXBYTES_API);
    mockApi_.insert(MOCK_GETUIDRXBYTES_API);
    mockApi_.insert(MOCK_GETUIDTXBYTES_API);
    mockApi_.insert(MOCK_GETUIDONIFACETXBYTES_API);
    mockApi_.insert(MOCK_GETUIDONIFACETXBYTES_API);
    mockApi_.insert(MOCK_GETIFACERXBYTES_API);
    mockApi_.insert(MOCK_GETIFACETXBYTES_API);
    mockApi_.insert(MOCK_INTERFACEGETLIST_API);
    mockApi_.insert(MOCK_UIDGETLIST_API);
    mockApi_.insert(MOCK_GETIFACERXPACKETS_API);
    mockApi_.insert(MOCK_GETIFACETXPACKETS_API);
    mockApi_.insert(MOCK_BINDSOCKET_API);
    mockApi_.insert(MOCK_SHAREDNSSET_API);
    mockApi_.insert(MOCK_REGISTERNETSYSNOTIFYCALLBACK_API);
    mockApi_.insert(MOCK_BINDNETWORKSERVICEVPN_API);
    mockApi_.insert(MOCK_ENABLEVIRTUALNETIFACECARD_API);
    mockApi_.insert(MOCK_SETIPADDRESS_API);
    mockApi_.insert(MOCK_SETBLOCKING_API);
}

bool MockNetsysNativeClient::CheckMockApi(const std::string &api)
{
    return mockApi_.find(api) != mockApi_.end();
}

int32_t MockNetsysNativeClient::NetworkCreatePhysical(int32_t netId, int32_t permission)
{
    NETMGR_LOG_I("Create Physical network: netId[%{public}d], permission[%{public}d]", netId, permission);
    return 0;
}

int32_t MockNetsysNativeClient::NetworkDestroy(int32_t netId)
{
    NETMGR_LOG_I("Destroy network: netId[%{public}d]", netId);
    return 0;
}

int32_t MockNetsysNativeClient::NetworkAddInterface(int32_t netId, const std::string &iface)
{
    NETMGR_LOG_I("Add network interface: netId[%{public}d], iface[%{public}s]", netId, iface.c_str());
    return 0;
}

int32_t MockNetsysNativeClient::NetworkRemoveInterface(int32_t netId, const std::string &iface)
{
    NETMGR_LOG_I("Remove network interface: netId[%{public}d], iface[%{public}s]", netId, iface.c_str());
    return 0;
}

int32_t MockNetsysNativeClient::NetworkAddRoute(int32_t netId, const std::string &ifName,
    const std::string &destination, const std::string &nextHop)
{
    NETMGR_LOG_I("Add Route: netId[%{public}d], ifName[%{public}s], destination[%{public}s], nextHop[%{public}s]",
        netId, ifName.c_str(), ToAnonymousIp(destination).c_str(), ToAnonymousIp(nextHop).c_str());
    std::string mask = "0.0.0.0";
    return AddRoute(destination, mask, nextHop, ifName);
}

int32_t MockNetsysNativeClient::NetworkRemoveRoute(int32_t netId, const std::string &ifName,
    const std::string &destination, const std::string &nextHop)
{
    NETMGR_LOG_I("Remove Route: netId[%{public}d], ifName[%{public}s], destination[%{public}s], nextHop[%{public}s]",
        netId, ifName.c_str(), ToAnonymousIp(destination).c_str(), ToAnonymousIp(nextHop).c_str());
    return 0;
}

int32_t MockNetsysNativeClient::SetInterfaceDown(const std::string &iface)
{
    NETMGR_LOG_I("Set interface down: iface[%{public}s]", iface.c_str());
    return 0;
}

int32_t MockNetsysNativeClient::SetInterfaceUp(const std::string &iface)
{
    NETMGR_LOG_I("Set interface up: iface[%{public}s]", iface.c_str());
    return 0;
}

void MockNetsysNativeClient::ClearInterfaceAddrs(const std::string &ifName)
{
    NETMGR_LOG_I("Clear addrs: ifName[%{public}s]", ifName.c_str());
    return;
}

int32_t MockNetsysNativeClient::GetInterfaceMtu(const std::string &ifName)
{
    NETMGR_LOG_I("Get mtu: ifName[%{public}s]", ifName.c_str());
    return 0;
}

int32_t MockNetsysNativeClient::SetInterfaceMtu(const std::string &ifName, int32_t mtu)
{
    NETMGR_LOG_I("Set mtu: ifName[%{public}s], mtu[%{public}d]", ifName.c_str(), mtu);
    return 0;
}

int32_t MockNetsysNativeClient::AddInterfaceAddress(const std::string &ifName, const std::string &ipAddr,
    int32_t prefixLength)
{
    NETMGR_LOG_I("Add address: ifName[%{public}s], ipAddr[%{public}s], prefixLength[%{public}d]",
        ifName.c_str(), ToAnonymousIp(ipAddr).c_str(), prefixLength);
    return 0;
}

int32_t MockNetsysNativeClient::DelInterfaceAddress(const std::string &ifName, const std::string &ipAddr,
    int32_t prefixLength)
{
    NETMGR_LOG_I("Delete address: ifName[%{public}s], ipAddr[%{public}s], prefixLength[%{public}d]",
        ifName.c_str(), ToAnonymousIp(ipAddr).c_str(), prefixLength);
    return 0;
}

int32_t MockNetsysNativeClient::SetResolverConfig(uint16_t netId, uint16_t baseTimeoutMsec, uint8_t retryCount,
    const std::vector<std::string> &servers, const std::vector<std::string> &domains)
{
    NETMGR_LOG_I("Set resolver config: netId[%{public}d]", netId);
    return 0;
}

int32_t MockNetsysNativeClient::GetResolverConfig(uint16_t netId, std::vector<std::string> &servers,
    std::vector<std::string> &domains, uint16_t &baseTimeoutMsec, uint8_t &retryCount)
{
    NETMGR_LOG_I("Get resolver config: netId[%{public}d]", netId);
    return 0;
}

int32_t MockNetsysNativeClient::CreateNetworkCache(uint16_t netId)
{
    NETMGR_LOG_I("create dns cache: netId[%{public}d]", netId);
    return 0;
}

int32_t MockNetsysNativeClient::DestroyNetworkCache(uint16_t netId)
{
    NETMGR_LOG_I("Destroy dns cache: netId[%{public}d]", netId);
    return 0;
}

static long GetInterfaceTrafficByType(const std::string &path, const std::string &type)
{
    if (path.empty()) {
        return -1;
    }
    std::string trafficPath = path + type;
    std::string realPath;
    if (!CheckFilePath(trafficPath, realPath)) {
        NETMGR_LOG_E("file does not exist! ");
        return -1;
    }
    int fd = open(realPath.c_str(), 0, 0666);
    if (fd == -1) {
        return -1;
    }
    char buf[100] = {0};
    if (read(fd, buf, sizeof(buf)) == -1) {
        close(fd);
        return -1;
    }
    close(fd);
    long infBytes = atol(buf);
    return infBytes;
}

int64_t MockNetsysNativeClient::GetCellularRxBytes()
{
    NETMGR_LOG_I("MockNetsysNativeClient GetCellularRxBytes");
    if (access(TEST_CELL_RX.c_str(), F_OK) == 0) {
        return 0;
    }
    return GetIfaceBytes("usb0", NET_STATS_FILE_RX_BYTES.c_str());
}

int64_t MockNetsysNativeClient::GetCellularTxBytes()
{
    NETMGR_LOG_I("MockNetsysNativeClient GetCellularTxBytes");
    if (access(TEST_CELL_TX.c_str(), F_OK) == 0) {
        return 0;
    }
    return GetIfaceBytes("usb0", NET_STATS_FILE_TX_BYTES.c_str());
}

int64_t MockNetsysNativeClient::GetAllBytes(const std::string &filename)
{
    NETMGR_LOG_I("MockNetsysNativeClient GetAllBytes");
    long allBytes = 0;
    std::vector<std::string> ifNameList = InterfaceGetList();
    if (ifNameList.empty()) {
        return allBytes;
    }
    for (auto iter = ifNameList.begin(); iter != ifNameList.end(); iter++) {
        if (*iter != "lo") {
            std::string base_traffic_path = INTERFACE_LIST_DIR + (*iter) + "/" + "statistics" + "/";
            long bytes = GetInterfaceTrafficByType(base_traffic_path, filename.c_str());
            allBytes = allBytes + bytes;
        }
    }
    return allBytes;
}

int64_t MockNetsysNativeClient::GetAllRxBytes()
{
    NETMGR_LOG_I("MockNetsysNativeClient GetAllRxBytes");
    if (access(TEST_ALL_RX.c_str(), F_OK) == 0) {
        return 0;
    }
    return GetAllBytes(NET_STATS_FILE_RX_BYTES.c_str());
}

int64_t MockNetsysNativeClient::GetAllTxBytes()
{
    NETMGR_LOG_I("MockNetsysNativeClient GetAllTxBytes");
    if (access(TEST_ALL_TX.c_str(), F_OK) == 0) {
        return 0;
    }
    return GetAllBytes(NET_STATS_FILE_TX_BYTES.c_str());
}

static long GetUidTrafficFromBpf(int uid, int cgroupType)
{
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        close(sock);
        return -1;
    }
    sockaddr_un s_un;
    s_un.sun_family = AF_UNIX;
    if (strcpy_s(s_un.sun_path, sizeof(s_un.sun_path), UID_TRAFFIC_BPF_PATH.c_str()) != 0) {
        close(sock);
        return -1;
    }
    if (connect(sock, reinterpret_cast<sockaddr *>(&s_un), sizeof(s_un)) != 0) {
        close(sock);
        return -1;
    }
    char buf[128];
    bzero(buf, sizeof(buf));
    std::string query = std::to_string(uid) + "," + std::to_string(cgroupType);
    if (strcpy_s(buf, sizeof(buf), query.c_str()) != 0) {
        close(sock);
        return -1;
    }
    ssize_t writeRet = write(sock, buf, strlen(buf));
    if (writeRet < 0) {
        close(sock);
        return -1;
    }
    bzero(buf, sizeof(buf));
    ssize_t readRet = read(sock, buf, sizeof(buf));
    if (readRet < 0) {
        close(sock);
        return -1;
    }
    close(sock);
    return atol(buf);
}

int64_t MockNetsysNativeClient::GetUidRxBytes(uint32_t uid)
{
    NETMGR_LOG_D("MockNetsysNativeClient GetUidRxBytes uid is [%{public}u]", uid);
    long result = GetUidTrafficFromBpf(uid, 0);
    return static_cast<int64_t>(result);
}

int64_t MockNetsysNativeClient::GetUidTxBytes(uint32_t uid)
{
    NETMGR_LOG_D("MockNetsysNativeClient GetUidTxBytes uid is [%{public}u]", uid);
    long result = GetUidTrafficFromBpf(uid, 1);
    return static_cast<int64_t>(result);
}

static int64_t GetUidOnIfaceBytes(uint32_t uid, const std::string &interfaceName)
{
    time_t now = time(nullptr);
    now %= MOCK_MODULO_LAST_SIX_DIGITS;
    return static_cast<int64_t>(now);
}

int64_t MockNetsysNativeClient::GetUidOnIfaceRxBytes(uint32_t uid, const std::string &interfaceName)
{
    NETMGR_LOG_D("MockNetsysNativeClient GetUidOnIfaceRxBytes uid is [%{public}u] "
        "iface name is [%{public}s]", uid, interfaceName.c_str());
    return GetUidOnIfaceBytes(uid, interfaceName);
}

int64_t MockNetsysNativeClient::GetUidOnIfaceTxBytes(uint32_t uid, const std::string &interfaceName)
{
    NETMGR_LOG_D("MockNetsysNativeClient GetUidOnIfaceRxBytes uid is [%{public}u] "
        "iface name is [%{public}s]", uid, interfaceName.c_str());
    return GetUidOnIfaceBytes(uid, interfaceName);
}

int64_t MockNetsysNativeClient::GetIfaceBytes(const std::string &interfaceName, const std::string &filename)
{
    int64_t bytes = 0;
    std::vector<std::string> ifNameList = InterfaceGetList();
    if (ifNameList.empty()) {
        return bytes;
    }
    for (auto iter = ifNameList.begin(); iter != ifNameList.end(); iter++) {
        if (interfaceName == *iter) {
            std::string baseTrafficPath = INTERFACE_LIST_DIR + (*iter) + "/" + "statistics" + "/";
            long infRxBytes = GetInterfaceTrafficByType(baseTrafficPath, filename.c_str());
            bytes = infRxBytes == -1 ? 0 : infRxBytes;
        }
    }
    return bytes;
}

int64_t MockNetsysNativeClient::GetIfaceRxBytes(const std::string &interfaceName)
{
    NETMGR_LOG_I("MockNetsysNativeClient GetIfaceRxBytes iface name is [%{public}s]", interfaceName.c_str());
    if (access(TEST_IFACE_RX.c_str(), F_OK) == 0) {
        return 0;
    }
    return GetIfaceBytes(interfaceName, NET_STATS_FILE_RX_BYTES.c_str());
}

int64_t MockNetsysNativeClient::GetIfaceTxBytes(const std::string &interfaceName)
{
    NETMGR_LOG_I("MockNetsysNativeClient GetIfaceTxBytes iface name is [%{public}s]", interfaceName.c_str());
    if (access(TEST_IFACE_TX.c_str(), F_OK) == 0) {
        return 0;
    }
    return GetIfaceBytes(interfaceName, NET_STATS_FILE_TX_BYTES.c_str());
}

int64_t MockNetsysNativeClient::GetIfaceRxPackets(const std::string &interfaceName)
{
    NETMGR_LOG_I("MockNetsysNativeClient GetIfaceRxBytes iface name is [%{public}s]", interfaceName.c_str());
    if (access(TEST_IFACE_RX_P.c_str(), F_OK) == 0) {
        return 0;
    }
    return GetIfaceBytes(interfaceName, NET_STATS_FILE_RX_PACKETS.c_str());
}

int64_t MockNetsysNativeClient::GetIfaceTxPackets(const std::string &interfaceName)
{
    NETMGR_LOG_I("MockNetsysNativeClient GetIfaceTxBytes iface name is [%{public}s]", interfaceName.c_str());
    if (access(TEST_IFACE_TX_P.c_str(), F_OK) == 0) {
        return 0;
    }
    return GetIfaceBytes(interfaceName, NET_STATS_FILE_TX_PACKETS.c_str());
}

std::vector<std::string> MockNetsysNativeClient::InterfaceGetList()
{
    NETMGR_LOG_I("MockNetsysNativeClient InterfaceGetList");
    DIR *dir(nullptr);
    std::vector<std::string> ifList;
    if ((dir = opendir(INTERFACE_LIST_DIR.c_str())) == nullptr) {
        return ifList;
    }
    struct dirent *ptr(nullptr);
    while ((ptr = readdir(dir)) != nullptr) {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
            continue;
        }
        ifList.push_back(ptr->d_name);
    }
    closedir(dir);
    return ifList;
}

std::vector<std::string> MockNetsysNativeClient::UidGetList()
{
    NETMGR_LOG_I("MockNetsysNativeClient UidGetList");
    DIR *dir(nullptr);
    std::vector<std::string> uidList;
    if ((dir = opendir(UID_LIST_DIR.c_str())) == nullptr) {
        return uidList;
    }
    struct dirent *ptr(nullptr);
    while ((ptr = readdir(dir)) != nullptr) {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) {
            continue;
        }
        uidList.push_back(ptr->d_name);
    }
    closedir(dir);
    return uidList;
}

int32_t MockNetsysNativeClient::AddRoute(const std::string &ip, const std::string &mask,
    const std::string &gateWay, const std::string &devName)
{
    struct sockaddr_in _sin;
    struct rtentry  rt;
    bzero(&rt, sizeof(struct rtentry));
    bzero(&_sin, sizeof(struct sockaddr_in));
    _sin.sin_family = AF_INET;
    _sin.sin_port = 0;
    if (inet_aton(gateWay.c_str(), &(_sin.sin_addr)) < 0) {
        NETMGR_LOG_E("MockNetsysNativeClient inet_aton gateWay[%{private}s]", gateWay.c_str());
        return -1;
    }
    int copyRet = memcpy_s(&rt.rt_gateway, sizeof(rt.rt_gateway), &_sin, sizeof(struct sockaddr_in));
    NETMGR_LOG_I("copyRet = %{public}d", copyRet);
    (reinterpret_cast<struct sockaddr_in *>(&rt.rt_dst))->sin_family = AF_INET;
    if (inet_aton(ip.c_str(), &((struct sockaddr_in *)&rt.rt_dst)->sin_addr) < 0) {
        NETMGR_LOG_E("MockNetsysNativeClient inet_aton ip[%{public}s]", ToAnonymousIp(ip).c_str());
        return -1;
    }
    (reinterpret_cast<struct sockaddr_in *>(&rt.rt_genmask))->sin_family = AF_INET;
    if (inet_aton(mask.c_str(), &(reinterpret_cast<struct sockaddr_in *>(&rt.rt_genmask))->sin_addr) < 0) {
        NETMGR_LOG_E("MockNetsysNativeClient inet_aton mask[%{public}s]", mask.c_str());
        return -1;
    }
    auto name = std::make_unique<char[]>(devName.size());
    if (!devName.empty()) {
        if (strcpy_s(name.get(), devName.size(), devName.c_str()) != 0) {
            return -1;
        }
        rt.rt_dev = name.get();
    }
    rt.rt_flags = RTF_GATEWAY;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        NETMGR_LOG_E("MockNetsysNativeClient create socket fd[%{public}d]", fd);
        return -1;
    }
    if (ioctl(fd, SIOCADDRT, &rt) < 0) {
        NETMGR_LOG_I("MockNetsysNativeClient ioctl error");
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

int32_t MockNetsysNativeClient::SetDefaultNetWork(int32_t netId)
{
    NETMGR_LOG_D("MockNetsysNativeClient SetDefaultNetWork netId is [%{public}d]", netId);
    return 0;
}

int32_t MockNetsysNativeClient::ClearDefaultNetWorkNetId()
{
    NETMGR_LOG_D("MockNetsysNativeClient ClearDefaultNetWorkNetId");
    return 0;
}

int32_t MockNetsysNativeClient::BindSocket(int32_t socket_fd, uint32_t netId)
{
    NETMGR_LOG_D("MockNetsysNativeClient::BindSocket: netId = [%{public}u]", netId);
    return NETMANAGER_SUCCESS;
}

int32_t MockNetsysNativeClient::ShareDnsSet(uint16_t netId)
{
    NETMGR_LOG_D("MockNetsysNativeClient ShareDnsSet: netId[%{public}d]", netId);
    return 0;
}

int32_t MockNetsysNativeClient::RegisterNetsysNotifyCallback(const NetsysNotifyCallback &callback)
{
    NETMGR_LOG_D("MockNetsysNativeClient RegisterNetsysNotifyCallback");
    return 0;
}

int32_t MockNetsysNativeClient::BindNetworkServiceVpn(int32_t socketFd)
{
    NETMGR_LOG_D("MockNetsysNativeClient::BindNetworkServiceVpn: socketFd[%{public}d]", socketFd);
    return 0;
}

int32_t MockNetsysNativeClient::EnableVirtualNetIfaceCard(int32_t socketFd, struct ifreq &ifRequest, int32_t &ifaceFd)
{
    NETMGR_LOG_D("MockNetsysNativeClient::EnableVirtualNetIfaceCard: socketFd[%{public}d]", socketFd);
    const char *ifaceName = "wlan0";
    strncpy_s(ifRequest.ifr_name, sizeof(ifRequest.ifr_name), ifaceName, strlen(ifaceName));
    NETMGR_LOG_D("ifRequest.ifr_name[%{public}s]", ifRequest.ifr_name);
    ifaceFd = 1;
    return 0;
}

int32_t MockNetsysNativeClient::SetIpAddress(int32_t socketFd, const std::string &ipAddress, int32_t prefixLen,
    struct ifreq &ifRequest)
{
    return 0;
}

int32_t MockNetsysNativeClient::SetBlocking(int32_t ifaceFd, bool isBlock)
{
    NETMGR_LOG_D("MockNetsysNativeClient::SetBlocking: ifaceFd[%{public}d], isBlock[%{public}d]", ifaceFd, isBlock);
    return 0;
}

int32_t MockNetsysNativeClient::StartDhcpClient(const std::string &iface, bool bIpv6)
{
    NETMGR_LOG_D("MockNetsysNativeClient::StartDhcpClient: iface[%{public}s], bIpv6[%{public}d]", iface.c_str(),
        bIpv6);
    return 0;
}

int32_t MockNetsysNativeClient::StopDhcpClient(const std::string &iface, bool bIpv6)
{
    NETMGR_LOG_D("MockNetsysNativeClient::StopDhcpClient: iface[%{public}s], bIpv6[%{public}d]", iface.c_str(),
        bIpv6);
    return 0;
}

int32_t MockNetsysNativeClient::RegisterCallback(sptr<NetsysControllerCallback> callback)
{
    NETMGR_LOG_D("MockNetsysNativeClient::RegisterCallback");
    return 0;
}

int32_t MockNetsysNativeClient::StartDhcpService(const std::string &iface, const std::string &ipv4addr)
{
    return 0;
}

int32_t MockNetsysNativeClient::StopDhcpService(const std::string &iface)
{
    return 0;
}
} // namespace NetManagerStandard
} // namespace OHOS
