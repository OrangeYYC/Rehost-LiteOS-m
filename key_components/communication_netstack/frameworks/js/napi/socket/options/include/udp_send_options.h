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

#ifndef COMMUNICATIONNETSTACK_UDP_SEND_OPTIONS_H
#define COMMUNICATIONNETSTACK_UDP_SEND_OPTIONS_H

#include <cstddef>
#include <iosfwd>
#include <string>

#include "net_address.h"

namespace OHOS::NetStack {
class UDPSendOptions final {
public:
    UDPSendOptions() = default;

    ~UDPSendOptions() = default;

    void SetData(const std::string &data);

    void SetData(void *data, size_t size);

    [[nodiscard]] const std::string &GetData() const;

    NetAddress address;

private:
    std::string data_;
};
} // namespace OHOS::NetStack

#endif /* COMMUNICATIONNETSTACK_UDP_SEND_OPTIONS_H */
