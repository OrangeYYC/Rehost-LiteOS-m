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

#ifndef COMMUNICATION_NETMANAGER_BASE_NEW_DNS_NETSYS_CLIENT_H
#define COMMUNICATION_NETMANAGER_BASE_NEW_DNS_NETSYS_CLIENT_H

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DEFAULT_CONNECT_TIMEOUT 2
#define DEFAULT_POLL_TIMEOUT 1000 // 1 second
#define MAX_POLL_RETRY 3

#if NETSYS_CLIENT_DEBUG
#ifndef NETSYS_CLIENT_PRINT
#define NETSYS_CLIENT_PRINT(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#endif
#else
#define NETSYS_CLIENT_PRINT(fmt, ...)
#endif

bool MakeNonBlock(int sock);

int64_t SendWrapper(int fd, char *buf, size_t len);

int64_t RecvWrapper(int fd, char *buf, size_t len);

int64_t Poll(int sock, short event, int *retry);

bool ProcData(int sock, char *data, size_t size, short event, int64_t (*func)(int fd, char *buf, size_t len));

bool PollSendData(int sock, const char *data, size_t size);

bool PollRecvData(int sock, char *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif // COMMUNICATION_NETMANAGER_BASE_NEW_DNS_NETSYS_CLIENT_H
