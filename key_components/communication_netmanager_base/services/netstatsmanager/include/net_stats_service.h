/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef NET_STATS_SERVICE_H
#define NET_STATS_SERVICE_H

#include "singleton.h"
#include "system_ability.h"

#include "net_stats_cached.h"
#include "net_stats_callback.h"
#include "net_stats_listener.h"
#include "net_stats_service_stub.h"
#include "net_stats_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
class NetStatsService : public SystemAbility,
                        public NetStatsServiceStub,
                        public std::enable_shared_from_this<NetStatsService> {
    DECLARE_DELAYED_SINGLETON(NetStatsService)
    DECLARE_SYSTEM_ABILITY(NetStatsService)

public:
    void OnStart() override;
    void OnStop() override;
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string &deviceId) override;
    int32_t Dump(int32_t fd, const std::vector<std::u16string> &args) override;
    int32_t GetIfaceRxBytes(uint64_t &stats, const std::string &interfaceName) override;
    int32_t GetIfaceTxBytes(uint64_t &stats, const std::string &interfaceName) override;
    int32_t GetCellularRxBytes(uint64_t &stats) override;
    int32_t GetCellularTxBytes(uint64_t &stats) override;
    int32_t GetAllRxBytes(uint64_t &stats) override;
    int32_t GetAllTxBytes(uint64_t &stats) override;
    int32_t GetUidRxBytes(uint64_t &stats, uint32_t uid) override;
    int32_t GetUidTxBytes(uint64_t &stats, uint32_t uid) override;
    int32_t RegisterNetStatsCallback(const sptr<INetStatsCallback> &callback) override;
    int32_t UnregisterNetStatsCallback(const sptr<INetStatsCallback> &callback) override;
    int32_t GetIfaceStatsDetail(const std::string &iface, uint64_t start, uint64_t end,
                                NetStatsInfo &statsInfo) override;
    int32_t GetUidStatsDetail(const std::string &iface, uint32_t uid, uint64_t start, uint64_t end,
                              NetStatsInfo &statsInfo) override;
    int32_t UpdateIfacesStats(const std::string &iface, uint64_t start, uint64_t end,
                              const NetStatsInfo &stats) override;
    int32_t UpdateStatsData() override;
    int32_t ResetFactory() override;

private:
    bool Init();
    void GetDumpMessage(std::string &message);

private:
    enum ServiceRunningState {
        STATE_STOPPED = 0,
        STATE_RUNNING,
    };

    bool registerToService_;
    ServiceRunningState state_;
    std::shared_ptr<NetStatsCallback> netStatsCallback_ = nullptr;
    std::shared_ptr<NetStatsListener> subscriber_ = nullptr;
    std::unique_ptr<NetStatsWrapper> netStatsWrapper_ = nullptr;
    std::unique_ptr<NetStatsCached> netStatsCached_ = nullptr;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_STATS_SERVICE_H
