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

#ifndef NETMANAGER_EXT_SHARE_JUSTCALLBACK_CONTEXT_H
#define NETMANAGER_EXT_SHARE_JUSTCALLBACK_CONTEXT_H

#include <cstddef>
#include <cstdint>

#include <napi/native_api.h>

#include "base_context.h"
#include "networkshare_constants.h"

namespace OHOS {
namespace NetManagerStandard {
class NetShareIsSharingContext final : public BaseContext {
public:
    DISALLOW_COPY_AND_MOVE(NetShareIsSharingContext);

    NetShareIsSharingContext() = delete;
    explicit NetShareIsSharingContext(napi_env env, EventManager *manager);

    void ParseParams(napi_value *params, size_t paramsCount);
    void SetBytes32(int32_t bytes32);
    void SetSharingSupported(int32_t supported);
    void SetSharing(int32_t sharingStatus);
    int32_t GetBytes32() const;
    int32_t GetSharingSupported() const;
    int32_t GetSharing() const;

private:
    int32_t supported_ = NETWORKSHARE_IS_SUPPORTED;
    int32_t sharingStatus_ = NETWORKSHARE_IS_SHARING;
    int32_t bytes32_ = 0;

private:
    bool CheckParamsType(napi_value *params, size_t paramsCount);
};

using IsSharingSupportedContext = NetShareIsSharingContext;
using GetStatsTotalBytesContext = NetShareIsSharingContext;
using GetStatsRxBytesContext = NetShareIsSharingContext;
using GetStatsTxBytesContext = NetShareIsSharingContext;
} // namespace NetManagerStandard
} // namespace OHOS
#endif /* NETMANAGER_EXT_SHARE_JUSTCALLBACK_CONTEXT_H */
