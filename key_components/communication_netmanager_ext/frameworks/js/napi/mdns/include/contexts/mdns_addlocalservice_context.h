/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef NETMANAGER_EXT_ADDLOCALSERVICE_CONTEXT_H
#define NETMANAGER_EXT_ADDLOCALSERVICE_CONTEXT_H

#include <cstddef>
#include <cstdint>
#include <map>

#include "mdns_base_context.h"
#include "mdns_callback_observer.h"
#include "mdns_service_info.h"

namespace OHOS {
namespace NetManagerStandard {
class MDnsAddLocalServiceContext final : public MDnsBaseContext {
public:
    DISALLOW_COPY_AND_MOVE(MDnsAddLocalServiceContext);
    MDnsAddLocalServiceContext() = delete;
    explicit MDnsAddLocalServiceContext(napi_env env, EventManager *manager);

    void ParseParams(napi_value *params, size_t paramsCount);
    sptr<IRegistrationCallback> GetObserver() const;

    static std::map<std::string, sptr<IRegistrationCallback>> registerCallbackMap_;

private:
    bool CheckParamsType(napi_value *params, size_t paramsCount);
    sptr<IRegistrationCallback> regObserver_ = nullptr;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif /* NETMANAGER_EXT_ADDLOCALSERVICE_CONTEXT_H */
