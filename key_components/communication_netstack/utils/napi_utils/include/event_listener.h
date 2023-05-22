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

#ifndef COMMUNICATIONNETSTACK_EVENT_LISTENER_H
#define COMMUNICATIONNETSTACK_EVENT_LISTENER_H

#include <cstddef>
#include <string>
#include <iosfwd>

#include "napi/native_api.h"
#include "uv.h"

namespace OHOS::NetStack {
class EventListener {
public:
    EventListener() = delete;

    EventListener(const EventListener &listener);

    EventListener(napi_env env, std::string type, napi_value callback, bool once, bool asyncCallback);

    ~EventListener();

    EventListener &operator=(const EventListener &listener);

    void Emit(const std::string &eventType, size_t argc, napi_value *argv) const;

    [[nodiscard]] bool Match(const std::string &type, napi_value callback) const;

    [[nodiscard]] bool MatchOnce(const std::string &type) const;

    [[nodiscard]] bool MatchType(const std::string &type) const;

    [[nodiscard]] bool IsAsyncCallback() const;

    void EmitByUv(const std::string &type, void *data, void(Handler)(uv_work_t *, int status)) const;

    [[nodiscard]] napi_env GetEnv() const;

    [[nodiscard]] napi_ref GetCallbackRef() const;

private:
    napi_env env_;

    std::string type_;

    bool once_;

    napi_ref callbackRef_;

    bool asyncCallback_;
};
} // namespace OHOS::NetStack

#endif /* COMMUNICATIONNETSTACK_EVENT_LISTENER_H */
