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

#include "task_queue.h"

#include <memory>
#include <mutex>
#include <queue>

#include "netstack_log.h"

namespace OHOS::NetStack::Task {
class Task {
public:
    Task() : executor(nullptr), callback(nullptr), data(nullptr), priority_(TaskPriority::CLOSE) {}

    Task(TaskPriority priority, AsyncWorkExecutor exec, AsyncWorkCallback call, void *d)
        : executor(exec), callback(call), data(d), priority_(priority)
    {
    }

    ~Task() = default;

    bool operator<(const Task &e) const
    {
        return priority_ < e.priority_;
    }

    AsyncWorkExecutor executor;
    AsyncWorkCallback callback;
    void *data;

private:
    TaskPriority priority_;
};

std::priority_queue<Task> g_taskExecutorQueue; /* NOLINT */

std::priority_queue<Task> g_taskCallbackQueue; /* NOLINT */

std::mutex g_execMutex;

std::mutex g_callbackMutex;

void Executor(napi_env env, void *data)
{
    Task task;
    do {
        std::lock_guard<std::mutex> lock(g_execMutex);

        if (g_taskExecutorQueue.empty()) {
            NETSTACK_LOGI("g_taskExecutorQueue is empty");
            return;
        }
        task = g_taskExecutorQueue.top();
        g_taskExecutorQueue.pop();
    } while (false);

    auto context = static_cast<BaseContext *>(data);
    context->SetExecOK(true);
    if (task.executor && task.data) {
        task.executor(env, task.data);
    }

    std::lock_guard<std::mutex> lock(g_callbackMutex);
    g_taskCallbackQueue.push(task);
}

void Callback(napi_env env, napi_status status, void *data)
{
    (void)status;

    auto deleter = [](BaseContext *context) { delete context; };
    std::unique_ptr<BaseContext, decltype(deleter)> context(static_cast<BaseContext *>(data), deleter);

    if (!context->IsExecOK()) {
        NETSTACK_LOGI("new async work again to read the task queue");
        auto again = new BaseContext(env, nullptr);
        again->CreateAsyncWork(context->GetAsyncWorkName(), Executor, Callback);
    }

    Task task;
    do {
        std::lock_guard<std::mutex> lock(g_callbackMutex);

        if (g_taskCallbackQueue.empty()) {
            NETSTACK_LOGI("g_taskCallbackQueue is empty");
            return;
        }
        task = g_taskCallbackQueue.top();
        g_taskCallbackQueue.pop();
    } while (false);

    if (task.callback && task.data) {
        task.callback(env, napi_ok, task.data);
    }
}

void PushTask(TaskPriority priority, AsyncWorkExecutor executor, AsyncWorkCallback callback, void *data)
{
    std::lock_guard<std::mutex> lock(g_execMutex);

    g_taskExecutorQueue.push(Task(priority, executor, callback, data));
}
} // namespace OHOS::NetStack::Task
