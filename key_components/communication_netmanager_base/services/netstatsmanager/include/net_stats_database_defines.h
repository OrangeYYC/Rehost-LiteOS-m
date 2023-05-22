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

#ifndef NET_STATS_DATABASE_DEFINES_H
#define NET_STATS_DATABASE_DEFINES_H

#include <map>

#include "netmanager_base_common_utils.h"
#include "net_stats_info.h"

namespace OHOS {
namespace NetManagerStandard {
namespace NetStatsDatabaseDefines {
constexpr const char *NET_STATS_DATABASE_PATH = "/data/service/el1/public/netmanager/net_stats_data.db";
constexpr const char *UID_TABLE_CREATE_PARAM =
    "UID INTEGER NOT NULL,"
    "IFace CHAR(50) NOT NULL,"
    "Date INTEGER NOT NULL,"
    "RxBytes INTEGER NOT NULL,"
    "RxPackets INTEGER NOT NULL,"
    "TxBytes INTEGER NOT NULL,"
    "TxPackets INTEGER NOT NULL";
constexpr const char *IFACE_TABLE_CREATE_PARAM =
    "IFace CHAR(50) NOT NULL,"
    "Date INTEGER NOT NULL,"
    "RxBytes INTEGER NOT NULL,"
    "RxPackets INTEGER NOT NULL,"
    "TxBytes INTEGER NOT NULL,"
    "TxPackets INTEGER NOT NULL";
constexpr const char *UID_TABLE_PARAM_LIST = "UID,IFace,Date,RxBytes,RxPackets,TxBytes,TxPackets";
constexpr const char *IFACE_TABLE_PARAM_LIST = "IFace,Date,RxBytes,RxPackets,TxBytes,TxPackets";
constexpr const char *IFACE_TABLE = "T_iface";
constexpr const char *UID_TABLE = "T_uid";

constexpr int32_t UID_PARAM_NUM = 7;

enum class DataType {
    UID,
    IFACE,
};
} // namespace NetStatsDatabaseDefines
} // namespace NetManagerStandard
} // namespace OHOS

#endif // NET_STATS_DATABASE_DEFINES_H
