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

#ifndef NET_STATS_DATA_HANDLER_H
#define NET_STATS_DATA_HANDLER_H

#include "net_stats_info.h"
#include "net_stats_database_defines.h"

namespace OHOS {
namespace NetManagerStandard {

class NetStatsDataHandler {
public:
    NetStatsDataHandler() = default;
    ~NetStatsDataHandler() = default;
    int32_t WriteStatsData(const std::vector<NetStatsInfo> &infos, const std::string &tableName);
    int32_t ReadStatsData(std::vector<NetStatsInfo> &infos, uint64_t start, uint64_t end);
    int32_t ReadStatsData(std::vector<NetStatsInfo> &infos, uint64_t uid, uint64_t start, uint64_t end);
    int32_t ReadStatsData(std::vector<NetStatsInfo> &infos, const std::string &iface, uint64_t start, uint64_t end);
    int32_t ReadStatsData(std::vector<NetStatsInfo> &infos, const std::string &iface, const uint32_t uid,
                          uint64_t start, uint64_t end);
    int32_t DeleteByUid(uint64_t uid);
    int32_t DeleteByDate(const std::string &tableName, uint64_t start, uint64_t end);
    int32_t ClearData();
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_STATS_DATA_HANDLER_H
