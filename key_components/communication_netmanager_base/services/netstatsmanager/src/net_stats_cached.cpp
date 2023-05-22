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

#include "net_stats_cached.h"

#include <initializer_list>
#include <list>

#include "net_stats_constants.h"
#include "net_stats_data_handler.h"
#include "net_stats_database_defines.h"
#include "net_stats_database_helper.h"
#include "net_stats_wrapper.h"
#include "netsys_controller.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace NetStatsDatabaseDefines;
namespace {
constexpr const char *IFACE_LO = "lo";
} // namespace

int32_t NetStatsCached::StartCached()
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    auto ret = helper->CreateTable(UID_TABLE, UID_TABLE_CREATE_PARAM);
    if (ret != NETMANAGER_SUCCESS) {
        NETMGR_LOG_E("Create uid table failed");
        return STATS_ERR_CREATE_TABLE_FAIL;
    }
    ret = helper->CreateTable(IFACE_TABLE, IFACE_TABLE_CREATE_PARAM);
    if (ret != 0) {
        NETMGR_LOG_E("Create iface table failed");
        return STATS_ERR_CREATE_TABLE_FAIL;
    }
    cacheTimer_ = std::make_unique<Timer>();
    writeTimer_ = std::make_unique<Timer>();
    cacheTimer_->Start(cycleThreshold_, [this]() { CacheStats(); });
    writeTimer_->Start(STATS_PACKET_CYCLE_MS, [this]() { WriteStats(); });
    return ret;
}

void NetStatsCached::GetUidStatsCached(std::vector<NetStatsInfo> &uidStatsInfo)
{
    uidStatsInfo.insert(uidStatsInfo.end(), stats_.GetUidStatsInfo().begin(), stats_.GetUidStatsInfo().end());
}

void NetStatsCached::GetIfaceStatsCached(std::vector<NetStatsInfo> &ifaceStatsInfo)
{
    ifaceStatsInfo.insert(ifaceStatsInfo.end(), stats_.GetIfaceStatsInfo().begin(), stats_.GetIfaceStatsInfo().end());
}

void NetStatsCached::CacheUidStats()
{
    std::vector<NetStatsInfo> statsInfos;
    NetStatsWrapper::GetInstance().GetAllStatsInfo(statsInfos);
    if (statsInfos.empty()) {
        NETMGR_LOG_W("No stats need to save");
        return;
    }

    std::for_each(statsInfos.begin(), statsInfos.end(), [this](NetStatsInfo &info) {
        if (info.iface_ == IFACE_LO) {
            return;
        }
        auto findRet = std::find_if(lastUidStatsInfo_.begin(), lastUidStatsInfo_.end(),
                                    [this, &info](const NetStatsInfo &lastInfo) { return info.Equals(lastInfo); });
        if (findRet == lastUidStatsInfo_.end()) {
            stats_.PushUidStats(info);
            return;
        }
        auto currentStats = info - *findRet;
        stats_.PushUidStats(currentStats);
    });
    lastUidStatsInfo_.clear();
    lastUidStatsInfo_ = std::move(statsInfos);
}

void NetStatsCached::CacheIfaceStats()
{
    std::vector<std::string> ifNameList = NetsysController::GetInstance().InterfaceGetList();
    std::for_each(ifNameList.begin(), ifNameList.end(), [this](const auto &ifName) {
        if (ifName == IFACE_LO) {
            return;
        }
        NetStatsInfo statsInfo;
        statsInfo.iface_ = ifName;
        NetStatsWrapper::GetInstance().GetIfaceStats(statsInfo.rxBytes_, StatsType::STATS_TYPE_RX_BYTES, ifName);
        NetStatsWrapper::GetInstance().GetIfaceStats(statsInfo.rxPackets_, StatsType::STATS_TYPE_RX_PACKETS, ifName);
        NetStatsWrapper::GetInstance().GetIfaceStats(statsInfo.txBytes_, StatsType::STATS_TYPE_TX_BYTES, ifName);
        NetStatsWrapper::GetInstance().GetIfaceStats(statsInfo.txPackets_, StatsType::STATS_TYPE_TX_PACKETS, ifName);
        auto findRet = lastIfaceStatsMap_.find(ifName);
        if (findRet == lastIfaceStatsMap_.end()) {
            stats_.PushIfaceStats(statsInfo);
            lastIfaceStatsMap_[ifName] = statsInfo;
            return;
        }
        auto currentStats = statsInfo - findRet->second;
        stats_.PushIfaceStats(currentStats);
        lastIfaceStatsMap_[ifName] = statsInfo;
    });
}

void NetStatsCached::CacheStats()
{
    std::unique_lock<std::mutex> lock(lock_);
    CacheUidStats();
    CacheIfaceStats();
}

void NetStatsCached::WriteStats()
{
    std::unique_lock<std::mutex> lock(lock_);
    WriteUidStats();
    WriteIfaceStats();
}
void NetStatsCached::WriteIfaceStats()
{
    if (!(CheckIfaceStor() || isForce_)) {
        return;
    }
    auto handler = std::make_unique<NetStatsDataHandler>();
    handler->WriteStatsData(stats_.GetIfaceStatsInfo(), NetStatsDatabaseDefines::IFACE_TABLE);
    handler->DeleteByDate(NetStatsDatabaseDefines::IFACE_TABLE, 0, CommonUtils::GetCurrentSecond() - dateCycle_);
    stats_.ResetIfaceStats();
}

void NetStatsCached::WriteUidStats()
{
    if (!(CheckUidStor() || isForce_)) {
        return;
    }
    auto handler = std::make_unique<NetStatsDataHandler>();
    handler->WriteStatsData(stats_.GetUidStatsInfo(), NetStatsDatabaseDefines::UID_TABLE);
    handler->DeleteByDate(NetStatsDatabaseDefines::UID_TABLE, 0, CommonUtils::GetCurrentSecond() - dateCycle_);
    stats_.ResetUidStats();
}

void NetStatsCached::SetCycleThreshold(uint32_t threshold)
{
    NETMGR_LOG_D("Current cycle threshold has changed current is : %{public}d", threshold);
    cycleThreshold_ = threshold;
    cacheTimer_ = std::make_unique<Timer>();
    cacheTimer_->Start(cycleThreshold_, [this]() { CacheStats(); });
}

void NetStatsCached::ForceUpdateStats()
{
    isForce_ = true;
    std::thread([this]() {
        CacheStats();
        WriteStats();
        isForce_ = false;
    }).detach();
}

void NetStatsCached::Reset() {}

} // namespace NetManagerStandard
} // namespace OHOS
