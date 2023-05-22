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
#ifndef NET_CONN_CONSTANTS_H
#define NET_CONN_CONSTANTS_H

#include "net_manager_constants.h"

namespace OHOS {
namespace NetManagerStandard {
enum NetConnResultCode {
    NET_CONN_ERR_INVALID_SUPPLIER_ID = 2101002,
    NET_CONN_ERR_NET_TYPE_NOT_FOUND = 2101003,
    NET_CONN_ERR_NO_ANY_NET_TYPE = 2101004,
    NET_CONN_ERR_NO_REGISTERED = 2101005,
    NET_CONN_ERR_NETID_NOT_FOUND = 2101006,
    NET_CONN_ERR_SAME_CALLBACK = 2101007,
    NET_CONN_ERR_CALLBACK_NOT_FOUND = 2101008,
    NET_CONN_ERR_REQ_ID_NOT_FOUND = 2101009,
    NET_CONN_ERR_NO_DEFAULT_NET = 2101010,
    NET_CONN_ERR_HTTP_PROXY_INVALID = 2101011,
    NET_CONN_ERR_NO_HTTP_PROXY = 2101012,
    NET_CONN_ERR_INVALID_NETWORK = 2101013,
    NET_CONN_ERR_SERVICE_REQUEST_CONNECT_FAIL = 2101014,
    NET_CONN_ERR_SERVICE_UPDATE_NET_LINK_INFO_FAIL = 2101015,
    NET_CONN_ERR_NO_SUPPLIER = 2101016,
    NET_CONN_ERR_NET_MONITOR_OPT_FAILED = 2101017,
    NET_CONN_ERR_SERVICE_NO_REQUEST = 2101018,
    NET_CONN_ERR_NO_ADDRESS = 2101019,
    NET_CONN_ERR_NET_NOT_FIND_BESTNETWORK_FOR_REQUEST = 2101020,
    NET_CONN_ERR_NET_NO_RESTRICT_BACKGROUND = 2101021,
    NET_CONN_ERR_NET_OVER_MAX_REQUEST_NUM = 2101022,
    NET_CONN_ERR_CREATE_DUMP_FAILED = 2101023,
    NET_CONN_SUCCESS = NETMANAGER_SUCCESS,
    NET_CONN_ERR_INPUT_NULL_PTR = -2,
};

enum NetDetectionResultCode {
    NET_DETECTION_FAIL = 0,
    NET_DETECTION_SUCCESS,
    NET_DETECTION_CAPTIVE_PORTAL,
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_CONN_CONSTANTS_H