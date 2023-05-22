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

#include "data_receiver.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "netlink_define.h"
#include "netnative_log_wrapper.h"
#include "wrapper_decoder.h"

namespace OHOS {
namespace nmd {
using namespace NetlinkDefine;
DataReceiver::DataReceiver(int32_t socketFd, int32_t format) : socket_(socketFd), format_(format)
{
    listener_ = std::make_unique<WrapperListener>(socketFd, [this](int32_t socket) { this->StartReceive(socket); });
}

void DataReceiver::RegisterCallback(std::function<void(std::shared_ptr<NetsysEventMessage>)> callback)
{
    callback_ = callback;
}

int32_t DataReceiver::Start()
{
    return listener_->Start();
}

int32_t DataReceiver::Stop()
{
    return listener_->Stop();
}

void DataReceiver::StartReceive(int32_t socket)
{
    socket_ = socket;
    uid_t uid;
    bool isRepair = format_ == NETLINK_FORMAT_BINARY_UNICAST;
    ssize_t recvTimes = TEMP_FAILURE_RETRY(ReceiveMessage(isRepair, uid));
    if (recvTimes < 0) {
        return;
    }

    bool isSuccess = false;
    std::shared_ptr<NetsysEventMessage> message = std::make_shared<NetsysEventMessage>();
    std::unique_ptr<WrapperDecoder> decoder = std::make_unique<WrapperDecoder>(message);
    if (format_ == NETLINK_FORMAT_BINARY || format_ == NETLINK_FORMAT_BINARY_UNICAST) {
        isSuccess = decoder->DecodeBinary(buffer_, recvTimes);
    } else {
        isSuccess = decoder->DecodeAscii(buffer_, recvTimes);
    }
    if (isSuccess && callback_) {
        callback_(message);
    }
}

ssize_t DataReceiver::ReceiveMessage(bool isRepair, uid_t &uid)
{
    iovec iov = {buffer_, sizeof(buffer_)};
    sockaddr_nl addr;
    char control[CMSG_SPACE(sizeof(ucred))];
    msghdr hdr;
    hdr.msg_name = &addr;
    hdr.msg_namelen = sizeof(addr);
    hdr.msg_iov = &iov;
    hdr.msg_iovlen = 1;
    hdr.msg_control = control;
    hdr.msg_controllen = sizeof(control);
    hdr.msg_flags = 0;
    ssize_t count = TEMP_FAILURE_RETRY(recvmsg(socket_, &hdr, 0));
    if (count < 0) {
        NETNATIVE_LOGE("recvmsg message failed %{public}d, %{public}s", errno, strerror(errno));
        return count;
    }

    cmsghdr *cmsgHeader = CMSG_FIRSTHDR(&hdr);
    if (cmsgHeader == nullptr || cmsgHeader->cmsg_type != SCM_CREDENTIALS) {
        NETNATIVE_LOGE("cmsg_type is not SCM_CREDENTIALS (%{public}d)", cmsgHeader == nullptr);
        bzero(buffer_, sizeof(buffer_));
        errno = EIO;
        return NetlinkResult::ERROR;
    }

    ucred *cred = reinterpret_cast<ucred *>(CMSG_DATA(cmsgHeader));
    uid = cred->uid;
    if (addr.nl_pid != 0 || (isRepair && addr.nl_groups == 0)) {
        bzero(buffer_, sizeof(buffer_));
        errno = EIO;
        return NetlinkResult::ERROR;
    }

    return count;
}
} // namespace nmd
} // namespace OHOS
