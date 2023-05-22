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

#include "iptables_wrapper.h"

#include <unistd.h>

#include "net_manager_constants.h"
#include "netmanager_base_common_utils.h"
#include "netnative_log_wrapper.h"

namespace OHOS {
namespace nmd {
using namespace NetManagerStandard;
namespace {
constexpr int32_t IPTABLES_WAIT_FOR_TIME_MS = 1000;
constexpr const char *IPATBLES_CMD_PATH = "/system/bin/iptables";
} // namespace

IptablesWrapper::IptablesWrapper()
{
    isRunningFlag_ = true;
    if (access(IPATBLES_CMD_PATH, F_OK) == 0) {
        isIptablesSystemAccess_ = true;
    } else {
        isIptablesSystemAccess_ = false;
    }

    auto eventLoop = AppExecFwk::EventRunner::Create("IptablesWrapper");
    if (eventLoop == nullptr) {
        return;
    }
    handler_ = std::make_shared<AppExecFwk::EventHandler>(eventLoop);
}

IptablesWrapper::~IptablesWrapper()
{
    isRunningFlag_ = false;
}

void IptablesWrapper::ExecuteCommand(const std::string &command)
{
    if (CommonUtils::ForkExec(command) == NETMANAGER_ERROR) {
        NETNATIVE_LOGE("run exec faild, command=%{public}s", command.c_str());
    }
}

void IptablesWrapper::ExecuteCommandForRes(const std::string &command)
{
    std::unique_lock<std::mutex> lock(iptablesMutex_);
    if (CommonUtils::ForkExec(command, &result_) == NETMANAGER_ERROR) {
        NETNATIVE_LOGE("run exec faild, command=%{public}s", command.c_str());
    }
    conditionVarLock_.notify_one();
}

int32_t IptablesWrapper::RunCommand(const IpType &ipType, const std::string &command)
{
    NETNATIVE_LOG_D("IptablesWrapper::RunCommand, ipType:%{public}d, command:%{public}s", ipType, command.c_str());
    if (handler_ == nullptr) {
        NETNATIVE_LOGE("RunCommand failed! handler is nullptr");
        return NETMANAGER_ERROR;
    }
    std::string cmd = std::string(IPATBLES_CMD_PATH) + " " + command;
    std::function<void()> executeCommand = std::bind(&IptablesWrapper::ExecuteCommand, this, cmd);
    handler_->PostTask(executeCommand);
    return NetManagerStandard::NETMANAGER_SUCCESS;
}

std::string IptablesWrapper::RunCommandForRes(const IpType &ipType, const std::string &command)
{
    NETNATIVE_LOG_D("IptablesWrapper::RunCommandForRes, ipType:%{public}d, command:%{public}s", ipType,
                    command.c_str());
    if (handler_ == nullptr) {
        NETNATIVE_LOGE("RunCommandForRes failed! handler is nullptr");
        return result_;
    }
    std::string cmd = std::string(IPATBLES_CMD_PATH) + " " + command;
    std::unique_lock<std::mutex> lock(iptablesMutex_);
    std::function<void()> executeCommandForRes = std::bind(&IptablesWrapper::ExecuteCommandForRes, this, cmd);
    handler_->PostTask(executeCommandForRes);
    auto status = conditionVarLock_.wait_for(lock, std::chrono::milliseconds(IPTABLES_WAIT_FOR_TIME_MS));
    if (status == std::cv_status::timeout) {
        NETNATIVE_LOGI("ExecuteCommandForRes timeout!");
    }
    return result_;
}
} // namespace nmd
} // namespace OHOS
