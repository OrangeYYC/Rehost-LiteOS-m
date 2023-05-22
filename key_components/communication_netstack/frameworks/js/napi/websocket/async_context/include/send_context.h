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

#ifndef COMMUNICATIONNETSTACK_SEND_CONTEXT_H
#define COMMUNICATIONNETSTACK_SEND_CONTEXT_H

#include <string>

#include "libwebsockets.h"
#include "base_context.h"
#include "nocopyable.h"

namespace OHOS::NetStack {
class SendContext final : public BaseContext {
public:
    DISALLOW_COPY_AND_MOVE(SendContext);

    SendContext() = delete;

    explicit SendContext(napi_env env, EventManager *manager);

    void ParseParams(napi_value *params, size_t paramsCount) override;

    [[nodiscard]] int32_t GetErrorCode() const override;

    [[nodiscard]] std::string GetErrorMessage() const override;

    void *data;

    size_t length;

    lws_write_protocol protocol;

private:
    bool CheckParamsType(napi_value *params, size_t paramsCount);
};
} // namespace OHOS::NetStack

#endif /* COMMUNICATIONNETSTACK_SEND_CONTEXT_H */
