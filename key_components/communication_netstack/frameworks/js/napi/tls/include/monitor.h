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

#ifndef TLS_MONITOR_H
#define TLS_MONITOR_H

#include <cstdint>
#include <set>
#include <string>
#include <string_view>

#include <napi/native_api.h>

#include "event_manager.h"
#include "singleton.h"
#include "socket_remote_info.h"
#include "tls.h"

namespace OHOS {
namespace NetStack {
class Monitor final {
    DECLARE_DELAYED_SINGLETON(Monitor);

public:
    napi_value On(napi_env env, napi_callback_info info);
    napi_value Off(napi_env env, napi_callback_info info);

public:
    std::string data_;
    SocketRemoteInfo remoteInfo_;
    int32_t errorNumber_ = 0;
    std::string errorString_;

private:
    EventManager *manager_ = nullptr;
    std::set<std::string_view> monitors_;
};
} // namespace NetStack
} // namespace OHOS
#endif // TLS_CONTEXT_MONITOR_CONTEXT_H
