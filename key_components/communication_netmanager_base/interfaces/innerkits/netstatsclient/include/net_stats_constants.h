/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef NET_STATS_CONSTANTS_H
#define NET_STATS_CONSTANTS_H

#include "net_manager_constants.h"

namespace OHOS {
namespace NetManagerStandard {
constexpr int16_t LIMIT_STATS_CALLBACK_NUM = 200;
enum NetStatsResultCode {
    STATS_DUMP_MESSAGE_FAIL = 2103002,
    STATS_REMOVE_FILE_FAIL,
    STATS_ERR_INVALID_TIME_PERIOD,
    STATS_ERR_READ_BPF_FAIL,
    STATS_ERR_INVALID_KEY,
    STATS_ERR_INVALID_IFACE_STATS_MAP,
    STATS_ERR_INVALID_STATS_VALUE,
    STATS_ERR_INVALID_STATS_TYPE,
    STATS_ERR_INVALID_UID_STATS_MAP,
    STATS_ERR_INVALID_IFACE_NAME_MAP,
    STATS_ERR_GET_IFACE_NAME_FAILED,
    STATS_ERR_CLEAR_STATS_DATA_FAIL,
    STATS_ERR_CREATE_TABLE_FAIL,
    STATS_ERR_DATABASE_RECV_NO_DATA,
    STATS_ERR_WRITE_DATA_FAIL,
    STATS_ERR_READ_DATA_FAIL,
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_STATS_CONSTANTS_H
