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

#ifndef NETSYS_DNS_PARAM_CACHE_H
#define NETSYS_DNS_PARAM_CACHE_H

#include <iostream>
#include <map>

#include "dns_resolv_config.h"
#include "netnative_log_wrapper.h"

#if DNS_CONFIG_DEBUG
#ifdef DNS_CONFIG_PRINT
#undef DNS_CONFIG_PRINT
#endif
#define DNS_CONFIG_PRINT(fmt, ...) NETNATIVE_LOGI("DNS" fmt, ##__VA_ARGS__)
#else
#define DNS_CONFIG_PRINT(fmt, ...)
#endif

namespace OHOS::nmd {
class DnsParamCache {
public:
    DnsParamCache();
    ~DnsParamCache() = default;

    // for net_conn_service
    int32_t SetResolverConfig(uint16_t netId, uint16_t baseTimeoutMsec, uint8_t retryCount,
                              const std::vector<std::string> &servers, const std::vector<std::string> &domains);

    int32_t CreateCacheForNet(uint16_t netId);

    void SetDefaultNetwork(uint16_t netId);

    // for client
    void SetDnsCache(uint16_t netId, const std::string &hostName, const AddrInfo &addrInfo);

    void SetCacheDelayed(uint16_t netId, const std::string &hostName);

    std::vector<AddrInfo> GetDnsCache(uint16_t netId, const std::string &hostName);

    int32_t GetResolverConfig(uint16_t netId, std::vector<std::string> &servers, std::vector<std::string> &domains,
                              uint16_t &baseTimeoutMsec, uint8_t &retryCount);

    int32_t GetDefaultNetwork() const;

    void GetDumpInfo(std::string &info);

    int32_t DestroyNetworkCache(uint16_t netId);

private:
    std::mutex cacheMutex_;

    std::atomic_uint defaultNetId_;

    std::map<uint16_t, DnsResolvConfig> serverConfigMap_;

    static std::vector<std::string> SelectNameservers(const std::vector<std::string> &servers);
};
} // namespace OHOS::nmd
#endif // NETSYS_DNS_PARAM_CACHE_H
