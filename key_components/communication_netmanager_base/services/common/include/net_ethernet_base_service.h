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
#ifndef NET_ETHERNET_BASE_SERVICE_H
#define NET_ETHERNET_BASE_SERVICE_H

#include "refbase.h"

namespace OHOS {
namespace NetManagerStandard {
class NetEthernetBaseService : public virtual RefBase {
public:
    virtual int32_t ResetEthernetFactory() = 0;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_ETHERNET_BASE_SERVICE_H