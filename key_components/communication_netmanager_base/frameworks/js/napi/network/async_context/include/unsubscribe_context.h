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

#ifndef NETMANAGER_BASE_UNSUBSCRIBE_CONTEXT_H
#define NETMANAGER_BASE_UNSUBSCRIBE_CONTEXT_H

#include "napi/native_api.h"
#include "base_context.h"
#include "nocopyable.h"

namespace OHOS::NetManagerStandard {
class UnsubscribeContext final : public BaseContext {
public:
    DISALLOW_COPY_AND_MOVE(UnsubscribeContext);

    UnsubscribeContext() = delete;

    explicit UnsubscribeContext(napi_env env, EventManager *manager);

    void ParseParams(napi_value *params, size_t paramsCount);

private:
    bool CheckParamsType(napi_value *params, size_t paramsCount);
};
} // namespace OHOS::NetManagerStandard

#endif /* NETMANAGER_BASE_UNSUBSCRIBE_CONTEXT_H */
