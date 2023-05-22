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

#ifndef COMMUNICATIONNETSTACK_TCP_CONNECT_OPTIONS_H
#define COMMUNICATIONNETSTACK_TCP_CONNECT_OPTIONS_H

#include "net_address.h"

static constexpr const uint32_t DEFAULT_CONNECT_TIMEOUT = 5000; // 5 Seconds

namespace OHOS::NetStack {
class TcpConnectOptions final {
public:
    TcpConnectOptions();

    ~TcpConnectOptions() = default;

    void SetTimeout(uint32_t timeout);

    [[nodiscard]] uint32_t GetTimeout() const;

    NetAddress address;

private:
    uint32_t timeout_;
};
} // namespace OHOS::NetStack

#endif /* COMMUNICATIONNETSTACK_TCP_CONNECT_OPTIONS_H */
