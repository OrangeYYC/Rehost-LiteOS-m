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

#ifndef NETMANAGER_BASE_NET_STATS_CALLBACK_OBSERVER_H
#define NETMANAGER_BASE_NET_STATS_CALLBACK_OBSERVER_H

#include <cstdint>
#include <memory>
#include <utility>
#include <string>

#include <napi/native_api.h>
#include <napi/native_node_api.h>
#include <uv.h>

#include "event_manager.h"
#include "napi_utils.h"
#include "net_stats_callback_stub.h"

namespace OHOS {
namespace NetManagerStandard {
class StatisticsCallbackObserver : public NetStatsCallbackStub {
public:
    int32_t NetIfaceStatsChanged(const std::string &iface) override;
    int32_t NetUidStatsChanged(const std::string &iface, uint32_t uid) override;

private:
    template <napi_value (*MakeJsValue)(napi_env, void *)> static void CallbackTemplate(uv_work_t *work, int status)
    {
        (void)status;
        if (work == nullptr) {
            return;
        }
        auto workWrapper = static_cast<UvWorkWrapper *>(work->data);
        if (workWrapper == nullptr) {
            delete work;
            return;
        }
        napi_env env = workWrapper->env;
        auto closeScope = [env](napi_handle_scope scope) { NapiUtils::CloseScope(env, scope); };
        std::unique_ptr<napi_handle_scope__, decltype(closeScope)> scope(NapiUtils::OpenScope(env), closeScope);
        napi_value obj = MakeJsValue(env, workWrapper->data);
        std::pair<napi_value, napi_value> arg = {NapiUtils::GetUndefined(workWrapper->env), obj};
        workWrapper->manager->Emit(workWrapper->type, arg);
        delete workWrapper;
        delete work;
    }

    static napi_value CreateNetIfaceStatsChangedParam(napi_env env, void *data);
    static napi_value CreateNetUidStatsChangedParam(napi_env env, void *data);

    static void NetIfaceStatsChangedCallback(uv_work_t *work, int status);
    static void NetUidStatsChangedCallback(uv_work_t *work, int status);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETMANAGER_BASE_NET_STATS_CALLBACK_OBSERVER_H
