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

#include "wrapper_listener.h"

#include <cerrno>
#include <cstdlib>
#include <fcntl.h>
#include <memory>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include "netlink_define.h"
#include "netnative_log_wrapper.h"

namespace OHOS::nmd {
namespace {
constexpr int32_t PRET_SIZE = 2;
} // namespace

WrapperListener::WrapperListener(int32_t socket, std::function<void(int32_t)> recvFunc)
{
    socket_ = socket;
    startReceiveFunc_ = recvFunc;
}

WrapperListener::~WrapperListener()
{
    Stop();
}

int32_t WrapperListener::Start()
{
    if (socket_ < 0) {
        NETNATIVE_LOGE("listener socket_ < 0 %{public}d", socket_);
        return NetlinkResult::ERROR;
    }

    int pipeRet = pipe2(pipe_, O_CLOEXEC);
    if (pipeRet != 0) {
        NETNATIVE_LOGE("pipeRes = %{public}d, pipe create failed errno = %{public}d, %{public}s", pipeRet, errno,
                       strerror(errno));
        return NetlinkResult::ERROR;
    }
    thread_ = std::thread(WrapperListener::ListenThread, this);
    return NetlinkResult::OK;
}

int32_t WrapperListener::Stop()
{
    NETNATIVE_LOGI("WrapperListener: Stop");
    char pipe = PIPE_SHUTDOWN;
    if (TEMP_FAILURE_RETRY(write(pipe_[1], &pipe, sizeof(pipe))) != 1) {
        NETNATIVE_LOGE("write pipe failed errno = %{public}d, %{public}s", errno, strerror(errno));
        return NetlinkResult::ERROR;
    }

    if (thread_.joinable()) {
        thread_.join();
    }
    for (auto &pi : pipe_) {
        if (pi > 0) {
            close(pi);
        }
    }

    if (socket_ > -1) {
        close(socket_);
    }

    return NetlinkResult::OK;
}

void WrapperListener::ListenThread(WrapperListener *listener)
{
    listener->Listen();
}

void WrapperListener::Listen()
{
    if (startReceiveFunc_ == nullptr) {
        NETNATIVE_LOGE("startReceiveFunc_ is nullptr start listen failed");
        return;
    }
    while (true) {
        std::vector<pollfd> pollFds;
        std::unique_lock<std::mutex> lock(clientsLock_);
        pollFds.reserve(PRET_SIZE + 1);
        pollfd polfd;
        polfd.fd = pipe_[0];
        polfd.events = POLLIN;
        pollFds.emplace_back(polfd);
        polfd.fd = socket_;
        polfd.events = POLLIN;
        pollFds.emplace_back(polfd);
        int32_t ret = TEMP_FAILURE_RETRY(poll(pollFds.data(), pollFds.size(), -1));
        if (ret < 0) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        if (pollFds[0].revents & (POLLIN | POLLERR)) {
            char ctlp = PIPE_SHUTDOWN;
            TEMP_FAILURE_RETRY(read(pipe_[0], &ctlp, sizeof(ctlp)));
            if (ctlp == PIPE_SHUTDOWN) {
                break;
            }
            continue;
        }
        startReceiveFunc_(socket_);
    }
}
} // namespace OHOS::nmd
