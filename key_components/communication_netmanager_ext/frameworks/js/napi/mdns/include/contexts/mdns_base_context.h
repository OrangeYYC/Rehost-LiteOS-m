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

#ifndef NETMANAGER_EXT_MDNS_BASE_CONTEXT_H
#define NETMANAGER_EXT_MDNS_BASE_CONTEXT_H

#include "base_context.h"
#include "event_manager.h"

#include "mdns_callback_observer.h"
#include "mdns_service_info.h"

namespace OHOS {
namespace NetManagerStandard {
class MDnsBaseContext : public BaseContext {
public:
    MDnsBaseContext() = delete;
    explicit MDnsBaseContext(napi_env env, EventManager *manager);

    MDnsServiceInfo &GetServiceInfo();
    void SetServiceInfo(const MDnsServiceInfo &info);
    void ParseServiceInfo(napi_env env, napi_value value);
    void ParseAddressObj(napi_env env, napi_value obj);
    void ParseAttributeObj(napi_env env, napi_value obj, TxtRecord &attrMap);
    bool GetAttributeObj(napi_env env, napi_value obj, uint32_t &len);
    std::string GetContextIdString(napi_env env, napi_value obj);

private:
    MDnsServiceInfo serviceInfo_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif /* NETMANAGER_EXT_MDNS_BASE_CONTEXT_H */
