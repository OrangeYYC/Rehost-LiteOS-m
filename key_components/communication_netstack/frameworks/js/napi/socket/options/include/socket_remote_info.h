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

#ifndef COMMUNICATIONNETSTACK_SOCKET_REMOTE_INFO_H
#define COMMUNICATIONNETSTACK_SOCKET_REMOTE_INFO_H

#include <cstdint>
#include <string>
#include <iosfwd>

#include "net_address.h"

namespace OHOS::NetStack {
class SocketRemoteInfo final {
public:
    SocketRemoteInfo();

    ~SocketRemoteInfo() = default;

    void SetAddress(const std::string &address);

    void SetFamily(sa_family_t family);

    void SetPort(uint16_t port);

    void SetSize(uint32_t size);

    [[nodiscard]] const std::string &GetAddress() const;

    [[nodiscard]] const std::string &GetFamily() const;

    [[nodiscard]] uint16_t GetPort() const;

    [[nodiscard]] uint32_t GetSize() const;

private:
    std::string address_;

    std::string family_;

    uint16_t port_;

    uint32_t size_;
};
} // namespace OHOS::NetStack

#endif /* COMMUNICATIONNETSTACK_SOCKET_REMOTE_INFO_H */
