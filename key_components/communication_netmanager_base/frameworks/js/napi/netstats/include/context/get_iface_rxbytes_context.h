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

#ifndef NETMANAGER_BASE_TSTATSGETIFACERXBYTESE_CONTEXT_H
#define NETMANAGER_BASE_TSTATSGETIFACERXBYTESE_CONTEXT_H

#include <cstddef>
#include <cstdint>
#include <string>

#include <napi/native_api.h>

#include "base_context.h"

namespace OHOS {
namespace NetManagerStandard {
class GetIfaceRxBytesContext final : public BaseContext {
public:
    GetIfaceRxBytesContext() = delete;
    explicit GetIfaceRxBytesContext(napi_env env, EventManager *manager);
    void ParseParams(napi_value *params, size_t paramsCount);
    void SetNic(std::string nic);
    void SetBytes64(int64_t bytes64);
    void SetInterfaceName(std::string interfaceName);
    std::string nic_;
    uint64_t bytes64_ = 0;
    std::string interfaceName_;

private:
    bool CheckParamsType(napi_value *params, size_t paramsCount);
};

using GetIfaceTxBytesContext = GetIfaceRxBytesContext;
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETMANAGER_BASE_TSTATSGETIFACERXBYTESE_CONTEXT_H
