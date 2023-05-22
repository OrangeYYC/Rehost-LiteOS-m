/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef NETMANAGER_EXT_CONSTANTS_H
#define NETMANAGER_EXT_CONSTANTS_H

#include <errors.h>

namespace OHOS {
namespace NetManagerStandard {
constexpr int NETMANAGER_EXT_ERROR = -1;

enum {
    NETMANAGER_EXT_COMMON = 0x00,
    EXTERNALVPN_MANAGER = 0x01,
    NETMANAGER_ETHERNET_MANAGER = 0x02,
    NETMANAGER_MDNS_MANAGER = 0x03,
};

// Error code for common
constexpr ErrCode COMMON_EXT_ERR_OFFSET = ErrCodeOffset(SUBSYS_COMMUNICATION, NETMANAGER_EXT_COMMON);
// Error code for netmanager ethernet
constexpr ErrCode ETHERNET_ERR_OFFSET = ErrCodeOffset(SUBSYS_COMMUNICATION, NETMANAGER_ETHERNET_MANAGER);
// Error code for netmanager external vpn
constexpr ErrCode EXTERNALVPN_ERR_OFFSET = ErrCodeOffset(SUBSYS_COMMUNICATION, EXTERNALVPN_MANAGER);

// Error code for netmanager mdns
constexpr ErrCode MDNS_ERR_OFFSET = ErrCodeOffset(SUBSYS_COMMUNICATION, NETMANAGER_MDNS_MANAGER);

// for network sharing
enum class SharingIfaceType {
    SHARING_WIFI = 0,
    SHARING_USB,
    SHARING_BLUETOOTH,
};
enum class SharingIfaceState {
    SHARING_NIC_SERVING = 1,
    SHARING_NIC_CAN_SERVER,
    SHARING_NIC_ERROR,
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETMANAGER_EXT_CONSTANTS_H
