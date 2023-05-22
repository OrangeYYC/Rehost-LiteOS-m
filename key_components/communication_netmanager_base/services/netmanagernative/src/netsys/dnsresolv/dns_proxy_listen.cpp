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

#include <netinet/ip.h>
#include <netinet/udp.h>
#include <thread>
#include <unistd.h>

#include "dns_config_client.h"
#include "dns_param_cache.h"
#include "netnative_log_wrapper.h"
#include "netsys_udp_transfer.h"
#include "singleton.h"

#include "dns_proxy_listen.h"

namespace OHOS {
namespace nmd {
uint16_t DnsProxyListen::netId_ = 0;
bool DnsProxyListen::proxyListenSwitch_ = false;
constexpr uint16_t DNS_PROXY_PORT = 53;
constexpr uint8_t RESPONSE_FLAG = 0x80;
constexpr uint8_t RESPONSE_FLAG_USED = 80;
constexpr size_t FLAG_BUFF_LEN = 1;
constexpr size_t FLAG_BUFF_OFFSET = 2;
DnsProxyListen::DnsProxyListen() : proxySockFd_(-1) {}
DnsProxyListen::~DnsProxyListen()
{
    if (proxySockFd_ > 0) {
        close(proxySockFd_);
        proxySockFd_ = -1;
    }
}

void DnsProxyListen::DnsProxyGetPacket(int32_t clientSocket, RecvBuff recvBuff, sockaddr_in proxyAddr)
{
    std::vector<std::string> servers;
    std::vector<std::string> domains;
    uint16_t baseTimeoutMsec;
    uint8_t retryCount;
    auto status = DelayedSingleton<DnsParamCache>::GetInstance()->GetResolverConfig(
        DnsProxyListen::netId_, servers, domains, baseTimeoutMsec, retryCount);
    if (status < 0) {
        NETNATIVE_LOGE("GetResolvConfig failed set default server failed status:[%{poublic}d]", status);
        return;
    }
    DnsParseBySocket(clientSocket, servers, recvBuff, proxyAddr);
}

void DnsProxyListen::DnsParseBySocket(int32_t clientSocket, std::vector<std::string> servers, RecvBuff recvBuff,
                                      sockaddr_in proxyAddr)
{
    int32_t parseSocketFd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC | SOCK_NONBLOCK, IPPROTO_UDP);
    if (parseSocketFd < 0) {
        NETNATIVE_LOGE("parseSocketFd create socket failed %{public}d", errno);
        return;
    }

    if (!PollUdpDataTransfer::MakeUdpNonBlock(parseSocketFd)) {
        NETNATIVE_LOGE("MakeNonBlock error  %{public}d: %{public}s", errno, strerror(errno));
        close(parseSocketFd);
        return;
    }
    int32_t resLen;
    socklen_t addrLen;
    char requesData[MAX_REQUESTDATA_LEN] = {0};
    sockaddr_in addrParse = {0};
    uint32_t serversNum = 0;
    while (serversNum < servers.size()) {
        addrParse.sin_family = AF_INET;
        addrParse.sin_addr.s_addr = inet_addr(servers[serversNum].c_str());
        if (addrParse.sin_addr.s_addr == INADDR_NONE) {
            NETNATIVE_LOGE("Input dns server [%{public}s] is not correct!", servers[serversNum].c_str());
            serversNum++;
            continue;
        }
        addrParse.sin_port = htons(DNS_PROXY_PORT);
        if (PollUdpDataTransfer::PollUdpSendData(parseSocketFd, recvBuff.questionsBuff, recvBuff.questionLen, addrParse,
                                                 addrLen) < 0) {
            NETNATIVE_LOGE("send failed %{public}d: %{public}s", errno, strerror(errno));
            serversNum++;
            continue;
        }
        addrLen = sizeof(addrParse);
        resLen =
            PollUdpDataTransfer::PollUdpRecvData(parseSocketFd, requesData, MAX_REQUESTDATA_LEN, addrParse, addrLen);
        if (resLen > 0) {
            break;
        }
        if (!CheckDnsResponse(requesData, MAX_REQUESTDATA_LEN)) {
            NETNATIVE_LOGE("read buff is not dns answer");
            break;
        }
        if (serversNum == servers.size() - 1) {
            return;
        }
        serversNum++;
    }
    close(parseSocketFd);
    if (resLen > 0) {
        DnsSendRecvParseData(clientSocket, requesData, resLen, proxyAddr);
    }
}

void DnsProxyListen::DnsSendRecvParseData(int32_t clientSocket, char *requesData, int32_t resLen, sockaddr_in proxyAddr)
{
    socklen_t addrLen = sizeof(proxyAddr);
    if (PollUdpDataTransfer::PollUdpSendData(clientSocket, requesData, resLen, proxyAddr, addrLen) < 0) {
        NETNATIVE_LOGE("send failed %{public}d: %{public}s", errno, strerror(errno));
    }
}

void DnsProxyListen::StartListen()
{
    RecvBuff recvBuff = {{0}};
    NETNATIVE_LOG_D("StartListen proxySockFd_ : %{public}d", proxySockFd_);
    if (proxySockFd_ < 0) {
        proxySockFd_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (proxySockFd_ < 0) {
            NETNATIVE_LOGE("proxySockFd_ create socket failed %{public}d", errno);
            return;
        }
    }

    sockaddr_in proxyAddr;
    (void)memset_s(&proxyAddr, sizeof(proxyAddr), 0, sizeof(proxyAddr));
    proxyAddr.sin_family = AF_INET;
    proxyAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    proxyAddr.sin_port = htons(DNS_PROXY_PORT);

    if (bind(proxySockFd_, (sockaddr *)&proxyAddr, sizeof(proxyAddr)) == -1) {
        NETNATIVE_LOGE("bind errno %{public}d: %{public}s", errno, strerror(errno));
        close(proxySockFd_);
        proxySockFd_ = -1;
        return;
    }
    while (true) {
        if (DnsThreadClose()) {
            break;
        }
        (void)memset_s(recvBuff.questionsBuff, MAX_REQUESTDATA_LEN, 0, MAX_REQUESTDATA_LEN);
        socklen_t len = sizeof(proxyAddr);
        recvBuff.questionLen = recvfrom(proxySockFd_, recvBuff.questionsBuff, MAX_REQUESTDATA_LEN, 0,
                                        reinterpret_cast<sockaddr *>(&proxyAddr), &len);
        if (!(recvBuff.questionLen > 0)) {
            NETNATIVE_LOGE("read errno %{public}d", errno);
            continue;
        }
        if (CheckDnsResponse(recvBuff.questionsBuff, MAX_REQUESTDATA_LEN)) {
            NETNATIVE_LOGE("read buff is not dns question");
            continue;
        }

        if (DnsThreadClose()) {
            break;
        }
        std::thread(DnsProxyListen::DnsProxyGetPacket, proxySockFd_, recvBuff, proxyAddr).detach();
    }
}

bool DnsProxyListen::CheckDnsResponse(char *recBuff, size_t recLen)
{
    if (recLen < FLAG_BUFF_LEN + FLAG_BUFF_OFFSET) {
        return false;
    }
    uint8_t flagBuff;
    char *recFlagBuff = recBuff + FLAG_BUFF_OFFSET;
    if (memcpy_s(reinterpret_cast<char *>(&flagBuff), FLAG_BUFF_LEN, recFlagBuff, FLAG_BUFF_LEN) != 0) {
        return false;
    }
    int reqFlag = (flagBuff & RESPONSE_FLAG) / RESPONSE_FLAG_USED;
    if (reqFlag) {
        return true; // answer
    } else {
        return false; // question
    }
}

bool DnsProxyListen::DnsThreadClose()
{
    return !DnsProxyListen::proxyListenSwitch_;
}

void DnsProxyListen::OnListen()
{
    DnsProxyListen::proxyListenSwitch_ = true;
    NETNATIVE_LOG_D("endl OnListen");
}

void DnsProxyListen::OffListen()
{
    DnsProxyListen::proxyListenSwitch_ = false;
    if (proxySockFd_ > 0) {
        close(proxySockFd_);
        proxySockFd_ = -1;
    }
    NETNATIVE_LOG_D("endl OffListen");
}

void DnsProxyListen::SetParseNetId(uint16_t netId)
{
    DnsProxyListen::netId_ = netId;
    NETNATIVE_LOG_D("endl SetParseNetId");
}
} // namespace nmd
} // namespace OHOS