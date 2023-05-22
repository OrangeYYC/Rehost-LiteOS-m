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

#include "socket_remote_info.h"

#include <string>
#include "sys/socket.h"

namespace OHOS::NetStack {
SocketRemoteInfo::SocketRemoteInfo() : port_(0), size_(0) {}

void SocketRemoteInfo::SetAddress(const std::string &address)
{
    address_ = address;
}

void SocketRemoteInfo::SetFamily(sa_family_t family)
{
    if (family == AF_INET) {
        family_ = "IPv4";
    } else if (family == AF_INET6) {
        family_ = "IPv6";
    } else {
        family_ = "Others";
    }
}

void SocketRemoteInfo::SetPort(uint16_t port)
{
    port_ = port;
}

void SocketRemoteInfo::SetSize(uint32_t size)
{
    size_ = size;
}

const std::string &SocketRemoteInfo::GetAddress() const
{
    return address_;
}

const std::string &SocketRemoteInfo::GetFamily() const
{
    return family_;
}

uint16_t SocketRemoteInfo::GetPort() const
{
    return port_;
}

uint32_t SocketRemoteInfo::GetSize() const
{
    return size_;
}
} // namespace OHOS::NetStack