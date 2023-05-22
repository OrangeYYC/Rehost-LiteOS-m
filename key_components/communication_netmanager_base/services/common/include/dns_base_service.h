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
#ifndef DNS_BASE_SERVICE_H
#define DNS_BASE_SERVICE_H

#include <vector>

#include "refbase.h"

#include "inet_addr.h"

namespace OHOS {
namespace NetManagerStandard {
class DnsBaseService : public virtual RefBase {
public:
    virtual int32_t GetAddressesByName(const std::string &hostName, int32_t netId,
                                       std::vector<INetAddr> &addrInfo) = 0;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // DNS_BASE_SERVICE_H