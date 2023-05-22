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

#ifndef INCLUDE_MANAGER_DNS_MANAGER_H
#define INCLUDE_MANAGER_DNS_MANAGER_H

#include <vector>

#include "dns_getaddrinfo.h"
#include "dns_param_cache.h"
#include "dns_proxy_listen.h"

namespace OHOS {
namespace nmd {
class DnsManager {
public:
    DnsManager();
    ~DnsManager() = default;

    /**
     * Set the Resolver Config object
     *
     * @param netId network ID
     * @param baseTimeoutMillis base Timeout Ms, default 5000
     * @param retryCount retry Count, default 2
     * @param servers server name set in config
     * @param domains domain set in config
     * @return int32_t 0:success -1:failed
     */
    int32_t SetResolverConfig(uint16_t netId, uint16_t baseTimeoutMillis, uint8_t retryCount,
                              const std::vector<std::string> &servers, const std::vector<std::string> &domains);

    /**
     * Get the Resolver Config object
     *
     * @param netId network ID
     * @param servers return value server name
     * @param domains return value doamin
     * @param baseTimeoutMillis return value Timeout Ms
     * @param retryCount return value retry Count
     * @return int32_t 0:success -1:failed
     */
    int32_t GetResolverConfig(uint16_t netId, std::vector<std::string> &servers, std::vector<std::string> &domains,
                              uint16_t &baseTimeoutMillis, uint8_t &retryCount);

    /**
     * Create a Network Cache object
     *
     * @param netId network ID
     * @return int32_t 0:success -1:failed
     */
    int32_t CreateNetworkCache(uint16_t netId);

    /**
     * Set the Default Network object
     *
     * @param netId network ID
     */
    void SetDefaultNetwork(uint16_t netId);

    /**
     * Network share set netId
     *
     * @param netId network ID
     */
    void ShareDnsSet(uint16_t netId);

    /**
     * Start Dns proxy for network share
     *
     */
    void StartDnsProxyListen();

    /**
     * Stop Dns proxy for network share
     *
     */
    void StopDnsProxyListen();

    /**
     * Get the Dump Info object, this is for dump.
     *
     * @param info Infos for dump
     */
    void GetDumpInfo(std::string &info);

    /**
     * dns resolution object
     *
     * @param node hostname
     * @param service service name
     * @param hints limit
     * @param result return value
     * @param netId network id
     * @return int32_t  0 is success -1 is failed
     */
    int32_t GetAddrInfo(const std::string &hostName, const std::string &serverName, const AddrInfo &hints,
                        uint16_t netId, std::vector<AddrInfo> &res);

    /**
     * destroy this netid's cache
     * @param netId network's id
     * @return destroy is success? 0 : -1
     */
    int32_t DestroyNetworkCache(uint16_t netId);

private:
    std::shared_ptr<DnsProxyListen> dnsProxyListen_;
    std::shared_ptr<DnsGetAddrInfo> dnsGetAddrInfo_;
};
} // namespace nmd
} // namespace OHOS
#endif // INCLUDE_MANAGER_DNS_MANAGER_H
