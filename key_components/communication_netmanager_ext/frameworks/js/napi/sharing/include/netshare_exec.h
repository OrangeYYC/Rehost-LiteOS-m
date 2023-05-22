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

#ifndef COMMUNICATIONNETMANAGEREXT_NETSHARE_EXEC_H
#define COMMUNICATIONNETMANAGEREXT_NETSHARE_EXEC_H

#include <napi/native_api.h>
#include <netdb.h>

#include "netshare_issharing_context.h"
#include "netshare_startsharing_context.h"

namespace OHOS {
namespace NetManagerStandard {
class NetShareExec final {
public:
    DISALLOW_COPY_AND_MOVE(NetShareExec);

    NetShareExec() = delete;
    ~NetShareExec() = delete;

    static bool ExecIsSharingSupported(IsSharingSupportedContext *context);
    static napi_value IsSharingSupportedCallback(IsSharingSupportedContext *context);
    static bool ExecIsSharing(NetShareIsSharingContext *context);
    static napi_value IsSharingCallback(NetShareIsSharingContext *context);
    static bool ExecStartSharing(NetShareStartSharingContext *context);
    static napi_value StartSharingCallback(NetShareStartSharingContext *context);
    static bool ExecStopSharing(StopSharingContext *context);
    static napi_value StopSharingCallback(StopSharingContext *context);
    static bool ExecGetSharingIfaces(GetSharingIfacesContext *context);
    static napi_value GetSharingIfacesCallback(GetSharingIfacesContext *context);
    static bool ExecGetSharingState(GetSharingStateContext *context);
    static napi_value GetSharingStateCallback(GetSharingStateContext *context);
    static bool ExecGetSharableRegexes(GetSharableRegexesContext *context);
    static napi_value GetSharableRegexesCallback(GetSharableRegexesContext *context);
    static bool ExecGetStatsRxBytes(GetStatsRxBytesContext *context);
    static napi_value GetStatsRxBytesCallback(GetStatsRxBytesContext *context);
    static bool ExecGetStatsTxBytes(GetStatsTxBytesContext *context);
    static napi_value GetStatsTxBytesCallback(GetStatsTxBytesContext *context);
    static bool ExecGetStatsTotalBytes(GetStatsTotalBytesContext *context);
    static napi_value GetStatsTotalBytesCallback(GetStatsTotalBytesContext *context);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // COMMUNICATIONNETMANAGEREXT_NETSHARE_EXEC_H
