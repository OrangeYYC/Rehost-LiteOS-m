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

#include "udp_send_options.h"

namespace OHOS::NetStack {
void UDPSendOptions::SetData(const std::string &data)
{
    data_ = data;
}

void UDPSendOptions::SetData(void *data, size_t size)
{
    data_.append(static_cast<char *>(data), size);
}

const std::string &UDPSendOptions::GetData() const
{
    return data_;
}
} // namespace OHOS::NetStack