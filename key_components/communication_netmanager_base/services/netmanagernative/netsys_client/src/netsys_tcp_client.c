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

#include "netsys_client.h"

#ifdef __cplusplus
extern "C" {
#endif

enum PollRec {
    POLL_FAILED = (-1),
    POLL_RETRY = 0,
    POLL_SUCCESS = 1,
};

bool MakeNonBlock(int sock)
{
    int flags = fcntl(sock, F_GETFL, 0);
    if (flags == -1 && errno == EINTR) {
        flags = fcntl(sock, F_GETFL, 0);
    }
    if (flags == -1) {
        NETSYS_CLIENT_PRINT("make non block failed %s", strerror(errno));
        return false;
    }
    uint32_t tempFlags = (uint32_t)flags | O_NONBLOCK;
    int ret = fcntl(sock, F_SETFL, tempFlags);
    if (ret == -1 && errno == EINTR) {
        ret = fcntl(sock, F_SETFL, tempFlags);
    }
    if (ret == -1) {
        NETSYS_CLIENT_PRINT("make non block failed %s", strerror(errno));
        return false;
    }
    return true;
}

int64_t SendWrapper(int fd, char *buf, size_t len)
{
    return send(fd, buf, len, 0);
}

int64_t RecvWrapper(int fd, char *buf, size_t len)
{
    return recv(fd, buf, len, 0);
}

int64_t Poll(int sock, short event, int *retry)
{
    nfds_t num = 1;
    struct pollfd fds[1] = {{0}};
    fds[0].fd = sock;
    fds[0].events = event;

    int ret = poll(fds, num, DEFAULT_POLL_TIMEOUT);
    if (ret == -1) {
        NETSYS_CLIENT_PRINT("poll to proc failed %s", strerror(errno));
        return POLL_RETRY;
    }
    if (ret == 0) {
        if (*retry < MAX_POLL_RETRY) {
            ++(*retry);
            return POLL_RETRY;
        }
        NETSYS_CLIENT_PRINT("poll to proc timeout");
        return POLL_FAILED;
    }
    return POLL_SUCCESS;
}

bool ProcData(int sock, char *data, size_t size, short event, int64_t (*func)(int fd, char *buf, size_t len))
{
    char *curPos = data;
    size_t leftSize = size;

    int retry = 0;
    while (leftSize > 0) {
        int32_t resPoll = Poll(sock, event, &retry);
        if (resPoll < 0) {
            return false;
        } else if (resPoll == 0) {
            continue;
        }

        int64_t length = func(sock, curPos, leftSize);
        if (length < 0) {
            if (errno == EAGAIN && retry < MAX_POLL_RETRY) {
                ++retry;
                continue;
            }
            NETSYS_CLIENT_PRINT("proc failed %s", strerror(errno));
            return false;
        }
        if (length == 0) {
            break;
        }
        curPos += length;
        leftSize -= length;
    }

    if (leftSize != 0) {
        NETSYS_CLIENT_PRINT("proc not complete");
        return false;
    }
    return true;
}

bool PollSendData(int sock, const char *data, size_t size)
{
    return ProcData(sock, (char *)data, size, POLLOUT, SendWrapper);
}

bool PollRecvData(int sock, char *data, size_t size)
{
    return ProcData(sock, data, size, POLLIN, RecvWrapper);
}
#ifdef __cplusplus
}
#endif
