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

#include "netsys_bpf_stats.h"

#include <vector>

#include <sys/resource.h>

#include <linux/bpf.h>

#include "securec.h"

#include "net_stats_constants.h"
#include "netnative_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
constexpr const char *IFACE_STATS_MAP_PATH = "/sys/fs/bpf/netsys_iface_stats_map";
constexpr const char *IFACE_INDEX_NAME_MAP_PATH = "/sys/fs/bpf/netsys_iface_name_map";
constexpr const char *APP_UID_STATS_MAP_PATH = "/sys/fs/bpf/netsys_app_uid_stats_map";
} // namespace

bool NetsysBpfStats::IsStatsValueValid(StatsValue value)
{
    return (value.rxBytes >= 0 && value.rxPackets >= 0 && value.txBytes >= 0 && value.txPackets >= 0);
}

int32_t NetsysBpfStats::DumpError(int32_t errorCode)
{
    if (errorCode != NETMANAGER_SUCCESS) {
        NETNATIVE_LOGE("Error at bpf reader, code %{public}d, errno: %{public}d, err: %{public}s", errorCode, errno,
                       strerror(errno));
    }
    return errorCode;
}

int32_t NetsysBpfStats::GetTotalStats(uint64_t &stats, StatsType statsType)
{
    NetsysBpfMap<uint32_t, StatsValue> ifaceStatsMap(IFACE_STATS_MAP_PATH, BPF_F_RDONLY);
    if (!ifaceStatsMap.IsValid()) {
        return DumpError(STATS_ERR_INVALID_IFACE_STATS_MAP);
    }
    return NetsysBpfStats::BpfGetTotalStats(stats, statsType, ifaceStatsMap);
}

int32_t NetsysBpfStats::BpfGetTotalStats(uint64_t &stats, StatsType statsType,
                                         const NetsysBpfMap<uint32_t, StatsValue> &ifaceStatsMap)
{
    StatsValue totalStats = {0};
    uint32_t lookupKey, nextKey;
    lookupKey = -1;
    while (ifaceStatsMap.GetNextKeyFromStatsMap(lookupKey, nextKey) == 0) {
        lookupKey = nextKey;
        StatsValue statsValue = ifaceStatsMap.ReadValueFromMap(lookupKey);
        if (!IsStatsValueValid(statsValue)) {
            return DumpError(STATS_ERR_INVALID_STATS_VALUE);
        }
        totalStats += statsValue;
    }
    switch (statsType) {
        case StatsType::STATS_TYPE_RX_BYTES:
            stats = totalStats.rxBytes;
            break;
        case StatsType::STATS_TYPE_RX_PACKETS:
            stats = totalStats.rxPackets;
            break;
        case StatsType::STATS_TYPE_TX_BYTES:
            stats = totalStats.txBytes;
            break;
        case StatsType::STATS_TYPE_TX_PACKETS:
            stats = totalStats.txPackets;
            break;
        default:
            return DumpError(STATS_ERR_INVALID_STATS_TYPE);
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysBpfStats::BpfGetUidStats(uint64_t &stats, StatsType statsType, uint32_t uid,
                                       const NetsysBpfMap<uint32_t, StatsValue> &appUidStatsMap)
{
    StatsValue uidStats = {0};
    StatsValue statsValue = appUidStatsMap.ReadValueFromMap(uid);
    if (!IsStatsValueValid(statsValue)) {
        return DumpError(STATS_ERR_INVALID_STATS_VALUE);
    }
    uidStats += statsValue;
    switch (statsType) {
        case StatsType::STATS_TYPE_RX_BYTES:
            stats = uidStats.rxBytes;
            break;
        case StatsType::STATS_TYPE_RX_PACKETS:
            stats = uidStats.rxPackets;
            break;
        case StatsType::STATS_TYPE_TX_BYTES:
            stats = uidStats.txBytes;
            break;
        case StatsType::STATS_TYPE_TX_PACKETS:
            stats = uidStats.txPackets;
            break;
        default:
            return DumpError(STATS_ERR_INVALID_STATS_TYPE);
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysBpfStats::GetUidStats(uint64_t &stats, StatsType type, uint32_t uid)
{
    NetsysBpfMap<uint32_t, StatsValue> appUidStatsMap(APP_UID_STATS_MAP_PATH, BPF_F_RDONLY);
    if (!appUidStatsMap.IsValid()) {
        return DumpError(STATS_ERR_INVALID_UID_STATS_MAP);
    }
    return BpfGetUidStats(stats, type, uid, appUidStatsMap);
}

int32_t NetsysBpfStats::GetAllStatsInfo(std::vector<StatsValue> &stats)
{
    stats = {};
    return 0;
}

int32_t NetsysBpfStats::BpfGetIfaceStats(uint64_t &stats, const StatsType statsType, const std::string &interfaceName,
                                         const NetsysBpfMap<uint32_t, IfaceName> &ifaceNameMap,
                                         const NetsysBpfMap<uint32_t, StatsValue> &ifaceStatsMap)
{
    StatsValue ifaceStats = {0};
    std::string ifName;
    const auto executeIfaceStats = [&interfaceName, &ifaceNameMap, &ifaceStats, &ifName,
                                    this](const uint32_t key,
                                          const NetsysBpfMap<uint32_t, StatsValue> &ifaceStatsMap) -> void {
        if (GetIfaceName(ifaceNameMap, key, ifName)) {
            NETNATIVE_LOGE("Get iface name failed");
            return;
        }
        if (ifName == interfaceName) {
            StatsValue statsValue = ifaceStatsMap.ReadValueFromMap(key);
            if (!IsStatsValueValid(statsValue)) {
                NETNATIVE_LOGE("Error key is %{public}u", key);
                return;
            }
            ifaceStats += statsValue;
        }
    };
    ifaceStatsMap.Iterate(executeIfaceStats);
    switch (statsType) {
        case StatsType::STATS_TYPE_RX_BYTES:
            stats = ifaceStats.rxBytes;
            break;
        case StatsType::STATS_TYPE_RX_PACKETS:
            stats = ifaceStats.rxPackets;
            break;
        case StatsType::STATS_TYPE_TX_BYTES:
            stats = ifaceStats.txBytes;
            break;
        case StatsType::STATS_TYPE_TX_PACKETS:
            stats = ifaceStats.txPackets;
            break;
        default:
            return DumpError(STATS_ERR_INVALID_STATS_TYPE);
    }
    return NETMANAGER_SUCCESS;
}

int32_t NetsysBpfStats::GetIfaceStats(uint64_t &stats, const StatsType statsType, const std::string &interfaceName)
{
    NetsysBpfMap<uint32_t, IfaceName> ifaceNameMap(IFACE_INDEX_NAME_MAP_PATH, BPF_F_RDONLY);
    if (!ifaceNameMap.IsValid()) {
        return DumpError(STATS_ERR_INVALID_IFACE_NAME_MAP);
    }
    NetsysBpfMap<uint32_t, StatsValue> ifaceStatsMap(IFACE_STATS_MAP_PATH, BPF_F_RDONLY);
    if (!ifaceStatsMap.IsValid()) {
        return DumpError(STATS_ERR_INVALID_IFACE_STATS_MAP);
    }
    return BpfGetIfaceStats(stats, statsType, interfaceName, ifaceNameMap, ifaceStatsMap);
}

bool NetsysBpfStats::GetIfaceName(const NetsysBpfMap<uint32_t, IfaceName> &ifaceNameMap, uint32_t ifaceIndex,
                                  std::string &ifName)
{
    IfaceName ifaceName = ifaceNameMap.ReadValueFromMap(ifaceIndex);
    ifName = ifaceName.name;
    return ifName.empty();
}
} // namespace NetManagerStandard
} // namespace OHOS
