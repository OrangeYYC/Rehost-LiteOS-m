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

#include "net_stats_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
NetStatsWrapper::NetStatsWrapper()
{
    netSysBpf_ = std::make_unique<NetsysBpfStats>();
}

NetStatsWrapper::~NetStatsWrapper() = default;

NetStatsWrapper &NetStatsWrapper::GetInstance()
{
    static NetStatsWrapper g_singleInstance_;
    return g_singleInstance_;
}

int32_t NetStatsWrapper::GetTotalStats(uint64_t &stats, StatsType type)
{
    return netSysBpf_->GetTotalStats(stats, type);
}

int32_t NetStatsWrapper::GetUidStats(uint64_t &stats, StatsType type, uint32_t uid)
{
    return netSysBpf_->GetUidStats(stats, type, uid);
}

int32_t NetStatsWrapper::GetIfaceStats(uint64_t &stats, StatsType type, const std::string &interfaceName)
{
    return netSysBpf_->GetIfaceStats(stats, type, interfaceName);
}

int32_t NetStatsWrapper::GetAllStatsInfo(std::vector<NetStatsInfo> &stats)
{
    return 0;
}
} // namespace NetManagerStandard
} // namespace OHOS
