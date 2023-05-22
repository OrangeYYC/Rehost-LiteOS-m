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

#ifndef NETSYS_ADDR_INFO_H
#define NETSYS_ADDR_INFO_H

#include <netdb.h>
#include <string>
#include <vector>

#include "message_parcel.h"
#include "parcel.h"

namespace OHOS {
namespace NetsysNative {
struct NetsysAddrInfoParcel final : public Parcelable {
public:
    NetsysAddrInfoParcel() = default;
    NetsysAddrInfoParcel(const addrinfo *addr, const uint16_t netId, const std::string Node,
                         const std::string ServData);
    ~NetsysAddrInfoParcel() = default;
    virtual bool Marshalling(Parcel &parcel) const override;
    static sptr<NetsysAddrInfoParcel> Unmarshalling(MessageParcel &parcel);

    addrinfo *addrHead;
    int32_t aiFamily;
    int32_t aiSocktype;
    int32_t aiFlags;
    int32_t aiProtocol;
    int32_t aiAddrlen;
    int32_t netId;
    int32_t ret;
    int32_t addrSize;
    int32_t isHintsNull;
    std::string hostName;
    std::string serverName;
};
} // namespace NetsysNative
} // namespace OHOS
#endif // NETSYS_ADDR_INFO_H
