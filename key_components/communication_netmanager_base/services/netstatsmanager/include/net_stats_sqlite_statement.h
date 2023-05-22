/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef NET_STATS_SQLITE_STATEMENT_H
#define NET_STATS_SQLITE_STATEMENT_H

#include <climits>
#include <functional>
#include <string>

#ifndef USE_SQLITE_SYMBOLS
#include "sqlite3.h"
#else
#include "sqlite3sym.h"
#endif

namespace OHOS {
namespace NetManagerStandard {
class NetStatsSqliteStatement {
public:
    NetStatsSqliteStatement() = default;
    ~NetStatsSqliteStatement();
    int32_t Prepare(sqlite3 *dbHandle, const std::string &sql);
    int32_t BindInt32(int32_t index, int32_t value) const;
    int32_t BindInt64(int32_t index, int64_t value) const;
    int32_t BindText(int32_t index, std::string value) const;
    void Finalize();
    void ResetStatementAndClearBindings() const;
    int32_t Step();
    int32_t GetColumnString(int32_t index, std::string &value) const;
    int32_t GetColumnLong(int32_t index, uint64_t &value) const;
    int32_t GetColumnInt(int32_t index, uint32_t &value) const;
    int32_t GetColumnCount() const
    {
        return columnCount_;
    }

private:
    std::string sqlCmd_;
    sqlite3_stmt *stmtHandle_ = nullptr;
    int32_t columnCount_ = 0;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif