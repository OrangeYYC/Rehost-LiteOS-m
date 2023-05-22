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

#ifndef NET_STATS_CACHED_H
#define NET_STATS_CACHED_H

#include <map>
#include <mutex>
#include <vector>

#include "net_stats_callback.h"
#include "net_stats_info.h"
#include "netmanager_base_common_utils.h"

#include "timer.h"

namespace OHOS {
namespace NetManagerStandard {
class NetStatsCached {
public:
    NetStatsCached() = default;
    ~NetStatsCached() = default;
    void ForceUpdateStats();

    int32_t StartCached();

    void SetCycleThreshold(uint32_t threshold);

    void GetUidStatsCached(std::vector<NetStatsInfo> &uidStatsInfo);

    void GetIfaceStatsCached(std::vector<NetStatsInfo> &ifaceStatsInfo);

    inline void SetTrafficThreshold(uint64_t threshold)
    {
        trafficThreshold_ = threshold;
    }

    inline void SetDateThreshold(uint64_t threshold)
    {
        cycleThreshold_ = threshold;
    }

    inline void SetCallbackManager(const std::shared_ptr<NetStatsCallback> &callbackManager)
    {
        stats_.SetNotifier(callbackManager);
    }

    void Reset();

private:
    class CachedInfo {
    public:
        void PushUidStats(NetStatsInfo &info)
        {
            if (info.HasNoData()) {
                return;
            }
            info.date_ = CommonUtils::GetCurrentSecond();
            uidStatsInfo_.push_back(info);
            currentUidStats_ += info.GetStats();
            if (netStatsCallbackManager_ != nullptr) {
                netStatsCallbackManager_->NotifyNetUidStatsChanged(info.iface_, info.uid_);
            }
        }

        void PushIfaceStats(NetStatsInfo &info)
        {
            if (info.HasNoData()) {
                return;
            }
            info.date_ = CommonUtils::GetCurrentSecond();
            ifaceStatsInfo_.push_back(info);
            currentIfaceStats_ += info.GetStats();
            if (netStatsCallbackManager_ != nullptr) {
                netStatsCallbackManager_->NotifyNetIfaceStatsChanged(info.iface_);
            }
        }

        inline std::vector<NetStatsInfo> &GetUidStatsInfo()
        {
            return uidStatsInfo_;
        }

        inline std::vector<NetStatsInfo> &GetIfaceStatsInfo()
        {
            return ifaceStatsInfo_;
        }

        inline uint64_t GetCurrentUidStats() const
        {
            return currentUidStats_;
        }

        inline uint64_t GetCurrentIfaceStats() const
        {
            return currentIfaceStats_;
        }

        void ResetUidStats()
        {
            uidStatsInfo_.clear();
            currentUidStats_ = 0;
        }

        void ResetIfaceStats()
        {
            ifaceStatsInfo_.clear();
            currentIfaceStats_ = 0;
        }

        inline void SetNotifier(const std::shared_ptr<NetStatsCallback> &callbackManager)
        {
            netStatsCallbackManager_ = callbackManager;
        }

    private:
        uint64_t currentUidStats_ = 0;
        uint64_t currentIfaceStats_ = 0;
        std::vector<NetStatsInfo> uidStatsInfo_;
        std::vector<NetStatsInfo> ifaceStatsInfo_;
        std::shared_ptr<NetStatsCallback> netStatsCallbackManager_ = nullptr;
    };

    static constexpr uint32_t DEFAULT_CACHE_CYCLE_MS = 30 * 60 * 1000;
    static constexpr uint64_t DEFAULT_TRAFFIC_STATISTICS_THRESHOLD_BYTES = 2 * 1024 * 1024;
    static constexpr uint64_t DEFAULT_DATA_CYCLE_S = 180 * 24 * 60 * 60;
    static constexpr uint64_t CACHE_DATE_TIME_S = 1 * 24 * 60 * 60;
    static constexpr uint64_t STATS_PACKET_CYCLE_MS = 1 * 60 * 60 * 1000;

    CachedInfo stats_;
    std::mutex lock_;
    bool isForce_ = false;
    std::unique_ptr<Timer> cacheTimer_ = nullptr;
    std::unique_ptr<Timer> writeTimer_ = nullptr;
    uint32_t cycleThreshold_ = DEFAULT_CACHE_CYCLE_MS;
    uint64_t trafficThreshold_ = DEFAULT_TRAFFIC_STATISTICS_THRESHOLD_BYTES;
    uint64_t dateCycle_ = DEFAULT_DATA_CYCLE_S;
    std::vector<NetStatsInfo> lastUidStatsInfo_;
    std::map<std::string, NetStatsInfo> lastIfaceStatsMap_;

    void CacheStats();
    void CacheUidStats();
    void CacheIfaceStats();

    void WriteStats();
    void WriteUidStats();
    void WriteIfaceStats();

    inline bool CheckUidStor()
    {
        return stats_.GetCurrentUidStats() >= trafficThreshold_;
    }

    inline bool CheckIfaceStor()
    {
        return stats_.GetCurrentIfaceStats() >= trafficThreshold_;
    }
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif // NET_STATS_CACHED_H
