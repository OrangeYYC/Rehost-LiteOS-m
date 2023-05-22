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

#include "netlink_manager.h"

#include <cerrno>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <map>
#include <mutex>
#include <sys/socket.h>
#include <unistd.h>

#include "netlink_define.h"
#include "netnative_log_wrapper.h"
#include "wrapper_distributor.h"

namespace OHOS {
namespace nmd {
using namespace NetlinkDefine;
namespace {
constexpr int32_t NFLOG_QUOTA_GROUP = 1;
constexpr int32_t UEVENT_GROUP = 0xffffffff;
struct DistributorParam {
    int32_t groups;
    int32_t format;
    bool flag;
};

const std::map<int32_t, DistributorParam> distributorParamList_ = {
    {NETLINK_KOBJECT_UEVENT, {UEVENT_GROUP, NETLINK_FORMAT_ASCII, false}},
    {NETLINK_ROUTE,
     {RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV6_IFADDR | RTMGRP_IPV4_ROUTE | RTMGRP_IPV6_ROUTE |
          (1 << (RTNLGRP_ND_USEROPT - 1)),
      NETLINK_FORMAT_BINARY, false}},
    {NETLINK_NFLOG, {NFLOG_QUOTA_GROUP, NETLINK_FORMAT_BINARY, false}},
    {NETLINK_NETFILTER, {0, NETLINK_FORMAT_BINARY_UNICAST, true}}};

std::map<int32_t, std::unique_ptr<WrapperDistributor>> distributorMap_;

bool CreateNetlinkDistributor(int32_t netlinkType, const DistributorParam &param)
{
    sockaddr_nl sockAddr;
    int32_t size = BUFFER_SIZE;
    int32_t on = 1;
    int32_t socketFd;

    sockAddr.nl_family = AF_NETLINK;
    sockAddr.nl_pid = 0;
    sockAddr.nl_groups = param.groups;

    if ((socketFd = socket(PF_NETLINK, SOCK_DGRAM | SOCK_CLOEXEC, netlinkType)) < 0) {
        NETNATIVE_LOGE("Creat socket for family failed NetLinkType is %{public}d: %{public}s = %{public}d",
                       netlinkType, strerror(errno), errno);
        return false;
    }

    if (setsockopt(socketFd, SOL_SOCKET, SO_RCVBUFFORCE, &size, sizeof(size)) < 0 &&
        setsockopt(socketFd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size)) < 0) {
        NETNATIVE_LOGE("Set buffer for revieve msg failed the error is : %{public}d, EMSG: %{public}s", errno,
                       strerror(errno));
        close(socketFd);
        return false;
    }

    if (setsockopt(socketFd, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on)) < 0) {
        NETNATIVE_LOGE("Uevent socket SO_PASSCRED set failed dump for this: %{public}d, EMSG: %{public}s", errno,
                       strerror(errno));
        close(socketFd);
        return false;
    }

    if (bind(socketFd, reinterpret_cast<sockaddr *>(&sockAddr), sizeof(sockAddr)) < 0) {
        NETNATIVE_LOGE("Bind netlink socket failed dumps is this : %{public}d, EMSG: %{public}s", errno,
                       strerror(errno));
        close(socketFd);
        return false;
    }
    NETNATIVE_LOGI("CreateNetlinkDistributor netlinkType: %{public}d, socketFd: %{public}d", netlinkType, socketFd);
    distributorMap_[netlinkType] = std::make_unique<WrapperDistributor>(socketFd, param.format);
    return true;
}
} // namespace

std::shared_ptr<std::vector<sptr<NetsysNative::INotifyCallback>>> NetlinkManager::callbacks_ =
    std::make_shared<std::vector<sptr<NetsysNative::INotifyCallback>>>();
NetlinkManager::NetlinkManager()
{
    for (const auto &it : distributorParamList_) {
        CreateNetlinkDistributor(it.first, it.second);
    }
    if (callbacks_ == nullptr) {
        callbacks_ = std::make_shared<std::vector<sptr<NetsysNative::INotifyCallback>>>();
    }
}

NetlinkManager::~NetlinkManager()
{
    callbacks_->clear();
    callbacks_ = nullptr;
}

int32_t NetlinkManager::StartListener()
{
    for (auto &it : distributorMap_) {
        if (it.second == nullptr) {
            continue;
        }
        it.second->RegisterNetlinkCallbacks(callbacks_);
        if (it.second->Start() != 0) {
            NETNATIVE_LOGE("Start netlink listener failed");
            return NetlinkResult::ERROR;
        }
    }
    return NetlinkResult::OK;
}

int32_t NetlinkManager::StopListener()
{
    for (auto &it : distributorMap_) {
        if (it.second == nullptr) {
            continue;
        }
        if (it.second->Stop() != 0) {
            NETNATIVE_LOGE("Stop netlink listener failed");
            return NetlinkResult::ERROR;
        }
    }
    return NetlinkResult::OK;
}

int32_t NetlinkManager::RegisterNetlinkCallback(sptr<NetsysNative::INotifyCallback> callback)
{
    if (callback == nullptr) {
        NETNATIVE_LOGE("callback is nullptr");
        return NetlinkResult::ERR_NULL_PTR;
    }
    for (const auto &cb : *callbacks_) {
        if (cb == callback) {
            NETNATIVE_LOGI("callback is already registered");
            return NetlinkResult::OK;
        }
    }
    callbacks_->push_back(callback);
    NETNATIVE_LOGI("callback is registered successfully current size is %{public}zu", callbacks_->size());
    return NetlinkResult::OK;
}

int32_t NetlinkManager::UnregisterNetlinkCallback(sptr<NetsysNative::INotifyCallback> callback)
{
    if (callback == nullptr) {
        NETNATIVE_LOGE("callback is nullptr");
        return NetlinkResult::ERR_NULL_PTR;
    }

    for (auto it = callbacks_->begin(); it != callbacks_->end(); ++it) {
        if (*it == callback) {
            callbacks_->erase(it);
            NETNATIVE_LOGI("callback is unregistered successfully");
            return NetlinkResult::OK;
        }
    }
    NETNATIVE_LOGI("callback has not registered current callback number is %{public}zu", callbacks_->size());
    return NetlinkResult::ERR_INVALID_PARAM;
}
} // namespace nmd
} // namespace OHOS
