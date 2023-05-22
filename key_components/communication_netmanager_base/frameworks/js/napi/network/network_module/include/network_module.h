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

#ifndef NETMANAGER_BASE_NETWORK_MODULE_H
#define NETMANAGER_BASE_NETWORK_MODULE_H

#include <map>

#include "event_manager.h"
#include "napi_utils.h"

namespace OHOS::NetManagerStandard {
class NetworkModule {
public:
    static constexpr const char *FUNCTION_GET_TYPE = "getType";
    static constexpr const char *FUNCTION_SUBSCRIBE = "subscribe";
    static constexpr const char *FUNCTION_UNSUBSCRIBE = "unsubscribe";

    static napi_value InitNetworkModule(napi_env env, napi_value exports);

private:
    static napi_value GetType(napi_env env, napi_callback_info info);

    static napi_value Subscribe(napi_env env, napi_callback_info info);

    static napi_value Unsubscribe(napi_env env, napi_callback_info info);
};
} // namespace OHOS::NetManagerStandard

#endif /* NETMANAGER_BASE_NETWORK_MODULE_H */
