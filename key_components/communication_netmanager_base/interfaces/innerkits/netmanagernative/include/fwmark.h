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

#ifndef INCLUDE_FWMARK_H
#define INCLUDE_FWMARK_H

#include <sys/un.h>

#include "network_permission.h"

namespace OHOS {
namespace nmd {
union Fwmark {
    uint32_t intValue;
    struct {
        uint16_t netId : 16;
        bool explicitlySelected : 1;
        bool protectedFromVpn : 1;
        NetworkPermission permission : 2;
        bool uidBillingDone : 1;
    };
    constexpr Fwmark() : intValue(0) {}
    static inline uint32_t GetUidBillingMask()
    {
        Fwmark m;
        m.uidBillingDone = true;
        return m.intValue;
    }
};
static constexpr uint32_t FWMARK_NET_ID_MASK = 0xffff;
static const struct sockaddr_un FWMARK_SERVER_PATH = {AF_UNIX, "/dev/unix/socket/fwmarkd"};
} // namespace nmd
} // namespace OHOS
#endif // INCLUDE_FWMARK_H
