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

#include "netconnection.h"

#include <mutex>

namespace OHOS::NetManagerStandard {
std::map<NetConnCallbackObserver *, NetConnection *> NET_CONNECTIONS;
std::mutex g_netConnectionsMutex;

NetConnection::NetConnection(EventManager *eventManager)
    : hasNetSpecifier_(false),
      hasTimeout_(false),
      timeout_(0),
      observer_(new NetConnCallbackObserver),
      manager_(eventManager)
{
}

NetConnection *NetConnection::MakeNetConnection(EventManager *eventManager)
{
    std::lock_guard<std::mutex> lock(g_netConnectionsMutex);
    auto netConnection = new NetConnection(eventManager);
    NET_CONNECTIONS[netConnection->observer_.GetRefPtr()] = netConnection;
    return netConnection;
}

void NetConnection::DeleteNetConnection(NetConnection *netConnection)
{
    std::lock_guard<std::mutex> lock(g_netConnectionsMutex);
    NET_CONNECTIONS.erase(netConnection->observer_.GetRefPtr());
    delete netConnection;
}

sptr<NetConnCallbackObserver> NetConnection::GetObserver() const
{
    return observer_;
}

EventManager *NetConnection::GetEventManager() const
{
    return manager_;
}
} // namespace OHOS::NetManagerStandard
