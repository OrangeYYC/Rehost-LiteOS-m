/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef NET_MONITOR_H
#define NET_MONITOR_H

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>

#include "refbase.h"

#include "i_net_monitor_callback.h"
#include "net_conn_types.h"

namespace OHOS {
namespace NetManagerStandard {
class NetMonitor : public virtual RefBase, public std::enable_shared_from_this<NetMonitor> {
public:
    /**
     * Construct a new NetMonitor to detection a network
     *
     * @param netId Detection network's id
     * @param callback Network monitor callback weak reference
     */
    NetMonitor(uint32_t netId, const std::weak_ptr<INetMonitorCallback> &callback);

    /**
     * Destroy the NetMonitor
     *
     */
    virtual ~NetMonitor() = default;

    /**
     * Start detection
     *
     */
    void Start();

    /**
     * Stop detecting
     *
     */
    void Stop();

    /**
     * Set network socket parameter
     *
     * @return Socket parameter setting result
     */
    int32_t SetSocketParameter(int32_t sockFd);

    /**
     * Is network monitor detecting
     *
     * @return Status value of whether the network is detecting
     */
    bool IsDetecting();

    /**
     * Network monitor detection
     *
     */
    void Detection();

private:
    NetDetectionStatus SendParallelHttpProbes();
    NetDetectionStatus SendHttpProbe(const std::string &defaultDomain, const std::string &defaultUrl,
                                     const uint16_t defaultPort);
    int32_t GetStatusCodeFromResponse(const std::string &strResponse);
    int32_t GetUrlRedirectFromResponse(const std::string &strResponse, std::string &urlRedirect);
    NetDetectionStatus dealRecvResult(const std::string &strResponse);
    int32_t GetDefaultNetDetectionUrlFromCfg(std::string &strUrl);
    int32_t ParseUrl(const std::string &url, std::string &domain, std::string &urlPath);
    int32_t GetIpAddr(const std::string domain, std::string &ip_addr, int &socketType);

    /**
     * Non blocking socket connection
     *
     * @param sockFd Socket fd to connect
     * @param port Transport layer default port
     * @param ipAddr IP address
     *
     * @return Socket connect result
     */
    int32_t Connect(int32_t sockFd, int socketType, const uint16_t port, const std::string &ipAddr);

    /**
     * Connect ipv4 ip address socket
     *
     * @param sockFd Socket fd to connect
     * @param port Transport layer default port
     * @param ipAddr IP address
     *
     * @return Socket connect result
     */
    int32_t ConnectIpv4(int32_t sockFd, const uint16_t port, const std::string &ipAddr);

    /**
     * Connect ipv6 ip address socket
     *
     * @param sockFd Socket fd to connect
     * @param port Transport layer default port
     * @param ipAddr IP address
     *
     * @return Socket connect result
     */
    int32_t ConnectIpv6(int32_t sockFd, const uint16_t port, const std::string &ipAddr);

    /**
     * Ready for socket reading or writing
     *
     * @param sockFd Socket fd to connect
     * @param canRead Store readable status
     * @param canWrite Store writable status
     *
     * @return Socket preparation result
     */
    int32_t Wait(int32_t sockFd, uint8_t *canRead, uint8_t *canWrite);

    /**
     * Send data to socket
     *
     * @param sockFd Socket fd to send
     * @param domain Probe link
     * @param url Probe url
     *
     * @return Result of send data to socket
     */
    int32_t Send(int32_t sockFd, const std::string &domain, const std::string &url);

    /**
     * Receive data from socket
     *
     * @param sockFd Socket fd to receive
     * @param probResult Store received data
     *
     * @return Result of receive data from socket
     */
    int32_t Receive(int32_t sockFd, std::string &probResult);

private:
    uint32_t netId_ = 0;
    std::atomic<bool> isDetecting_ = false;
    int32_t detectionSteps_ = 0;
    std::mutex detectionMtx_;
    std::condition_variable detectionCond_;
    uint32_t detectionDelay_ = 0;
    std::string portalUrlRedirect_;
    std::weak_ptr<INetMonitorCallback> netMonitorCallback_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_MONITOR_H
