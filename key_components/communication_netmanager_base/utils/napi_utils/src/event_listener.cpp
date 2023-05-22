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

#include "event_listener.h"

#include "netmanager_base_log.h"
#include "napi_utils.h"
#include "securec.h"

namespace OHOS {
namespace NetManagerStandard {
EventListener::EventListener(napi_env env, std::string type, napi_value callback, bool once, bool asyncCallback)
    : env_(env),
      type_(std::move(type)),
      callbackRef_(NapiUtils::CreateReference(env, callback)),
      once_(once),
      asyncCallback_(asyncCallback)
{
}

EventListener::EventListener(const EventListener &listener)
{
    env_ = listener.env_;
    type_ = listener.type_;
    once_ = listener.once_;
    asyncCallback_ = listener.asyncCallback_;

    if (listener.callbackRef_ == nullptr) {
        callbackRef_ = nullptr;
        return;
    }
    napi_value callback = NapiUtils::GetReference(listener.env_, listener.callbackRef_);
    callbackRef_ = NapiUtils::CreateReference(env_, callback);
}

EventListener::~EventListener()
{
    NETMANAGER_BASE_LOGI("~EventListener() is called, to delete EventListener");
    if (callbackRef_ != nullptr) {
        NapiUtils::DeleteReference(env_, callbackRef_);
    }
    callbackRef_ = nullptr;
}

EventListener &EventListener::operator=(const EventListener &listener)
{
    env_ = listener.env_;
    type_ = listener.type_;
    once_ = listener.once_;
    asyncCallback_ = listener.asyncCallback_;

    if (listener.callbackRef_ == nullptr) {
        callbackRef_ = nullptr;
        return *this;
    }
    napi_value callback = NapiUtils::GetReference(listener.env_, listener.callbackRef_);
    callbackRef_ = NapiUtils::CreateReference(env_, callback);
    return *this;
}

void EventListener::Emit(const std::string &eventType, size_t argc, napi_value *argv) const
{
    if (type_ != eventType || callbackRef_ == nullptr) {
        return;
    }

    napi_value callback = NapiUtils::GetReference(env_, callbackRef_);
    if (NapiUtils::GetValueType(env_, callback) == napi_function) {
        (void)NapiUtils::CallFunction(env_, NapiUtils::GetUndefined(env_), callback, argc, argv);
    }
}

bool EventListener::Match(const std::string &type, napi_value callback) const
{
    if (type_ != type) {
        return false;
    }

    napi_value callback1 = NapiUtils::GetReference(env_, callbackRef_);
    bool ret = false;
    NAPI_CALL_BASE(env_, napi_strict_equals(env_, callback1, callback, &ret), false);
    return ret;
}

bool EventListener::MatchOnce(const std::string &type) const
{
    return (type_ == type) ? once_ : false;
}

bool EventListener::MatchType(const std::string &type) const
{
    return type_ == type;
}

bool EventListener::IsAsyncCallback() const
{
    return asyncCallback_;
}

void EventListener::EmitByUv(const std::string &type, void *data, void(handler)(uv_work_t *, int status)) const
{
    NETMANAGER_BASE_LOGI("EventListener::EmitByUv() is called, to EmitByUv");

    if (type_ != type || callbackRef_ == nullptr) {
        return;
    }

    NapiUtils::CreateUvQueueWork(env_, data, handler);
}

napi_env EventListener::GetEnv() const
{
    return env_;
}

napi_ref EventListener::GetCallbackRef() const
{
    return callbackRef_;
}
} // namespace NetManagerStandard
} // namespace OHOS
