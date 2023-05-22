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

#ifndef TLS_EXTRA_CONTEXT_H
#define TLS_EXTRA_CONTEXT_H

#include <cstddef>
#include <cstdint>

#include <napi/native_api.h>

#include "base_context.h"
#include "tcp_extra_options.h"

namespace OHOS::NetStack {
class TLSSetExtraOptionsContext : public BaseContext {
public:
    TLSSetExtraOptionsContext() = delete;
    explicit TLSSetExtraOptionsContext(napi_env env, EventManager *manager);

public:
    void ParseParams(napi_value *params, size_t paramsCount) override;
    [[nodiscard]] int GetSocketFd() const;

public:
    TCPExtraOptions options_;
    int32_t errorNumber_ = 0;

private:
    bool CheckParamsType(napi_value *params, size_t paramsCount);
};
} // namespace OHOS::NetStack
#endif // TLS_EXTRA_CONTEXT_H
