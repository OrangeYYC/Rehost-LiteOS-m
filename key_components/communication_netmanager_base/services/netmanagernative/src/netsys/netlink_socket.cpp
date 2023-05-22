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

#include <arpa/inet.h>
#include <asm/types.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <linux/fib_rules.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include "netnative_log_wrapper.h"
#include "securec.h"

#include "netlink_socket.h"
namespace OHOS {
namespace nmd {
int32_t SendNetlinkMsgToKernel(struct nlmsghdr *msg, uint32_t table)
{
    if (msg == nullptr) {
        NETNATIVE_LOGE("[NetlinkSocket] msg can not be null ");
        return -1;
    }
    int32_t kernelSocket = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (kernelSocket == -1) {
        NETNATIVE_LOGE("[NetlinkSocket] create socket failed: %{public}d", errno);
        return -1;
    }
    struct iovec ioVector;
    ioVector.iov_base = msg;
    ioVector.iov_len = msg->nlmsg_len;

    struct msghdr msgHeader;
    (void)memset_s(&msgHeader, sizeof(msgHeader), 0, sizeof(msgHeader));

    struct sockaddr_nl kernel;
    (void)memset_s(&kernel, sizeof(kernel), 0, sizeof(kernel));
    kernel.nl_family = AF_NETLINK;
    kernel.nl_groups = 0;

    msgHeader.msg_name = &kernel;
    msgHeader.msg_namelen = sizeof(kernel);
    msgHeader.msg_iov = &ioVector;
    msgHeader.msg_iovlen = 1;

    ssize_t msgState = sendmsg(kernelSocket, &msgHeader, 0);
    if (msgState == -1) {
        NETNATIVE_LOGE("[NetlinkSocket] msg can not be null ");
        close(kernelSocket);
        return -1;
    } else if (msgState == 0) {
        NETNATIVE_LOGE("[NetlinkSocket] 0 bytes send.");
        close(kernelSocket);
        return -1;
    }
    NETNATIVE_LOG_D("[NetlinkSocket] msgState is %{public}zd", msgState);
    if (msg->nlmsg_flags & NLM_F_DUMP) {
        msgState = GetInfoFromKernel(kernelSocket, msg->nlmsg_type, table);
    }
    close(kernelSocket);
    return msgState;
}

int32_t ClearRouteInfo(uint16_t clearThing, uint32_t table)
{
    if (clearThing != RTM_GETROUTE && clearThing != RTM_GETRULE) {
        NETNATIVE_LOGE("ClearRouteInfo %{public}d type error", clearThing);
        return -1;
    }
    // Request the kernel to send a list of all routes or rules.
    std::unique_ptr<char[]> msghdrBuf = std::make_unique<char[]>(NLMSG_SPACE(NETLINKMESSAGE_MAX_LEN));
    struct nlmsghdr *msghdr = reinterpret_cast<struct nlmsghdr *>(msghdrBuf.get());
    errno_t result = memset_s(msghdr, NLMSG_SPACE(NETLINKMESSAGE_MAX_LEN), 0, NLMSG_SPACE(NETLINKMESSAGE_MAX_LEN));
    if (result != 0) {
        NETNATIVE_LOGE("[NetlinkMessage]: memset result %{public}d", result);
    }
    rtmsg msg;
    msg.rtm_family = AF_INET;
    int32_t copeResult = memcpy_s(NLMSG_DATA(msghdr), sizeof(struct rtmsg), &msg, sizeof(struct rtmsg));
    if (copeResult != 0) {
        NETNATIVE_LOGE("[AddRoute]: string copy failed result %{public}d", copeResult);
    }
    msghdr->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    msghdr->nlmsg_type = clearThing;
    msghdr->nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
    return SendNetlinkMsgToKernel(msghdr);
}

int32_t GetInfoFromKernel(int32_t sock, uint16_t clearThing, uint32_t table)
{
    char readBuffer[KERNEL_BUFFER_SIZE] = {0};
    // Read the information returned by the kernel through the socket.
    ssize_t readedInfos = read(sock, readBuffer, sizeof(readBuffer));
    if (readedInfos < 0) {
        return -errno;
    }
    while (readedInfos > 0) {
        uint32_t readLength = readedInfos;
        // Traverse and read the information returned by the kernel for item by item processing.
        for (nlmsghdr *nlmsgHeader = reinterpret_cast<nlmsghdr *>(readBuffer); NLMSG_OK(nlmsgHeader, readLength);
             nlmsgHeader = NLMSG_NEXT(nlmsgHeader, readLength)) {
            if (nlmsgHeader->nlmsg_type == NLMSG_ERROR) {
                nlmsgerr *err = reinterpret_cast<nlmsgerr *>(NLMSG_DATA(nlmsgHeader));
                NETNATIVE_LOGE("netlink read socket failed error = %{public}d", err->error);
                return err->error;
            } else if (nlmsgHeader->nlmsg_type == NLMSG_DONE) {
                return 0;
            } else {
                DealInfoFromKernel(nlmsgHeader, clearThing, table);
            }
        }
        readedInfos = read(sock, readBuffer, sizeof(readBuffer));
        if (readedInfos < 0) {
            return -errno;
        }
    }
    return 0;
}

void DealInfoFromKernel(nlmsghdr *nlmsgHeader, uint16_t clearThing, uint32_t table)
{
    if (nlmsgHeader == nullptr) {
        NETNATIVE_LOGE("nlmsgHeader is nullptr");
        return;
    }
    struct nlmsghdr *msg = nlmsgHeader;
    msg->nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK;
    if (clearThing == RTM_GETRULE) {
        msg->nlmsg_type = RTM_DELRULE;
        if (GetRouteProperty(nlmsgHeader, FRA_PRIORITY) != LOCAL_PRIORITY) {
            return;
        }
    } else if (clearThing == RTM_GETROUTE) {
        msg->nlmsg_type = RTM_DELROUTE;
        if (GetRouteProperty(nlmsgHeader, RTA_TABLE) != table) {
            return;
        }
    }
    SendNetlinkMsgToKernel(msg);
}

uint32_t GetRouteProperty(const nlmsghdr *nlmsgHeader, int32_t property)
{
    if (nlmsgHeader == nullptr) {
        NETNATIVE_LOGE("nlmsgHeader is nullptr");
        return -1;
    }
    uint32_t rtaLength = RTM_PAYLOAD(nlmsgHeader);
    rtmsg *infoMsg = reinterpret_cast<rtmsg *>(NLMSG_DATA(nlmsgHeader));
    for (rtattr *infoRta = reinterpret_cast<rtattr *> RTM_RTA(infoMsg); RTA_OK(infoRta, rtaLength);
         infoRta = RTA_NEXT(infoRta, rtaLength)) {
        if (infoRta->rta_type == property) {
            return *(reinterpret_cast<uint32_t *>(RTA_DATA(infoRta)));
        }
    }
    return 0;
}
} // namespace nmd
} // namespace OHOS
