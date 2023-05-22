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

#include "extra_options_base.h"

namespace OHOS::NetStack {
ExtraOptionsBase::ExtraOptionsBase()
    : receiveBufferSize_(0), sendBufferSize_(0), reuseAddress_(false), socketTimeout_(0)
{
}

void ExtraOptionsBase::SetReceiveBufferSize(uint32_t receiveBufferSize)
{
    receiveBufferSize_ = receiveBufferSize;
}

void ExtraOptionsBase::SetSendBufferSize(uint32_t sendBufferSize)
{
    sendBufferSize_ = sendBufferSize;
}

void ExtraOptionsBase::SetReuseAddress(bool reuseAddress)
{
    reuseAddress_ = reuseAddress;
}

void ExtraOptionsBase::SetSocketTimeout(uint32_t socketTimeout)
{
    socketTimeout_ = socketTimeout;
}

uint32_t ExtraOptionsBase::GetReceiveBufferSize() const
{
    return receiveBufferSize_;
}

uint32_t ExtraOptionsBase::GetSendBufferSize() const
{
    return sendBufferSize_;
}

bool ExtraOptionsBase::IsReuseAddress() const
{
    return reuseAddress_;
}

uint32_t ExtraOptionsBase::GetSocketTimeout() const
{
    return socketTimeout_;
}
} // namespace OHOS::NetStack
