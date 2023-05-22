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

#include "tcp_extra_options.h"

namespace OHOS::NetStack {
TCPExtraOptions::SocketLinger::SocketLinger() : on_(false), linger_(0) {}

void TCPExtraOptions::SocketLinger::SetOn(bool on)
{
    on_ = on;
}

void TCPExtraOptions::SocketLinger::SetLinger(uint32_t linger)
{
    linger_ = linger;
}

bool TCPExtraOptions::SocketLinger::IsOn() const
{
    return on_;
}

uint32_t TCPExtraOptions::SocketLinger::GetLinger() const
{
    return linger_;
}

TCPExtraOptions::TCPExtraOptions() : keepAlive_(false), OOBInline_(false), TCPNoDelay_(false) {}

void TCPExtraOptions::SetKeepAlive(bool keepAlive)
{
    keepAlive_ = keepAlive;
}

void TCPExtraOptions::SetOOBInline(bool OOBInline)
{
    OOBInline_ = OOBInline;
}

void TCPExtraOptions::SetTCPNoDelay(bool TCPNoDelay)
{
    TCPNoDelay_ = TCPNoDelay;
}

bool TCPExtraOptions::IsKeepAlive() const
{
    return keepAlive_;
}

bool TCPExtraOptions::IsOOBInline() const
{
    return OOBInline_;
}

bool TCPExtraOptions::IsTCPNoDelay() const
{
    return TCPNoDelay_;
}
} // namespace OHOS::NetStack