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

#include "event_manager.h"

#include "netstack_log.h"
#include <algorithm>

namespace OHOS::NetStack {
static constexpr const int CALLBACK_PARAM_NUM = 1;

static constexpr const int ASYNC_CALLBACK_PARAM_NUM = 2;

EventManager::EventManager() : data_(nullptr), isValid_(true) {}

EventManager::~EventManager()
{
    NETSTACK_LOGI("EventManager is destructed by the destructor");
}

bool EventManager::IsManagerValid() const
{
    return isValid_;
}

void EventManager::SetInvalid()
{
    isValid_ = false;
}

void EventManager::AddListener(napi_env env, const std::string &type, napi_value callback, bool once,
                               bool asyncCallback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = std::remove_if(listeners_.begin(), listeners_.end(),
                             [type](const EventListener &listener) -> bool { return listener.MatchType(type); });
    if (it != listeners_.end()) {
        listeners_.erase(it, listeners_.end());
    }

    listeners_.emplace_back(EventListener(env, type, callback, once, asyncCallback));
}

void EventManager::DeleteListener(const std::string &type, napi_value callback)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it =
        std::remove_if(listeners_.begin(), listeners_.end(), [type, callback](const EventListener &listener) -> bool {
            return listener.Match(type, callback);
        });
    listeners_.erase(it, listeners_.end());
}

void EventManager::Emit(const std::string &type, const std::pair<napi_value, napi_value> &argv)
{
    std::for_each(listeners_.begin(), listeners_.end(), [type, argv](const EventListener &listener) {
        if (listener.IsAsyncCallback()) {
            /* AsyncCallback(BusinessError error, T data) */
            napi_value arg[ASYNC_CALLBACK_PARAM_NUM] = {argv.first, argv.second};
            listener.Emit(type, ASYNC_CALLBACK_PARAM_NUM, arg);
        } else {
            /* Callback(T data) */
            napi_value arg[CALLBACK_PARAM_NUM] = {argv.second};
            listener.Emit(type, CALLBACK_PARAM_NUM, arg);
        }
    });

    auto it = std::remove_if(listeners_.begin(), listeners_.end(),
                             [type](const EventListener &listener) -> bool { return listener.MatchOnce(type); });
    listeners_.erase(it, listeners_.end());
}

void EventManager::SetData(void *data)
{
    std::lock_guard<std::mutex> lock(mutex_);
    data_ = data;
}

void *EventManager::GetData()
{
    std::lock_guard<std::mutex> lock(mutex_);
    return data_;
}

void EventManager::EmitByUv(const std::string &type, void *data, void(Handler)(uv_work_t *, int status))
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!IsManagerValid()) {
        return;
    }

    std::for_each(listeners_.begin(), listeners_.end(), [type, data, Handler, this](const EventListener &listener) {
        auto workWrapper = new UvWorkWrapper(data, listener.GetEnv(), type, this);
        listener.EmitByUv(type, workWrapper, Handler);
    });
}

bool EventManager::HasEventListener(const std::string &type)
{
    std::lock_guard<std::mutex> lock(mutex_);

    return std::any_of(listeners_.begin(), listeners_.end(),
                       [&type](const EventListener &listener) -> bool { return listener.MatchType(type); });
}

void EventManager::DeleteListener(const std::string &type)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = std::remove_if(listeners_.begin(), listeners_.end(),
                             [type](const EventListener &listener) -> bool { return listener.MatchType(type); });
    listeners_.erase(it, listeners_.end());
}

UvWorkWrapper::UvWorkWrapper(void *theData, napi_env theEnv, std::string eventType, EventManager *eventManager)
    : data(theData), env(theEnv), type(std::move(eventType)), manager(eventManager)
{
}
} // namespace OHOS::NetStack
