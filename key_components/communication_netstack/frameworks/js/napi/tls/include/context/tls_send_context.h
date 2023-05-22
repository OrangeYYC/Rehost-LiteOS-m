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

#ifndef TLS_CONTEXT_SEND_CONTEXT_H
#define TLS_CONTEXT_SEND_CONTEXT_H

#include <cstddef>
#include <string>

#include <napi/native_api.h>

#include "base_context.h"
#include "event_manager.h"
#include "tls.h"
#include "tls_socket.h"

namespace OHOS {
namespace NetStack {
class TLSSendContext final : public BaseContext {
public:
    TLSSendContext() = delete;
    explicit TLSSendContext(napi_env env, EventManager *manager);

public:
    std::string data_;
    int32_t errorNumber_ = 0;

public:
    void ParseParams(napi_value *params, size_t paramsCount);

private:
    bool CheckParamsType(napi_value *params, size_t paramsCount);
};
} // namespace NetStack
} // namespace OHOS
#endif // TLS_CONTEXT_SEND_CONTEXT_H
