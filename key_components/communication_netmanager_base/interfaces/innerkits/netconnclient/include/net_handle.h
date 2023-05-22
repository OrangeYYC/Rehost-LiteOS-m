/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef NET_HANDLE_H
#define NET_HANDLE_H

#include <string>
#include <list>

#include "parcel.h"
#include "singleton.h"

#include "inet_addr.h"

namespace OHOS {
namespace NetManagerStandard {
class NetHandle : public virtual RefBase {
public:
    NetHandle() : netId_(0) {}
    explicit NetHandle(int32_t netId) : netId_(netId) {}
    ~NetHandle() override = default;

    int32_t BindSocket(int32_t socket_fd);
    int32_t GetAddressesByName(const std::string &host, std::vector<INetAddr> &addrList);
    int32_t GetAddressByName(const std::string &host, INetAddr &addr);

    void SetNetId(int32_t netId)
    {
        netId_ = netId;
    }

    int32_t GetNetId() const
    {
        return netId_;
    }

private:
    int32_t netId_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_HANDLE_H