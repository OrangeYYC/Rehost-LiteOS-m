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

#ifndef NETMANAGER_BASE_IPTABLES_WRAPPER_H
#define NETMANAGER_BASE_IPTABLES_WRAPPER_H

#include <condition_variable>
#include <cstring>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include "event_runner.h"
#include "event_handler.h"
#include "singleton.h"

namespace OHOS {
namespace nmd {
using EventRunner = OHOS::AppExecFwk::EventRunner;
using EventHandler = OHOS::AppExecFwk::EventHandler;
enum IpType {
    IPTYPE_IPV4 = 1,
    IPTYPE_IPV6 = 2,
    IPTYPE_IPV4V6 = 3,
};
class IptablesWrapper : public std::enable_shared_from_this<IptablesWrapper> {
    DECLARE_DELAYED_SINGLETON(IptablesWrapper)
public:
    /**
     * @param ipType ipv4 or ipv6
     * @param command iptables command
     * @return NETMANAGER_SUCCESS suceess or NETMANAGER_ERROR failed
     */
    int32_t RunCommand(const IpType &ipType, const std::string &command);

    /**
     * @brief run iptables exec for result.
     *
     * @param ipType ipv4 or ipv6.
     * @param command iptables command.
     * @return NETMANAGER_SUCCESS suceess or NETMANAGER_ERROR failed
     */
    std::string RunCommandForRes(const IpType &ipType, const std::string &command);

private:
    void ExecuteCommand(const std::string &command);
    void ExecuteCommandForRes(const std::string &command);

private:
    std::mutex iptablesMutex_;
    std::condition_variable conditionVarLock_;
    bool isRunningFlag_ = false;
    bool isIptablesSystemAccess_ = false;
    bool forRes_ = false;
    std::string result_;
    std::thread iptablesWrapperThread_;
    std::queue<std::string> commandsQueue_;
    std::shared_ptr<EventHandler> handler_ = nullptr;
};
} // namespace nmd
} // namespace OHOS
#endif /* NETMANAGER_BASE_IPTABLES_WRAPPER_H */
