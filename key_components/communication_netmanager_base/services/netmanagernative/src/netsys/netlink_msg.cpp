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

#include "netlink_msg.h"

#include "netnative_log_wrapper.h"
#include "securec.h"

namespace OHOS {
namespace nmd {
NetlinkMsg::NetlinkMsg(uint16_t flags, size_t maxBufLen, int32_t pid)
{
    maxBufLen_ = maxBufLen;
    msghdrBuf_ = std::make_unique<char[]>(NLMSG_SPACE(maxBufLen));
    netlinkMessage_ = reinterpret_cast<struct nlmsghdr *>(msghdrBuf_.get());
    errno_t result = memset_s(netlinkMessage_, NLMSG_SPACE(maxBufLen), 0, NLMSG_SPACE(maxBufLen));
    if (result != 0) {
        NETNATIVE_LOGE("[NetlinkMessage]: memset result %{public}d", result);
        return;
    }
    netlinkMessage_->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK | flags;
    netlinkMessage_->nlmsg_pid = static_cast<uint32_t>(pid);
    netlinkMessage_->nlmsg_seq = 1;
}

NetlinkMsg::~NetlinkMsg() = default;

void NetlinkMsg::AddRoute(uint16_t action, struct rtmsg msg)
{
    netlinkMessage_->nlmsg_type = action;
    int32_t result = memcpy_s(NLMSG_DATA(netlinkMessage_), sizeof(struct rtmsg), &msg, sizeof(struct rtmsg));
    if (result != 0) {
        NETNATIVE_LOGE("[AddRoute]: string copy failed result %{public}d", result);
    }
    netlinkMessage_->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
}

void NetlinkMsg::AddRule(uint16_t action, struct fib_rule_hdr msg)
{
    netlinkMessage_->nlmsg_type = action;
    int32_t result =
        memcpy_s(NLMSG_DATA(netlinkMessage_), sizeof(struct fib_rule_hdr), &msg, sizeof(struct fib_rule_hdr));
    if (result != 0) {
        NETNATIVE_LOGE("[AddRule]: string copy failed result %{public}d", result);
    }
    netlinkMessage_->nlmsg_len = NLMSG_LENGTH(sizeof(struct fib_rule_hdr));
}

void NetlinkMsg::AddAddress(uint16_t action, struct ifaddrmsg msg)
{
    netlinkMessage_->nlmsg_type = action;
    int32_t result = memcpy_s(NLMSG_DATA(netlinkMessage_), sizeof(struct ifaddrmsg), &msg, sizeof(struct ifaddrmsg));
    if (result != 0) {
        NETNATIVE_LOGE("[AddAddress]: string copy failed result %{public}d", result);
        return;
    }
    netlinkMessage_->nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
}

int32_t NetlinkMsg::AddAttr(uint16_t type, void *data, size_t alen)
{
    if (alen == 0 || data == nullptr) {
        NETNATIVE_LOGE("[NetlinkMessage]: length data can not be 0 or attr data can not be null");
        return -1;
    }

    int32_t len = RTA_LENGTH(alen);
    if (NLMSG_ALIGN(netlinkMessage_->nlmsg_len) + RTA_ALIGN(len) > maxBufLen_) {
        NETNATIVE_LOGE("[NetlinkMessage]: attr length than max len: %{public}d", (int32_t)maxBufLen_);
        return -1;
    }

    struct rtattr *rta = (struct rtattr *)(((char *)netlinkMessage_) + NLMSG_ALIGN(netlinkMessage_->nlmsg_len));
    if (rta == nullptr) {
        NETNATIVE_LOGE("Pointer rta is nullptr");
        return -1;
    }
    rta->rta_type = type;
    rta->rta_len = static_cast<uint16_t>(len);

    if (data != nullptr) {
        int32_t result = memcpy_s(RTA_DATA(rta), alen, data, alen);
        if (result != 0) {
            NETNATIVE_LOGE("[get_addr_info]: string copy failed result %{public}d", result);
            return -1;
        }
    }

    netlinkMessage_->nlmsg_len = NLMSG_ALIGN(netlinkMessage_->nlmsg_len) + RTA_ALIGN(len);
    return 0;
}

int32_t NetlinkMsg::AddAttr16(uint16_t type, uint16_t data)
{
    return AddAttr(type, &data, sizeof(uint16_t));
}

int32_t NetlinkMsg::AddAttr32(uint16_t type, uint32_t data)
{
    return AddAttr(type, &data, sizeof(uint32_t));
}

nlmsghdr *NetlinkMsg::GetNetLinkMessage()
{
    return netlinkMessage_;
}
} // namespace nmd
} // namespace OHOS
