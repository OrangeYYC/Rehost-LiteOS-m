/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef NET_EXT_NAPI_IS_IFACE_CONTEXT_H
#define NET_EXT_NAPI_IS_IFACE_CONTEXT_H

#include <cstddef>
#include <cstdint>
#include <string>

#include <napi/native_api.h>

#include "base_context.h"
#include "event_manager.h"

namespace OHOS {
namespace NetManagerStandard {
class IsIfaceActiveContext : public BaseContext {
public:
    IsIfaceActiveContext() = delete;
    explicit IsIfaceActiveContext(napi_env env, EventManager *manager);

    void ParseParams(napi_value *params, size_t paramsCount);

public:
    std::string iface_;
    int32_t ifActivate_ = 0;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_EXT_NAPI_IS_IFACE_CONTEXT_H
