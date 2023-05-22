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

#ifndef COMMUNICATIONNETMANAGER_BASE_EVENT_LISTENER_H
#define COMMUNICATIONNETMANAGER_BASE_EVENT_LISTENER_H

#include <iosfwd>
#include <cstddef>
#include <string>

#include <napi/native_api.h>
#include <uv.h>

namespace OHOS {
namespace NetManagerStandard {
class EventListener {
public:
    EventListener() = delete;
    EventListener(const EventListener &listener);
    EventListener(napi_env env, std::string type, napi_value callback, bool once, bool asyncCallback);
    ~EventListener();

    void Emit(const std::string &eventType, size_t argc, napi_value *argv) const;
    void EmitByUv(const std::string &type, void *data, void(handler)(uv_work_t *, int status)) const;
    [[nodiscard]] bool Match(const std::string &type, napi_value callback) const;
    [[nodiscard]] bool MatchOnce(const std::string &type) const;
    [[nodiscard]] bool MatchType(const std::string &type) const;
    [[nodiscard]] bool IsAsyncCallback() const;
    [[nodiscard]] napi_env GetEnv() const;
    [[nodiscard]] napi_ref GetCallbackRef() const;

    EventListener &operator=(const EventListener &listener);

private:
    napi_env env_;
    std::string type_;
    napi_ref callbackRef_;
    bool once_;
    bool asyncCallback_;
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif // COMMUNICATIONNETMANAGER_BASE_EVENT_LISTENER_H
