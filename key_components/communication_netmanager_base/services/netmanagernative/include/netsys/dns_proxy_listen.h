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

#ifndef INCLUDE_DNS_PROXY_LISTEN_H
#define INCLUDE_DNS_PROXY_LISTEN_H

#include <iostream>
#include <netinet/in.h>
#include <vector>

namespace OHOS {
namespace nmd {
class DnsProxyListen {
public:
    DnsProxyListen();
    ~DnsProxyListen();

    /**
     * Begin dns proxy listen
     *
     */
    void OnListen();

    /**
     * Close dns proxy listen
     */
    void OffListen();

    /**
     * Dns proxy listen obj
     *
     */
    void StartListen();

    /**
     * Set the Parse Net Id objectse
     *
     * @param netId network ID
     */
    void SetParseNetId(uint16_t netId);

private:
    static constexpr const uint32_t MAX_REQUESTDATA_LEN = 512;
    struct RecvBuff {
        char questionsBuff[MAX_REQUESTDATA_LEN];
        int32_t questionLen;
    };
    static void DnsProxyGetPacket(int32_t clientSocket, RecvBuff recvBuff, sockaddr_in proxyAddr);
    static void DnsParseBySocket(int32_t clientSocket, std::vector<std::string> servers, RecvBuff recvBuff,
                                 sockaddr_in proxyAddr);
    static void DnsSendRecvParseData(int32_t clientSocket, char *requestData, int32_t resLen, sockaddr_in proxyAddr);
    static bool CheckDnsResponse(char* recBuff, size_t recLen);
    static bool DnsThreadClose();
    int32_t proxySockFd_;
    static uint16_t netId_;
    static bool proxyListenSwitch_;
};
} // namespace nmd
} // namespace OHOS
#endif // INCLUDE_DNS_PROXY_LISTEN_H
