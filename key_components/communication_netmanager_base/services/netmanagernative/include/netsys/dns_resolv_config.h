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

#ifndef INCLUDE_DNSRESOLV_CONFIG_H
#define INCLUDE_DNSRESOLV_CONFIG_H

#include <atomic>
#include <vector>

#include "delayed_queue.h"
#include "dns_config_client.h"
#include "lru_cache.h"

namespace OHOS::nmd {
static constexpr size_t DEFAULT_DELAYED_COUNT = 30;

class DnsResolvConfig {
public:
    DnsResolvConfig();

    void SetNetId(uint16_t netId);
    void SetTimeoutMsec(int32_t baseTimeoutMsec);
    void SetRetryCount(uint8_t retryCount);
    void SetServers(const std::vector<std::string> &servers);
    void SetDomains(const std::vector<std::string> &domains);

    uint16_t GetNetId() const;
    uint16_t GetTimeoutMsec() const;
    std::vector<std::string> GetServers() const;
    std::vector<std::string> GetDomains() const;
    uint8_t GetRetryCount() const;

    NetManagerStandard::LRUCache<AddrInfo> &GetCache();

    void SetCacheDelayed(const std::string &hostName);

private:
    class DelayedTaskWrapper {
    public:
        DelayedTaskWrapper(std::string hostName, NetManagerStandard::LRUCache<AddrInfo> &cache);

        void Execute() const;

        bool operator<(const DelayedTaskWrapper &other) const;

    private:
        std::string hostName_;

        NetManagerStandard::LRUCache<AddrInfo> &cache_;
    };

    uint16_t netId_;
    std::atomic_bool netIdIsSet_;
    int32_t revisionId_;
    int32_t timeoutMsec_;
    uint8_t retryCount_;
    std::vector<std::string> nameServers_;
    std::vector<std::string> searchDomains_;
    NetManagerStandard::LRUCache<AddrInfo> cache_;
    NetManagerStandard::DelayedQueue<DelayedTaskWrapper, NetManagerStandard::DEFAULT_CAPABILITY, DEFAULT_DELAYED_COUNT>
        delayedQueue_;
};
} // namespace OHOS::nmd
#endif // INCLUDE_DNSRESOLV_CONFIG_H
