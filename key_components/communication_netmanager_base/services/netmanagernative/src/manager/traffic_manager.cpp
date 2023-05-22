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

#include "traffic_manager.h"

#include <algorithm>
#include <dirent.h>
#include <fcntl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "net_manager_native.h"
#include "netnative_log_wrapper.h"
#include "securec.h"

namespace OHOS {
namespace nmd {
namespace {
constexpr const char *INTERFACE_LIST_DIR = "/sys/class/net/";
constexpr const char *STATISTICS = "statistics";
constexpr const char *RX_BYTES = "rx_bytes";
constexpr const char *TX_BYTES = "tx_bytes";
constexpr const char *RX_PACKETS = "rx_packets";
constexpr const char *TX_PACKETS = "tx_packets";
} // namespace

std::vector<std::string> GetInterfaceList()
{
    DIR *dir(nullptr);
    struct dirent *ptr(nullptr);
    std::vector<std::string> ifList;

    dir = opendir(INTERFACE_LIST_DIR);
    if (dir == nullptr) {
        NETNATIVE_LOGE("GetInterfaceList open %{private}s failed", INTERFACE_LIST_DIR);
        return ifList;
    }

    ptr = readdir(dir);
    while (ptr != nullptr) {
        if (strcmp(ptr->d_name, ".") != 0 && strcmp(ptr->d_name, "..") != 0) {
            ifList.push_back(ptr->d_name);
        }
        ptr = readdir(dir);
    }
    closedir(dir);

    return ifList;
}

long GetInterfaceTrafficByType(const std::string &path, const std::string &type)
{
    if (path.empty()) {
        return -1;
    }

    std::string trafficPath = path + type;

    char tmpPath[PATH_MAX] = {0};
    if (!realpath(trafficPath.c_str(), tmpPath)) {
        NETNATIVE_LOGE("file name is illegal");
        return -1;
    }

    int fd = open(tmpPath, 0, 0666);
    if (fd == -1) {
        NETNATIVE_LOGE("GetInterfaceTrafficByType open %{private}s failed", INTERFACE_LIST_DIR);
        return -1;
    }

    char buf[100] = {0};
    int nread = read(fd, buf, sizeof(long));
    if (nread == -1) {
        NETNATIVE_LOGE("GetInterfaceTrafficByType read %{private}s failed", INTERFACE_LIST_DIR);
        close(fd);
        return -1;
    }
    close(fd);

    return atol(buf);
}

long TrafficManager::GetAllRxTraffic()
{
    std::vector<std::string> ifNameList = GetInterfaceList();
    if (ifNameList.empty()) {
        return 0;
    }

    long allRxBytes = 0;
    for (auto iter = ifNameList.begin(); iter != ifNameList.end(); iter++) {
        if (*iter != "lo") {
            std::string baseTrafficPath = INTERFACE_LIST_DIR + (*iter) + "/" + STATISTICS + "/";
            long rxBytes = GetInterfaceTrafficByType(baseTrafficPath, RX_BYTES);
            allRxBytes += rxBytes;
        }
    }
    return allRxBytes;
}

long TrafficManager::GetAllTxTraffic()
{
    std::vector<std::string> ifNameList = GetInterfaceList();
    if (ifNameList.empty()) {
        return 0;
    }

    long allTxBytes = 0;
    for (auto iter = ifNameList.begin(); iter != ifNameList.end(); iter++) {
        if (*iter != "lo") {
            std::string baseTrafficPath = INTERFACE_LIST_DIR + (*iter) + "/" + STATISTICS + "/";
            long txBytes = GetInterfaceTrafficByType(baseTrafficPath, TX_BYTES);
            allTxBytes = allTxBytes + txBytes;
        }
    }
    return allTxBytes;
}

TrafficStatsParcel TrafficManager::GetInterfaceTraffic(const std::string &ifName)
{
    nmd::TrafficStatsParcel interfaceTrafficBytes = {"", 0, 0, 0, 0, 0};
    std::vector<std::string> ifNameList = GetInterfaceList();
    if (ifNameList.empty()) {
        return interfaceTrafficBytes;
    }
    for (auto iter = ifNameList.begin(); iter != ifNameList.end(); iter++) {
        if (ifName != *iter) {
            continue;
        }
        std::string baseTrafficPath = INTERFACE_LIST_DIR + (*iter) + "/" + STATISTICS + "/";
        long infRxBytes = GetInterfaceTrafficByType(baseTrafficPath, RX_BYTES);
        long infRxPackets = GetInterfaceTrafficByType(baseTrafficPath, RX_PACKETS);
        long infTxBytes = GetInterfaceTrafficByType(baseTrafficPath, TX_BYTES);
        long infTxPackets = GetInterfaceTrafficByType(baseTrafficPath, TX_PACKETS);

        interfaceTrafficBytes.iface = ifName;
        interfaceTrafficBytes.ifIndex = if_nametoindex(ifName.c_str());

        interfaceTrafficBytes.rxBytes = infRxBytes == -1 ? 0 : infRxBytes;
        interfaceTrafficBytes.rxPackets = infRxPackets == -1 ? 0 : infRxPackets;
        interfaceTrafficBytes.txBytes = infTxBytes == -1 ? 0 : infTxBytes;
        interfaceTrafficBytes.txPackets = infTxPackets == -1 ? 0 : infTxPackets;
    }
    return interfaceTrafficBytes;
}
} // namespace nmd
} // namespace OHOS
