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

#ifndef COMMUNICATIONNETMANAGEREXT_MDNS_EXEC_H
#define COMMUNICATIONNETMANAGEREXT_MDNS_EXEC_H

#include <netdb.h>

#include "napi/native_api.h"
#include "nocopyable.h"

#include "mdns_addlocalservice_context.h"
#include "mdns_removelocalservice_context.h"
#include "mdns_resolvelocalservice_context.h"
#include "mdns_startsearching_context.h"

namespace OHOS {
namespace NetManagerStandard {
class MDnsExec final {
public:
    DISALLOW_COPY_AND_MOVE(MDnsExec);

    MDnsExec() = delete;
    ~MDnsExec() = delete;

    static bool ExecAddLocalService(MDnsAddLocalServiceContext *context);
    static bool ExecRemoveLocalService(MDnsRemoveLocalServiceContext *context);
    static bool ExecResolveLocalService(MDnsResolveLocalServiceContext *context);
    static bool ExecStartSearchingMDNS(MDnsStartSearchingContext *context);
    static bool ExecStopSearchingMDNS(MDnsStopSearchingContext *context);

    static napi_value AddLocalServiceCallback(MDnsAddLocalServiceContext *context);
    static napi_value RemoveLocalServiceCallback(MDnsRemoveLocalServiceContext *context);
    static napi_value ResolveLocalServiceCallback(MDnsResolveLocalServiceContext *context);
    static napi_value StartSearchingMDNSCallback(MDnsStartSearchingContext *context);
    static napi_value StopSearchingMDNSCallback(MDnsStopSearchingContext *context);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // COMMUNICATIONNETMANAGEREXT_MDNS_EXEC_H
