/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef MDNS_SOCKET_LISTENER_H
#define MDNS_SOCKET_LISTENER_H

#include <atomic>
#include <cstddef>
#include <functional>
#include <mutex>
#include <string_view>
#include <sys/socket.h>
#include <thread>

#include <ifaddrs.h>

#include "mdns_common.h"

namespace OHOS {
namespace NetManagerStandard {

// This class create and manage udp socket for mDNS transmission
// In multi-interface device, we should create socket for each iface for multicast
class MDnsSocketListener {
public:
    using ReceiveHandler = std::function<void(int, const MDnsPayload &)>;
    using SendHandler = std::function<void(int)>;

    MDnsSocketListener();
    ~MDnsSocketListener();

    void OpenSocketForEachIface(bool ipv6Support, bool lo);
    void OpenSocketForDefault(bool ipv6Support);
    void CloseAllSocket();
    void Start();
    void Stop();
    ssize_t MulticastAll(const MDnsPayload &payload);
    void SetReceiveHandler(const ReceiveHandler &callback);
    void SetRefreshHandler(const SendHandler &callback);
    ssize_t Multicast(int sock, const MDnsPayload &);
    ssize_t Unicast(int sock, sockaddr *saddr, const MDnsPayload &);
    const std::vector<int> &GetSockets() const;
    std::string_view GetIface(int sock) const;
    const sockaddr *GetSockAddr(int sock) const;
    void TriggerRefresh();

private:
    void Run();
    bool CanRefresh();
    void ReceiveInSock(int sock);
    void HandleSend(int sock);
    void OpenSocketV4(ifaddrs *ifa);
    void OpenSocketV6(ifaddrs *ifa);

    std::vector<int> socks_;
    std::map<int, std::string> iface_;
    std::map<int, sockaddr_storage> saddr_;
    std::atomic_bool runningFlag_ = false;
    int ctrlPair_[2] = {-1, -1};
    std::thread thread_;
    std::mutex mutex_;
    ReceiveHandler recv_;
    SendHandler refresh_;
};

} // namespace NetManagerStandard
} // namespace OHOS
#endif /* MDNS_SOCKET_LISTENER_H */
