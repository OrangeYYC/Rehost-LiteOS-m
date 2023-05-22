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

#ifndef WRAPPER_LISTENER_H
#define WRAPPER_LISTENER_H

#include <thread>

namespace OHOS {
namespace nmd {
class WrapperListener {
public:
    using RecvFunc = std::function<void(int32_t)>;
    WrapperListener(int32_t socketFd, RecvFunc recvFunc);
    WrapperListener() = delete;
    ~WrapperListener();

    /**
     * Start listen event message
     * @return NETMANAGER_SUCCESS suceess or NETMANAGER_ERROR failed
     */
    int32_t Start();

    /**
     * Stop listen event message
     * @return NETMANAGER_SUCCESS suceess or NETMANAGER_ERROR failed
     */
    int32_t Stop();

private:
    static constexpr int32_t BACK_LOG = 4;
    static constexpr int32_t PIPE_SHUTDOWN = 0;
    static constexpr int32_t PIPE_WAKEUP = 1;
    static constexpr uint32_t PIPE_SIZE = 2;
    int32_t socket_;
    std::thread thread_;
    std::mutex clientsLock_;
    int32_t pipe_[PIPE_SIZE] = {0};
    RecvFunc startReceiveFunc_;

    void Listen();
    static void ListenThread(WrapperListener *listener);
};
} // namespace nmd
} // namespace OHOS

#endif // WRAPPER_LISTENER_H
