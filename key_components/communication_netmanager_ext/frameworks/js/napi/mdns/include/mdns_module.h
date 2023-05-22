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

#include <cstdint>
#include <string_view>

#include "module_template.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi_utils.h"

#include "constant.h"
#include "mdns_addlocalservice_context.h"
#include "mdns_async_work.h"
#include "mdns_removelocalservice_context.h"
#include "mdns_startsearching_context.h"
#include "netmgr_ext_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
class MDnsModule final {
public:
    static constexpr const char *FUNCTION_CREATE_DISCOVERY = "createDiscoveryService";
    static constexpr const char *FUNCTION_ADDLOCALSERVICE = "addLocalService";
    static constexpr const char *FUNCTION_REMOVELOCALSERVICE = "removeLocalService";
    static constexpr const char *FUNCTION_RESOLVELOCALSERVICE = "resolveLocalService";
    static constexpr const char *FUNCTION_DISCOVERY_SERVICE = "discoveryService";

    class DiscoveryServiceInterface final {
    public:
        static constexpr const char *FUNCTION_ON = "on";
        static constexpr const char *FUNCTION_STARTSEARCHINGMDNS = "startSearchingMDNS";
        static constexpr const char *FUNCTION_STOPSEARCHINGMDNS = "stopSearchingMDNS";

        static napi_value On(napi_env env, napi_callback_info info);
        static napi_value StartSearchingMDNS(napi_env env, napi_callback_info info);
        static napi_value StopSearchingMDNS(napi_env env, napi_callback_info info);
    };

    static napi_value InitMDnsModule(napi_env env, napi_value exports);
    static napi_value CreateDiscoveryService(napi_env env, napi_callback_info info);
    static napi_value AddLocalService(napi_env env, napi_callback_info info);
    static napi_value RemoveLocalService(napi_env env, napi_callback_info info);
    static napi_value ResolveLocalService(napi_env env, napi_callback_info info);
};
} // namespace NetManagerStandard
} // namespace OHOS
