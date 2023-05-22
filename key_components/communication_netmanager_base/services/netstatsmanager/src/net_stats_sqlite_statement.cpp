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

#include "net_stats_sqlite_statement.h"

#include "net_manager_constants.h"
#include "net_mgr_log_wrapper.h"
#include "net_stats_constants.h"

namespace OHOS {
namespace NetManagerStandard {

NetStatsSqliteStatement::~NetStatsSqliteStatement()
{
    Finalize();
}

int32_t NetStatsSqliteStatement::Prepare(sqlite3 *dbHandle, const std::string &newSql)
{
    if (sqlCmd_.compare(newSql) == 0) {
        NETMGR_LOG_I("%{public}s is already prepared", newSql.c_str());
        return SQLITE_OK;
    }
    sqlite3_stmt *stmt = nullptr;
    int32_t errCode = sqlite3_prepare_v2(dbHandle, newSql.c_str(), newSql.length(), &stmt, nullptr);
    if (errCode != SQLITE_OK) {
        NETMGR_LOG_E("Prepare failed err = %{public}d", errCode);
        if (stmt != nullptr) {
            sqlite3_finalize(stmt);
        }
        return errCode;
    }
    Finalize(); // finalize the old
    sqlCmd_ = newSql;
    stmtHandle_ = stmt;
    columnCount_ = sqlite3_column_count(stmtHandle_);
    return SQLITE_OK;
}

void NetStatsSqliteStatement::Finalize()
{
    if (stmtHandle_ == nullptr) {
        return;
    }

    sqlite3_finalize(stmtHandle_);
    stmtHandle_ = nullptr;
    sqlCmd_ = "";
    columnCount_ = 0;
}

int32_t NetStatsSqliteStatement::BindInt32(int32_t index, int32_t value) const
{
    return sqlite3_bind_int(stmtHandle_, index, value);
}

int32_t NetStatsSqliteStatement::BindInt64(int32_t index, int64_t value) const
{
    return sqlite3_bind_int64(stmtHandle_, index, value);
}

int32_t NetStatsSqliteStatement::BindText(int32_t index, std::string value) const
{
    return sqlite3_bind_text(stmtHandle_, index, value.c_str(), -1, SQLITE_STATIC);
}

void NetStatsSqliteStatement::ResetStatementAndClearBindings() const
{
    if (stmtHandle_ == nullptr) {
        return;
    }

    int32_t errCode = sqlite3_reset(stmtHandle_);
    if (sqlite3_reset(stmtHandle_) != SQLITE_OK) {
        NETMGR_LOG_E("Reset statement failed. %{public}d", errCode);
        return;
    }

    errCode = sqlite3_clear_bindings(stmtHandle_);
    if (errCode != SQLITE_OK) {
        NETMGR_LOG_E("Reset clear bindings failed. %{public}d", errCode);
    }
}

int32_t NetStatsSqliteStatement::Step()
{
    return sqlite3_step(stmtHandle_);
}

int32_t NetStatsSqliteStatement::GetColumnString(int32_t index, std::string &value) const
{
    if (stmtHandle_ == nullptr || index >= columnCount_) {
        NETMGR_LOG_E("Get column string failed");
        return SQLITE_ERROR;
    }

    int32_t type = sqlite3_column_type(stmtHandle_, index);
    if (type != SQLITE_TEXT) {
        NETMGR_LOG_E("Get column string failed type is not text");
        return SQLITE_ERROR;
    }
    auto val = reinterpret_cast<const char *>(sqlite3_column_text(stmtHandle_, index));
    value = (val == nullptr) ? "" : std::string(val, sqlite3_column_bytes(stmtHandle_, index));
    return val == nullptr ? SQLITE_ERROR : SQLITE_OK;
}

int32_t NetStatsSqliteStatement::GetColumnLong(int32_t index, uint64_t &value) const
{
    if (stmtHandle_ == nullptr || index >= columnCount_) {
        NETMGR_LOG_E("Get column long failed");
        return SQLITE_ERROR;
    }
    int32_t type = sqlite3_column_type(stmtHandle_, index);
    if (type != SQLITE_INTEGER) {
        NETMGR_LOG_E("Get column long failed type is not interger");
        return SQLITE_ERROR;
    }
    value = static_cast<uint64_t>(sqlite3_column_int64(stmtHandle_, index));
    return SQLITE_OK;
}

int32_t NetStatsSqliteStatement::GetColumnInt(int32_t index, uint32_t &value) const
{
    if (stmtHandle_ == nullptr || index >= columnCount_) {
        NETMGR_LOG_E("Get column int failed");
        return SQLITE_ERROR;
    }
    int32_t type = sqlite3_column_type(stmtHandle_, index);
    if (type != SQLITE_INTEGER) {
        NETMGR_LOG_E("Get column int failed type is not interger");
        return SQLITE_ERROR;
    }
    value = static_cast<uint64_t>(sqlite3_column_int(stmtHandle_, index));
    return SQLITE_OK;
}
} // namespace NetManagerStandard
} // namespace OHOS
