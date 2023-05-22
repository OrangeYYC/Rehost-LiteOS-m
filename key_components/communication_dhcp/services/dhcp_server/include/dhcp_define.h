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

#ifndef OHOS_DHCP_DEFINE_H
#define OHOS_DHCP_DEFINE_H

#define DHCP_LEASE_TIME 21600
#define DHCP_RENEWAL_TIME 10800
#define DHCP_REBINDING_TIME 17280
#define DHCP_DEFAULT_TIMEOUT 30
#define DHCP_ADDRESS_LENGTH 4
#define DHCP_HWADDR_LENGTH 16
#define DHCP_HOST_NAME_LENGTH 64
#define DHCP_BOOT_FILE_LENGTH 128
#define DHCP_LEASE_FILE_LENGTH 256
#define DHCP_FILE_LINE_LENGTH 1024
#define DHCP_REFRESH_LEASE_FILE_INTERVAL 120
#define DHCP_MESSAGE_FLAG_LENGTH 16
#define DHCP_MAX_PATH_LENGTH 256
#define DHCP_ONE_BYTE_BITS 8

#define DHCP_ROOT "/data/service/el1/public/dhcp/"

#define DHCPD_CONFIG_FILE DHCP_ROOT"etc/dhcpd.conf"
#define DHCPD_EXT_CONFIG_PATH DHCP_ROOT"etc/dhcpd.d"
#define DHCPD_LEASE_FILE DHCP_ROOT"dhcpd_lease.lease"
#define DHCPD_PID_FILE DHCP_ROOT"dhcpd.pid"

#define DHCPD_VERSION "0.0.1"

#define DHCP_SERVER_PORT 67
#define DHCP_CLIENT_PORT 68

#define DHCP_OPTIONS_SIZE   312
#define IFACE_NAME_SIZE     50
#define PID_MAX_LEN         16

#define DHCP_RENEWAL_MULTIPLE 0.5
#define DHCP_REBIND_MULTIPLE 0.875

#define DHCP_TRUE 1
#define DHCP_FALSE 0
enum DhcpNumber {
    NUM_ZERO = 0,
    NUM_ONE,
    NUM_TWO,
    NUM_THREE,
    NUM_FOUR,
    NUM_FIVE,
    NUM_SIX,
    NUM_SEVEN,
    NUM_EIGHT,
    NUM_NINE,
    NUM_TEN,
    NUM_ELEVEN,
    NUM_TWELVE,
    NUM_THIRTEEN,
    NUM_FOURTEEN,
};

enum DhcpReturnCode {
    RET_SUCCESS = 0,
    RET_FAILED,
    RET_ERROR,
    ERR_SELECT,
    RET_BREAK,
    RET_WAIT_SAVE,
    RET_NO_SUPPORTED,
    RET_SELECT_TIME_OUT
};

#endif