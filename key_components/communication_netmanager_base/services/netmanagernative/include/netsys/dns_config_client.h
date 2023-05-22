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

#ifndef COMMUNICATION_NETMANAGER_BASE_DNS_CONFIG_CLIENT_H
#define COMMUNICATION_NETMANAGER_BASE_DNS_CONFIG_CLIENT_H

#include <arpa/inet.h>
#include <netdb.h>
#include <stdint.h>

#include "securec.h"

#ifdef __cplusplus
extern "C" {
#endif

#if DNS_CONFIG_DEBUG
#ifndef DNS_CONFIG_PRINT
#define DNS_CONFIG_PRINT(fmt, ...) printf("DNS" fmt "\n", ##__VA_ARGS__)
#endif
#else
#define DNS_CONFIG_PRINT(fmt, ...)
#endif

#define MAX_SERVER_NUM 4
#define MAX_SERVER_LENGTH 50
#define DNS_SOCKET_PATH "/dev/unix/socket/dnsproxyd"
#define DNS_SOCKET_NAME "dnsproxyd"
#define MAX_RESULTS 32
#define MAX_CANON_NAME 256
#define MAX_HOST_NAME_LEN 256
#define MAX_KEY_LENGTH (MAX_HOST_NAME_LEN + 1 + MAX_SERVER_LENGTH + 1 + sizeof(uint32_t) * 4 + 3 + 1)
#define DEFAULT_TIMEOUT 5000
#define DEFAULT_RETRY 2
#define DEFAULT_SERVER_LENTH 16
#define DEFAULT_SERVER_NAME 114

enum CommandType { GET_CONFIG = 1, GET_CACHE = 2, SET_CACHE = 3 };

struct RequestInfo {
    uint32_t command;
    uint32_t netId;
};

struct ResolvConfig {
    int32_t error;
    int32_t timeoutMs;
    uint32_t retryCount;
    char nameservers[MAX_SERVER_NUM][MAX_SERVER_LENGTH + 1];
};

typedef union {
    struct sockaddr sa;
    struct sockaddr_in6 sin6;
    struct sockaddr_in sin;
} AlignedSockAddr;

struct AddrInfo {
    uint32_t aiFlags;
    uint32_t aiFamily;
    uint32_t aiSockType;
    uint32_t aiProtocol;
    uint32_t aiAddrLen;
    AlignedSockAddr aiAddr;
    char aiCanonName[MAX_CANON_NAME + 1];
};

struct ParamWrapper {
    char *host;
    char *serv;
    struct addrinfo *hint;
};

#ifdef __cplusplus
}
#endif
#endif // COMMUNICATION_NETMANAGER_BASE_1_DNS_CONFIG_CLIENT_H
