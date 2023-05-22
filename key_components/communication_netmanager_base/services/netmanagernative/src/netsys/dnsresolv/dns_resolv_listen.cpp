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

#include <arpa/inet.h>
#include <sys/stat.h>
#include <thread>

#include "dns_config_client.h"
#include "dns_param_cache.h"
#include "netsys_client.h"
#include "init_socket.h"
#ifdef USE_SELINUX
#include "selinux.h"
#endif
#include "singleton.h"

#include "dns_resolv_listen.h"

namespace OHOS::nmd {
static constexpr const uint32_t MAX_LISTEN_NUM = 1024;

DnsResolvListen::DnsResolvListen() : serverSockFd_(-1)
{
    NETNATIVE_LOGE("DnsResolvListen start");
}

DnsResolvListen::~DnsResolvListen()
{
    NETNATIVE_LOGE("DnsResolvListen end");
    if (serverSockFd_ > 0) {
        close(serverSockFd_);
    }
}

void DnsResolvListen::ProcGetConfigCommand(int clientSockFd, uint32_t netId)
{
    DNS_CONFIG_PRINT("ProcGetConfigCommand");
    ResolvConfig sendData = {0};
    std::vector<std::string> servers;
    std::vector<std::string> domains;
    uint16_t baseTimeoutMsec = DEFAULT_TIMEOUT;
    uint8_t retryCount = DEFAULT_RETRY;

    auto status = DelayedSingleton<DnsParamCache>::GetInstance()->GetResolverConfig(
        static_cast<uint16_t>(netId), servers, domains, baseTimeoutMsec, retryCount);
    DNS_CONFIG_PRINT("GetResolverConfig status: %{public}d", status);
    if (status < 0) {
        sendData.error = status;
    } else {
        sendData.retryCount = retryCount;
        sendData.timeoutMs = baseTimeoutMsec;
        for (size_t i = 0; i < std::min<size_t>(MAX_SERVER_NUM, servers.size()); i++) {
            if (memcpy_s(sendData.nameservers[i], sizeof(sendData.nameservers[i]), servers[i].c_str(),
                         servers[i].length()) < 0) {
                DNS_CONFIG_PRINT("mem copy failed");
                continue;
            }
            DNS_CONFIG_PRINT("i = %{public}d sendData.nameservers: %{public}s", i, sendData.nameservers[i]);
        }
    }
    if (!PollSendData(clientSockFd, reinterpret_cast<char *>(&sendData), sizeof(ResolvConfig))) {
        DNS_CONFIG_PRINT("send failed");
    }
    DNS_CONFIG_PRINT("ProcGetConfigCommand end");
}

int32_t DnsResolvListen::ProcGetKeyForCache(int clientSockFd, char *name)
{
    DNS_CONFIG_PRINT("ProcGetKeyForCache");
    uint32_t nameLen = 0;
    if (!PollRecvData(clientSockFd, reinterpret_cast<char *>(&nameLen), sizeof(nameLen))) {
        DNS_CONFIG_PRINT("read errno %{public}d", errno);
        close(clientSockFd);
        return -1;
    }

    if (nameLen > MAX_HOST_NAME_LEN) {
        DNS_CONFIG_PRINT("MAX_HOST_NAME_LEN is %{public}u, but get %{public}u", MAX_HOST_NAME_LEN, nameLen);
        close(clientSockFd);
        return -1;
    }

    if (!PollRecvData(clientSockFd, name, nameLen)) {
        DNS_CONFIG_PRINT("read errno %{public}d", errno);
        close(clientSockFd);
        return -1;
    }
    DNS_CONFIG_PRINT("ProcGetKeyForCache end");
    return 0;
}

void DnsResolvListen::ProcGetCacheCommand(int clientSockFd, uint32_t netId)
{
    DNS_CONFIG_PRINT("ProcGetCacheCommand");
    char name[MAX_HOST_NAME_LEN] = {0};
    int32_t res = ProcGetKeyForCache(clientSockFd, name);
    if (res < 0) {
        return;
    }

    auto cacheRes = DelayedSingleton<DnsParamCache>::GetInstance()->GetDnsCache(netId, name);

    uint32_t resNum = std::min<uint32_t>(MAX_RESULTS, static_cast<uint32_t>(cacheRes.size()));
    if (!PollSendData(clientSockFd, reinterpret_cast<char *>(&resNum), sizeof(resNum))) {
        DNS_CONFIG_PRINT("send errno %{public}d", errno);
        close(clientSockFd);
        return;
    }

    if (resNum == 0 || resNum > MAX_RESULTS) {
        return;
    }

    AddrInfo addrInfo[MAX_RESULTS] = {};
    for (uint32_t i = 0; i < resNum; i++) {
        if (memcpy_s(reinterpret_cast<char *>(&addrInfo[i]), sizeof(AddrInfo),
                     reinterpret_cast<char *>(&cacheRes[i]), sizeof(AddrInfo)) != 0) {
            return;
        }
    }
    if (!PollSendData(clientSockFd, reinterpret_cast<char *>(addrInfo), sizeof(AddrInfo) * resNum)) {
        DNS_CONFIG_PRINT("send errno %{public}d", errno);
        close(clientSockFd);
        return;
    }
    DNS_CONFIG_PRINT("ProcGetCacheCommand end");
}

void DnsResolvListen::ProcSetCacheCommand(int clientSockFd, uint32_t netId)
{
    DNS_CONFIG_PRINT("ProcSetCacheCommand");
    char name[MAX_HOST_NAME_LEN] = {0};
    int32_t res = ProcGetKeyForCache(clientSockFd, name);
    if (res < 0) {
        return;
    }

    uint32_t resNum = 0;
    if (!PollRecvData(clientSockFd, reinterpret_cast<char *>(&resNum), sizeof(resNum))) {
        DNS_CONFIG_PRINT("read errno %{public}d", errno);
        close(clientSockFd);
        return;
    }

    resNum = std::min<uint32_t>(MAX_RESULTS, resNum);
    if (resNum == 0) {
        return;
    }

    AddrInfo addrInfo[MAX_RESULTS] = {};
    if (!PollRecvData(clientSockFd, reinterpret_cast<char *>(addrInfo), sizeof(AddrInfo) * resNum)) {
        DNS_CONFIG_PRINT("read errno %{public}d", errno);
        close(clientSockFd);
        return;
    }

    for (size_t i = 0; i < resNum; ++i) {
        DelayedSingleton<DnsParamCache>::GetInstance()->SetDnsCache(netId, name, addrInfo[i]);
    }
    DelayedSingleton<DnsParamCache>::GetInstance()->SetCacheDelayed(netId, name);
    DNS_CONFIG_PRINT("ProcSetCacheCommand end");
}

void DnsResolvListen::ProcCommand(int clientSockFd)
{
    char buff[sizeof(RequestInfo)] = {0};
    if (!PollRecvData(clientSockFd, buff, sizeof(buff))) {
        DNS_CONFIG_PRINT("read errno %{public}d", errno);
        close(clientSockFd);
        return;
    }

    auto info = reinterpret_cast<RequestInfo *>(buff);
    auto netId = info->netId;

    switch (info->command) {
        case GET_CONFIG:
            ProcGetConfigCommand(clientSockFd, netId);
            break;
        case GET_CACHE:
            ProcGetCacheCommand(clientSockFd, netId);
            break;
        case SET_CACHE:
            ProcSetCacheCommand(clientSockFd, netId);
            break;
        default:
            DNS_CONFIG_PRINT("invalid command %{public}u", info->command);
            break;
    }

    close(clientSockFd);
}

void DnsResolvListen::StartListen()
{
    NETNATIVE_LOGE("Enter StartListen");

    unlink(DNS_SOCKET_PATH);

    serverSockFd_ = GetControlSocket(DNS_SOCKET_NAME);
    if (serverSockFd_ < 0) {
        NETNATIVE_LOGE("create socket failed %{public}d", errno);
        return;
    }

    // listen
    if (listen(serverSockFd_, MAX_LISTEN_NUM) < 0) {
        NETNATIVE_LOGE("listen errno %{public}d", errno);
        close(serverSockFd_);
        return;
    }

    NETNATIVE_LOGE("begin listen");

    while (true) {
        sockaddr_un clientAddr = {0};
        socklen_t len = sizeof(clientAddr);

        int clientSockFd = accept(serverSockFd_, (sockaddr *)&clientAddr, &len);
        if (clientSockFd < 0) {
            DNS_CONFIG_PRINT("accept errno %{public}d", errno);
            continue;
        }
        if (!MakeNonBlock(clientSockFd)) {
            DNS_CONFIG_PRINT("MakeNonBlock errno %{public}d", errno);
            close(clientSockFd);
            continue;
        }

        std::thread(DnsResolvListen::ProcCommand, clientSockFd).detach();
    }
}
} // namespace OHOS::nmd
