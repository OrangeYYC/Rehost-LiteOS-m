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

#ifndef WRAPPER_DECODER_H
#define WRAPPER_DECODER_H

#include "netsys_event_message.h"

#include <functional>
#include <linux/rtnetlink.h>
#include <netinet/icmp6.h>

namespace OHOS {
namespace nmd {
class WrapperDecoder {
public:
    WrapperDecoder(std::shared_ptr<NetsysEventMessage> message);
    WrapperDecoder() = delete;
    ~WrapperDecoder() = default;

    /**
     * Decode Ascii event message
     * @param buffer message buffer
     * @param buffSize message buffer size
     * @return true if decode success, otherwise false
     */
    bool DecodeAscii(const char *buffer, int32_t buffSize);

    /**
     * Decode Binary event message
     * @param buffer message buffer
     * @param buffSize message buffer size
     * @return true if decode success, otherwise false
     */
    bool DecodeBinary(const char *buffer, int32_t buffSize);

private:
    static constexpr int32_t SPLIT_SIZE = 2;
    std::shared_ptr<NetsysEventMessage> message_ = nullptr;

    bool PushAsciiMessage(const std::vector<std::string> &recvmsg);
    bool InterpreteInfoMsg(const nlmsghdr *hdrMsg);
    bool InterpreteUlogMsg(const nlmsghdr *hdrMsg);
    bool InterpreteAddressMsg(const nlmsghdr *hdrMsg);
    bool InterpreteRtMsg(const nlmsghdr *hdrMsg);
    bool InterpreteIFaceAddr(ifaddrmsg *ifAddr, char *addrStr, socklen_t sockLen, const std::string &msgType,
                             char *ifName, rtattr *rta);
    bool SaveAddressMsg(const std::string addrStr, const ifaddrmsg *addrMsg, const std::string flags,
                        const ifa_cacheinfo *cacheInfo, const std::string ifname);
    bool SaveRtMsg(std::string dst, const std::string gateWay, const std::string device, int32_t length,
                   int32_t family);
    rtmsg *CheckRtParam(const nlmsghdr *hdrMsg, uint8_t type);
    void SaveOtherMsg(const std::string &info);
};
} // namespace nmd
} // namespace OHOS

#endif // WRAPPER_DECODER_H
