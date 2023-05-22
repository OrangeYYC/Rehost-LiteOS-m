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

#include "net_conn_event_handler.h"

namespace OHOS {
namespace NetManagerStandard {
NetConnEventHandler::NetConnEventHandler(const std::shared_ptr<AppExecFwk::EventRunner> &runner)
    : AppExecFwk::EventHandler(runner)
{
}

bool NetConnEventHandler::PostAsyncTask(const Callback &callback, int64_t delayTime)
{
    return AppExecFwk::EventHandler::PostTask(callback, delayTime);
}

bool NetConnEventHandler::PostAsyncTask(const Callback &callback, const std::string &name, int64_t delayTime)
{
    return AppExecFwk::EventHandler::PostTask(callback, name, delayTime);
}

void NetConnEventHandler::RemoveAsyncTask(const std::string &name)
{
    AppExecFwk::EventHandler::RemoveTask(name);
}
} // namespace NetManagerStandard
} // namespace OHOS