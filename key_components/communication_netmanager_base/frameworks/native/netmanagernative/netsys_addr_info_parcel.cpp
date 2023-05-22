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

#include "netsys_addr_info_parcel.h"

#include <cstring>
#include <securec.h>

#include "netnative_log_wrapper.h"

namespace OHOS {
namespace NetsysNative {
NetsysAddrInfoParcel::NetsysAddrInfoParcel(const addrinfo *addr, const uint16_t netId, const std::string node,
                                           const std::string service)
    : addrHead(nullptr)
{
    if (addr == nullptr) {
        isHintsNull = 1;
        NETNATIVE_LOG_D("hints is nullptr");
    } else {
        isHintsNull = 0;
        aiFamily = addr->ai_family;
        aiSocktype = addr->ai_socktype;
        aiFlags = addr->ai_flags;
        aiProtocol = addr->ai_protocol;
        aiAddrlen = addr->ai_addrlen;
    }
    this->netId = netId;
    hostName = node;
    serverName = service;
}

bool NetsysAddrInfoParcel::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteInt16(isHintsNull)) {
        return false;
    }
    if (!isHintsNull) {
        if ((!parcel.WriteInt16(aiFamily)) || (!parcel.WriteInt16(aiSocktype)) || (!parcel.WriteInt16(aiFlags)) ||
            (!parcel.WriteInt16(aiProtocol))) {
            return false;
        }
    }
    if (!parcel.WriteInt16(netId) || !parcel.WriteString(hostName) || !parcel.WriteString(serverName)) {
        return false;
    }
    return true;
}

sptr<NetsysAddrInfoParcel> NetsysAddrInfoParcel::Unmarshalling(MessageParcel &parcelMsg)
{
    sptr<NetsysAddrInfoParcel> ptr = new (std::nothrow) NetsysAddrInfoParcel();
    if (ptr == nullptr) {
        return nullptr;
    }
    ptr->ret = parcelMsg.ReadInt32();
    ptr->addrSize = parcelMsg.ReadInt32();
    addrinfo *headNode = nullptr;
    addrinfo *nextNode = nullptr;
    int count = 0;
    int size = ptr->addrSize;
    while (size--) {
        addrinfo *node = static_cast<addrinfo *>(malloc(sizeof(addrinfo)));
        if (node == nullptr) {
            break;
        }
        node->ai_flags = parcelMsg.ReadInt16();
        node->ai_family = parcelMsg.ReadInt16();
        node->ai_socktype = parcelMsg.ReadInt16();
        node->ai_protocol = parcelMsg.ReadInt16();
        node->ai_addrlen = static_cast<socklen_t>(parcelMsg.ReadUint32());
        int16_t canSize = parcelMsg.ReadInt16();
        node->ai_canonname = nullptr;
        const uint8_t *buffer = canSize > 0 ? static_cast<const uint8_t *>(parcelMsg.ReadRawData(canSize)) : nullptr;
        if (buffer != nullptr) {
            node->ai_canonname = static_cast<char *>(calloc(sizeof(char), (canSize + 1)));
            if (memcpy_s(node->ai_canonname, canSize, buffer, canSize) != 0) {
                NETNATIVE_LOGE("memcpy_s faild");
                return nullptr;
            }
        }
        node->ai_addr = nullptr;
        const sockaddr *aiAddr = static_cast<sockaddr *>(const_cast<void *>(parcelMsg.ReadRawData(node->ai_addrlen)));

        if (aiAddr) {
            node->ai_addr = static_cast<sockaddr *>(calloc(1, node->ai_addrlen + 1));
            if (memcpy_s(node->ai_addr, node->ai_addrlen, aiAddr, node->ai_addrlen) != 0) {
                NETNATIVE_LOGE("memcpy_s faild");
                return nullptr;
            }
        }
        node->ai_next = nullptr;
        if (count == 0) {
            headNode = node;
            nextNode = headNode;
        } else {
            nextNode->ai_next = node;
            nextNode = node;
        }
        count++;
    }
    ptr->addrHead = headNode;
    return ptr;
}
} // namespace NetsysNative
} // namespace OHOS
