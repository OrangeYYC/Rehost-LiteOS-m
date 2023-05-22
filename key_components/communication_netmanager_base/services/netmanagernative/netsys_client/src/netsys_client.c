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

#include <errno.h>
#include <sys/socket.h>
#include <sys/time.h>

#include "dns_config_client.h"
#include "netdb.h"
#include "securec.h"
#include "stdbool.h"
#include "sys/select.h"
#include "sys/un.h"
#include "unistd.h"

#ifdef __cplusplus
extern "C" {
#endif

static volatile uint8_t g_allowInternet = 1;

void DisallowInternet(void)
{
    g_allowInternet = 0;
}

uint8_t IsAllowInternet(void)
{
    return g_allowInternet;
}

static inline uint32_t Min(uint32_t a, uint32_t b)
{
    return a < b ? a : b;
}

static inline int CloseSocketReturn(int sock, int ret)
{
    close(sock);
    return ret;
}

void MakeDefaultDnsServer(char *server, size_t length)
{
    int ret = memset_s(server, length, 0, DEFAULT_SERVER_LENTH);
    if (ret < 0) {
        DNS_CONFIG_PRINT("MakeDefaultDnsServer memset_s failed");
        return;
    }

    ret = sprintf_s(server, length, "%d.%d.%d.%d", DEFAULT_SERVER_NAME, DEFAULT_SERVER_NAME, DEFAULT_SERVER_NAME,
                    DEFAULT_SERVER_NAME);
    if (ret != 0) {
        DNS_CONFIG_PRINT("MakeDefaultDnsServer sprintf_s failed");
    }
}

static bool NonBlockConnect(int sock, struct sockaddr *addr, socklen_t addrLen)
{
    int ret = connect(sock, addr, addrLen);
    if (ret >= 0) {
        return true;
    }
    if (errno != EINPROGRESS) {
        return false;
    }

    fd_set set = {0};
    FD_ZERO(&set);
    FD_SET(sock, &set);
    struct timeval timeout = {
        .tv_sec = DEFAULT_CONNECT_TIMEOUT,
        .tv_usec = 0,
    };

    ret = select(sock + 1, NULL, &set, NULL, &timeout);
    if (ret < 0) {
        DNS_CONFIG_PRINT("select error: %s", strerror(errno));
        return false;
    } else if (ret == 0) {
        DNS_CONFIG_PRINT("timeout!");
        return false;
    }

    int err = 0;
    socklen_t optLen = sizeof(err);
    ret = getsockopt(sock, SOL_SOCKET, SO_ERROR, (void *)(&err), &optLen);
    if (ret < 0 || err != 0) {
        return false;
    }
    return true;
}

static int CreateConnectionToNetSys(void)
{
    int32_t sockFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sockFd < 0) {
        DNS_CONFIG_PRINT("socket failed %d", errno);
        return -errno;
    }
    if (!MakeNonBlock(sockFd)) {
        DNS_CONFIG_PRINT("MakeNonBlock failed");
        return CloseSocketReturn(sockFd, -errno);
    }

    struct sockaddr_un address = {0};
    address.sun_family = AF_UNIX;

    if (strcpy_s(address.sun_path, sizeof(address.sun_path), DNS_SOCKET_PATH) != 0) {
        DNS_CONFIG_PRINT("str copy failed ");
        return CloseSocketReturn(sockFd, -1);
    }

    if (!NonBlockConnect(sockFd, (struct sockaddr *)&address, sizeof(address))) {
        return CloseSocketReturn(sockFd, -errno);
    }

    return sockFd;
}

static bool MakeKey(const char *hostName, const char *serv, const struct addrinfo *hints,
                    char key[static MAX_KEY_LENGTH])
{
    if (serv && hints) {
        return sprintf_s(key, MAX_KEY_LENGTH, "%s %s %d %d %d %d", hostName, serv, hints->ai_family, hints->ai_flags,
                         hints->ai_protocol, hints->ai_socktype) > 0;
    }

    if (hints) {
        return sprintf_s(key, MAX_KEY_LENGTH, "%s %d %d %d %d", hostName, hints->ai_family, hints->ai_flags,
                         hints->ai_protocol, hints->ai_socktype) > 0;
    }

    if (serv) {
        return sprintf_s(key, MAX_KEY_LENGTH, "%s %s", hostName, serv) > 0;
    }

    return sprintf_s(key, MAX_KEY_LENGTH, "%s", hostName) > 0;
}

static int32_t NetSysGetResolvConfInternal(int sockFd, uint16_t netId, struct ResolvConfig *config)
{
    struct RequestInfo info = {
        .command = GET_CONFIG,
        .netId = netId,
    };

    DNS_CONFIG_PRINT("NetSysGetResolvConfInternal begin netid: %d", info.netId);
    if (!PollSendData(sockFd, (const char *)(&info), sizeof(info))) {
        DNS_CONFIG_PRINT("send failed %d", errno);
        return CloseSocketReturn(sockFd, -errno);
    }

    if (!PollRecvData(sockFd, (char *)(config), sizeof(struct ResolvConfig))) {
        DNS_CONFIG_PRINT("receive failed %d", errno);
        return CloseSocketReturn(sockFd, -errno);
    }

    if (config->error < 0) {
        DNS_CONFIG_PRINT("get Config error: %d", config->error);
        return CloseSocketReturn(sockFd, config->error);
    }

    DNS_CONFIG_PRINT("NetSysGetResolvConfInternal end netid: %d", info.netId);
    return CloseSocketReturn(sockFd, 0);
}

int32_t NetSysGetResolvConf(uint16_t netId, struct ResolvConfig *config)
{
    if (config == NULL) {
        DNS_CONFIG_PRINT("Invalid Param");
        return -EINVAL;
    }

    int sockFd = CreateConnectionToNetSys();
    if (sockFd < 0) {
        DNS_CONFIG_PRINT("NetSysGetResolvConf CreateConnectionToNetSys connect to netsys err: %d", errno);
        return -errno;
    }

    int32_t err = NetSysGetResolvConfInternal(sockFd, netId, config);
    if (err < 0) {
        DNS_CONFIG_PRINT("NetSysGetResolvConf NetSysGetResolvConfInternal err: %d", errno);
        return err;
    }

    if (strlen(config->nameservers[0]) == 0) {
        return -1;
    }
    return 0;
}

static int32_t NetsysSendKeyForCache(int sockFd, struct ParamWrapper param, struct RequestInfo info)
{
    char key[MAX_KEY_LENGTH] = {0};
    if (!MakeKey(param.host, param.serv, param.hint, key)) {
        return CloseSocketReturn(sockFd, -1);
    }

    DNS_CONFIG_PRINT("NetSysSetResolvCacheInternal begin netid: %d", info.netId);
    if (!PollSendData(sockFd, (const char *)(&info), sizeof(info))) {
        DNS_CONFIG_PRINT("send failed %d", errno);
        return CloseSocketReturn(sockFd, -errno);
    }

    uint32_t nameLen = strlen(key) + 1;
    if (!PollSendData(sockFd, (const char *)&nameLen, sizeof(nameLen))) {
        DNS_CONFIG_PRINT("send failed %d", errno);
        return CloseSocketReturn(sockFd, -errno);
    }

    if (!PollSendData(sockFd, key, nameLen)) {
        DNS_CONFIG_PRINT("send failed %d", errno);
        return CloseSocketReturn(sockFd, -errno);
    }
    return 0;
};

static int32_t NetSysGetResolvCacheInternal(int sockFd, uint16_t netId, const struct ParamWrapper param,
                                            struct AddrInfo addrInfo[static MAX_RESULTS], uint32_t *num)
{
    struct RequestInfo info = {
        .command = GET_CACHE,
        .netId = netId,
    };

    int32_t res = NetsysSendKeyForCache(sockFd, param, info);
    if (res < 0) {
        return res;
    }

    if (!PollRecvData(sockFd, (char *)num, sizeof(uint32_t))) {
        DNS_CONFIG_PRINT("read failed %d", errno);
        return CloseSocketReturn(sockFd, -errno);
    }

    *num = Min(*num, MAX_RESULTS);
    if (*num == 0) {
        return CloseSocketReturn(sockFd, 0);
    }

    if (!PollRecvData(sockFd, (char *)addrInfo, sizeof(struct AddrInfo) * (*num))) {
        DNS_CONFIG_PRINT("read failed %d", errno);
        return CloseSocketReturn(sockFd, -errno);
    }

    DNS_CONFIG_PRINT("NetSysGetResolvCacheInternal end netid: %d", info.netId);
    return CloseSocketReturn(sockFd, 0);
}

int32_t NetSysGetResolvCache(uint16_t netId, const struct ParamWrapper param,
                             struct AddrInfo addrInfo[static MAX_RESULTS], uint32_t *num)
{
    char *hostName = param.host;
    if (hostName == NULL || strlen(hostName) == 0 || num == NULL) {
        DNS_CONFIG_PRINT("Invalid Param");
        return -EINVAL;
    }

    int sockFd = CreateConnectionToNetSys();
    if (sockFd < 0) {
        DNS_CONFIG_PRINT("NetSysGetResolvCache CreateConnectionToNetSys connect to netsys err: %d", errno);
        return sockFd;
    }

    int err = NetSysGetResolvCacheInternal(sockFd, netId, param, addrInfo, num);
    if (err < 0) {
        DNS_CONFIG_PRINT("NetSysGetResolvCache NetSysGetResolvCacheInternal err: %d", errno);
        return err;
    }

    return 0;
}

static int32_t FillAddrInfo(struct AddrInfo addrInfo[static MAX_RESULTS], struct addrinfo *res)
{
    if (memset_s(addrInfo, sizeof(struct AddrInfo) * MAX_RESULTS, 0, sizeof(struct AddrInfo) * MAX_RESULTS) != 0) {
        return -1;
    }

    int32_t resNum = 0;
    for (struct addrinfo *tmp = res; tmp != NULL; tmp = tmp->ai_next) {
        addrInfo[resNum].aiFlags = tmp->ai_flags;
        addrInfo[resNum].aiFamily = tmp->ai_family;
        addrInfo[resNum].aiSockType = tmp->ai_socktype;
        addrInfo[resNum].aiProtocol = tmp->ai_protocol;
        addrInfo[resNum].aiAddrLen = tmp->ai_addrlen;
        if (memcpy_s(&addrInfo[resNum].aiAddr, sizeof(addrInfo[resNum].aiAddr), tmp->ai_addr, tmp->ai_addrlen) != 0) {
            DNS_CONFIG_PRINT("memcpy_s failed");
            return -1;
        }
        if (strcpy_s(addrInfo[resNum].aiCanonName, sizeof(addrInfo[resNum].aiCanonName), tmp->ai_canonname) != 0) {
            DNS_CONFIG_PRINT("strcpy_s failed");
            return -1;
        }

        ++resNum;
        if (resNum >= MAX_RESULTS) {
            break;
        }
    }

    return resNum;
}

static int32_t NetSysSetResolvCacheInternal(int sockFd, uint16_t netId, const struct ParamWrapper param,
                                            struct addrinfo *res)
{
    struct RequestInfo info = {
        .command = SET_CACHE,
        .netId = netId,
    };

    int32_t result = NetsysSendKeyForCache(sockFd, param, info);
    if (result < 0) {
        return result;
    }

    struct AddrInfo addrInfo[MAX_RESULTS] = {};
    int32_t resNum = FillAddrInfo(addrInfo, res);
    if (resNum < 0) {
        return CloseSocketReturn(sockFd, -1);
    }

    if (!PollSendData(sockFd, (char *)&resNum, sizeof(resNum))) {
        DNS_CONFIG_PRINT("send failed %d", errno);
        return CloseSocketReturn(sockFd, -errno);
    }

    if (resNum == 0) {
        return CloseSocketReturn(sockFd, 0);
    }

    if (!PollSendData(sockFd, (char *)addrInfo, sizeof(struct AddrInfo) * resNum)) {
        DNS_CONFIG_PRINT("send failed %d", errno);
        return CloseSocketReturn(sockFd, -errno);
    }

    return CloseSocketReturn(sockFd, 0);
}

int32_t NetSysSetResolvCache(uint16_t netId, const struct ParamWrapper param, struct addrinfo *res)
{
    char *hostName = param.host;
    if (hostName == NULL || strlen(hostName) == 0 || res == NULL) {
        DNS_CONFIG_PRINT("Invalid Param");
        return -EINVAL;
    }

    int sockFd = CreateConnectionToNetSys();
    if (sockFd < 0) {
        DNS_CONFIG_PRINT("NetSysSetResolvCache CreateConnectionToNetSys connect to netsys err: %d", errno);
        return sockFd;
    }

    int err = NetSysSetResolvCacheInternal(sockFd, netId, param, res);
    if (err < 0) {
        DNS_CONFIG_PRINT("NetSysSetResolvCache NetSysSetResolvCacheInternal err: %d", errno);
        return err;
    }

    return 0;
}

#ifdef __cplusplus
}
#endif
