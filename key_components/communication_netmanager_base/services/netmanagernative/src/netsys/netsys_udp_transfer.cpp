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

#include "netsys_udp_transfer.h"

#include "netsys_client.h"

namespace OHOS {
namespace nmd {
namespace {
struct UdpBuffer {
    size_t size;
    int32_t sock;
    int16_t event;
    sockaddr_in addr;
};

int32_t ProcUdpData(UdpBuffer udpBuffer, char *data, socklen_t &lenAddr,
                    int64_t (*func)(int fd, char *buf, size_t len, sockaddr_in addr, socklen_t &lenAddr))
{
    char *curPos = data;
    size_t leftSize = udpBuffer.size;
    int64_t length = -1;
    int retry = 0;
    while (leftSize > 0) {
        int32_t resPoll = Poll(udpBuffer.sock, udpBuffer.event, &retry);
        if (resPoll < 0) {
            return -1;
        } else if (resPoll == 0) {
            continue;
        }

        length = func(udpBuffer.sock, curPos, leftSize, udpBuffer.addr, lenAddr);
        if (length <= 0) {
            if (errno == EAGAIN && retry < MAX_POLL_RETRY) {
                ++retry;
                continue;
            }
            return -1;
        }
        return length;
    }
    return leftSize;
}

int64_t SendUdpWrapper(int32_t fd, char *buf, size_t len, sockaddr_in addr, socklen_t &lenAddr)
{
    (void)lenAddr;
    return sendto(fd, buf, len, 0, (sockaddr *)&addr, sizeof(sockaddr_in));
}

int64_t RecvUdpWrapper(int32_t fd, char *buf, size_t len, sockaddr_in addr, socklen_t &lenAddr)
{
    return recvfrom(fd, buf, len, 0, (sockaddr *)&addr, &lenAddr);
}
}

bool PollUdpDataTransfer::MakeUdpNonBlock(int32_t sock)
{
    if (sock < 0) {
        return false;
    }
    return MakeNonBlock(sock);
}

int32_t PollUdpDataTransfer::PollUdpSendData(int32_t sock, char *data, size_t size, sockaddr_in addr,
                                             socklen_t &lenAddr)
{
    struct UdpBuffer udpBuffer;
    udpBuffer.size = size;
    udpBuffer.sock = sock;
    udpBuffer.event = POLLOUT;
    udpBuffer.addr = addr;
    return ProcUdpData(udpBuffer, data, lenAddr, SendUdpWrapper);
}

int32_t PollUdpDataTransfer::PollUdpRecvData(int32_t sock, char *data, size_t size, sockaddr_in addr,
                                             socklen_t &lenAddr)
{
    struct UdpBuffer udpBuffer;
    udpBuffer.size = size;
    udpBuffer.sock = sock;
    udpBuffer.event = POLLIN;
    udpBuffer.addr = addr;
    return ProcUdpData(udpBuffer, data, lenAddr, RecvUdpWrapper);
}
} // namespace nmd
} // namespace OHOS