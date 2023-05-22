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

#ifndef NET_STATS_DATABASE_HELPER_H
#define NET_STATS_DATABASE_HELPER_H

#include <climits>
#include <functional>
#include <string>

#ifndef USE_SQLITE_SYMBOLS
#include "sqlite3.h"
#else
#include "sqlite3sym.h"
#endif

#include "net_stats_sqlite_statement.h"
#include "net_stats_info.h"

namespace OHOS {
namespace NetManagerStandard {
class NetStatsDatabaseHelper {
public:
    using SqlCallback = sqlite3_callback;
    explicit NetStatsDatabaseHelper(const std::string &path);
    NetStatsDatabaseHelper() = delete;
    ~NetStatsDatabaseHelper();

    int32_t CreateTable(const std::string &tableName, const std::string &tableInfo);
    int32_t InsertData(const std::string &tableName, const std::string &paramList,
                       const NetStatsInfo &info);
    int32_t SelectData(std::vector<NetStatsInfo> &infos, const std::string &tableName, uint64_t start, uint64_t end);
    int32_t SelectData(const uint32_t uid, uint64_t start, uint64_t end, std::vector<NetStatsInfo> &infos);
    int32_t SelectData(const std::string &iface, uint64_t start, uint64_t end, std::vector<NetStatsInfo> &infos);
    int32_t SelectData(const std::string &iface, const uint32_t uid, uint64_t start, uint64_t end,
                       std::vector<NetStatsInfo> &infos);
    int32_t DeleteData(const std::string &tableName, uint64_t start, uint64_t end);
    int32_t DeleteData(const std::string &tableName, uint64_t uid);
    int32_t ClearData(const std::string &tableName);
    int32_t Step(std::vector<NetStatsInfo> &infos);
    int32_t ExecSql(const std::string &sql, void *recv, SqlCallback callback);

private:
    int32_t Open(const std::string &path);
    int32_t Close();
    sqlite3 *sqlite_ = nullptr;
    NetStatsSqliteStatement statement_;
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif // NET_STATS_DATABASE_HELPER_H
