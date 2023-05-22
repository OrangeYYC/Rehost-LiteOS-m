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

#include "net_stats_data_handler.h"

#include "net_mgr_log_wrapper.h"
#include "net_stats_database_defines.h"
#include "net_stats_database_helper.h"
#include "net_stats_constants.h"
#include "net_manager_constants.h"
#include "netmanager_base_common_utils.h"

namespace OHOS {
namespace NetManagerStandard {
using namespace NetStatsDatabaseDefines;

int32_t NetStatsDataHandler::ReadStatsData(std::vector<NetStatsInfo> &infos, uint64_t start, uint64_t end)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    return helper->SelectData(infos, UID_TABLE, start, end);
}

int32_t NetStatsDataHandler::ReadStatsData(std::vector<NetStatsInfo> &infos, uint64_t uid, uint64_t start, uint64_t end)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    return helper->SelectData(uid, start, end, infos);
}

int32_t NetStatsDataHandler::ReadStatsData(std::vector<NetStatsInfo> &infos, const std::string &iface, uint64_t start,
                                           uint64_t end)
{
    if (iface.empty()) {
        NETMGR_LOG_E("Param is invalid");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    return helper->SelectData(iface, start, end, infos);
}

int32_t NetStatsDataHandler::ReadStatsData(std::vector<NetStatsInfo> &infos, const std::string &iface,
                                           const uint32_t uid, uint64_t start, uint64_t end)
{
    if (iface.empty()) {
        NETMGR_LOG_E("Param is invalid");
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    return helper->SelectData(iface, uid, start, end, infos);
}

int32_t NetStatsDataHandler::WriteStatsData(const std::vector<NetStatsInfo> &infos, const std::string &tableName)
{
    NETMGR_LOG_I("WriteStatsData enter tableName:%{public}s", tableName.c_str());
    if (infos.empty() || tableName.empty()) {
        NETMGR_LOG_E("Param wrong, info: %{public}zu, tableName: %{public}zu", infos.size(), tableName.size());
        return NETMANAGER_ERR_PARAMETER_ERROR;
    }
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    if (tableName == UID_TABLE) {
        std::for_each(infos.begin(), infos.end(),
                      [&helper](const auto &info) { helper->InsertData(UID_TABLE, UID_TABLE_PARAM_LIST, info); });
        return NETMANAGER_SUCCESS;
    }
    if (tableName == IFACE_TABLE) {
        std::for_each(infos.begin(), infos.end(),
                      [&helper](const auto &info) { helper->InsertData(IFACE_TABLE, IFACE_TABLE_PARAM_LIST, info); });
        return NETMANAGER_SUCCESS;
    }
    return NETMANAGER_ERR_PARAMETER_ERROR;
}

int32_t NetStatsDataHandler::DeleteByUid(uint64_t uid)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    return helper->DeleteData(UID_TABLE, uid);
}

int32_t NetStatsDataHandler::DeleteByDate(const std::string &tableName, uint64_t start, uint64_t end)
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    return helper->DeleteData(tableName, start, end);
}

int32_t NetStatsDataHandler::ClearData()
{
    auto helper = std::make_unique<NetStatsDatabaseHelper>(NET_STATS_DATABASE_PATH);
    int32_t ifaceDataRet = helper->ClearData(IFACE_TABLE);
    int32_t uidDataRet = helper->ClearData(UID_TABLE);
    if (ifaceDataRet != NETMANAGER_SUCCESS || uidDataRet != NETMANAGER_SUCCESS) {
        return NETMANAGER_ERROR;
    }
    return NETMANAGER_SUCCESS;
}
} // namespace NetManagerStandard
} // namespace OHOS
