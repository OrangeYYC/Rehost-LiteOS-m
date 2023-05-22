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

#ifndef INCLUDE_NETLINK_MSG_H
#define INCLUDE_NETLINK_MSG_H

#include <arpa/inet.h>
#include <asm/types.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <linux/fib_rules.h>
#include <linux/inet_diag.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

namespace OHOS {
namespace nmd {
constexpr uint32_t NETLINK_MAX_LEN = 1024;
class NetlinkMsg {
public:
    NetlinkMsg(uint16_t flags, size_t maxBufLen, int32_t pid);
    ~NetlinkMsg();
    /**
     * Add route message to nlmsghdr
     *
     * @param action Action name
     * @param msg Added message
     */
    void AddRoute(uint16_t action, struct rtmsg msg);

    /**
     * Add rule message to nlmsghdr
     *
     * @param action Action name
     * @param msg Added message
     */
    void AddRule(uint16_t action, struct fib_rule_hdr msg);

    /**
     * Add address message to nlmsghdr
     *
     * @param action Action name
     * @param msg Added message
     */
    void AddAddress(uint16_t action, struct ifaddrmsg msg);

    /**
     * Add rtattr to nlmsghdr
     *
     * @param rtaType Rta type
     * @param buf Rta data
     * @param bufLen Rta data length
     * @return Returns 0, add rtattr to nlmsghdr successfully, otherwise it will fail
     */
    int32_t AddAttr(uint16_t rtaType, void *data, size_t dataLen);

    /**
     * Add 16 bit rtattr to nlmsghdr
     *
     * @param rtaType Rta type
     * @param data Rta data
     * @return Returns 0, add 16 bit rtattr to nlmsghdr successfully, otherwise it will fail
     */
    int32_t AddAttr16(uint16_t rtaType, uint16_t data);

    /**
     * Add 32 bit rtattr to nlmsghdr for
     *
     * @param rtaType Rta type
     * @param data Rta data
     * @return Returns 0, add 32 bit rtattr to nlmsghdr successfully, otherwise it will fail
     */
    int32_t AddAttr32(uint16_t rtaType, uint32_t data);

    /**
     * Get the netlink message
     *
     * @return Netlink message struct
     */
    struct nlmsghdr *GetNetLinkMessage();

private:
    std::unique_ptr<char[]> msghdrBuf_;
    struct nlmsghdr *netlinkMessage_;
    size_t maxBufLen_;
};
} // namespace nmd
} // namespace OHOS
#endif // !INCLUDE_NETLINK_MSG_H
