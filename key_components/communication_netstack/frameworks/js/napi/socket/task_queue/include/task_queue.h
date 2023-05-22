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

#ifndef COMMUNICATIONNETSTACK_TASK_QUEUE_H
#define COMMUNICATIONNETSTACK_TASK_QUEUE_H

#include "base_context.h"

namespace OHOS::NetStack::Task {
enum class TaskPriority {
    CLOSE,
    SEND,
    CONNECT,
    BIND,
    SET_OPTIONS,
};

void Executor(napi_env env, void *data);

void Callback(napi_env env, napi_status status, void *data);

void PushTask(TaskPriority priority, AsyncWorkExecutor executor, AsyncWorkCallback callback, void *data);
} // namespace OHOS::NetStack::Task

#endif /* COMMUNICATIONNETSTACK_TASK_QUEUE_H */
