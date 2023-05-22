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

#ifndef COMMUNICATIONNETMANAGERBASE_ISUIDNET_ALLOWED_CONTEXT_H
#define COMMUNICATIONNETMANAGERBASE_ISUIDNET_ALLOWED_CONTEXT_H

#include <cstddef>
#include <cstdint>
#include <string>

#include <napi/native_api.h>

#include "base_context.h"

namespace OHOS {
namespace NetManagerStandard {
class IsUidNetAllowedContext final : public BaseContext {
public:
    IsUidNetAllowedContext() = delete;
    explicit IsUidNetAllowedContext(napi_env env, EventManager *manager);

    void ParseParams(napi_value *params, size_t paramsCount);

public:
    int32_t uid_ = 0;
    bool isMetered_ = false;
    std::string iface_;
    bool isBoolean_ = false;
    bool isUidNet_ = false;

private:
    bool CheckParamsType(napi_value *params, size_t paramsCount);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // COMMUNICATIONNETMANAGERBASE_ISUIDNET_ALLOWED_CONTEXT_H
