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

#ifndef INCLUDE_LOCAL_NETWORK_H
#define INCLUDE_LOCAL_NETWORK_H

#include <string>

#include "netsys_network.h"

namespace OHOS {
namespace nmd {
class LocalNetwork : public NetsysNetwork {
public:
    explicit LocalNetwork(uint16_t netId);
    virtual ~LocalNetwork() = default;

private:
    std::string GetNetworkType() const override
    {
        return "LOCAL";
    };
    int32_t AddInterface(std::string &interfaceName) override;
    int32_t RemoveInterface(std::string &interfaceName) override;
};
} // namespace nmd
} // namespace OHOS
#endif // INCLUDE_LOCAL_NETWORK_H
