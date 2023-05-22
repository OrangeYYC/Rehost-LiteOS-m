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

#ifndef DATA_RECEIVER_H
#define DATA_RECEIVER_H

#include <netinet/icmp6.h>

#include "netlink_define.h"
#include "netsys_event_message.h"
#include "wrapper_listener.h"

namespace OHOS {
namespace nmd {
class WrapperListener;
class DataReceiver {
public:
    using EventCallback = std::function<void(std::shared_ptr<NetsysEventMessage>)>;
    DataReceiver(int32_t socketFd, int32_t format);
    DataReceiver() = delete;
    ~DataReceiver() = default;

    /**
     * Register callback function to receive event message
     * @param callback function pointer
     */
    void RegisterCallback(EventCallback callback);

    /**
     * Start receive event message
     * @return success or failed
     */
    int32_t Start();

    /**
     * Stop receive event message
     * @return success or failed
     */
    int32_t Stop();

private:
    void StartReceive(int32_t socket);
    ssize_t ReceiveMessage(bool isRepair, uid_t &uid);
    int32_t socket_;
    int32_t format_;
    std::unique_ptr<WrapperListener> listener_;
    char buffer_[NetlinkDefine::BUFFER_SIZE] __attribute__((aligned(4))) = {0};
    EventCallback callback_;
};
} // namespace nmd
} // namespace OHOS

#endif // DATA_RECEIVER_H
