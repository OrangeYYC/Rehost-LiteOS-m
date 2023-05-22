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

#ifndef NETMANAGER_BASE_NETWORK_OBSERVER_H
#define NETMANAGER_BASE_NETWORK_OBSERVER_H

#include "net_all_capabilities.h"
#include "net_conn_callback_stub.h"
#include "event_manager.h"
#include "napi_utils.h"

namespace OHOS::NetManagerStandard {
struct NetworkType {
    std::set<NetBearType> bearerTypes;
};

class NetworkObserver : public NetConnCallbackStub {
public:
    int32_t NetAvailable(sptr<NetHandle> &netHandle) override;

    int32_t NetCapabilitiesChange(sptr<NetHandle> &netHandle, const sptr<NetAllCapabilities> &netAllCap) override;

    int32_t NetConnectionPropertiesChange(sptr<NetHandle> &netHandle, const sptr<NetLinkInfo> &info) override;

    int32_t NetLost(sptr<NetHandle> &netHandle) override;

    int32_t NetUnavailable() override;

    int32_t NetBlockStatusChange(sptr<NetHandle> &netHandle, bool blocked) override;

    void SetManager(EventManager *manager);

private:
    template <napi_value (*MakeJsValue)(napi_env, NetworkType *)>
    static void CallbackTemplate(uv_work_t *work, int status)
    {
        (void)status;

        auto workWrapper = static_cast<UvWorkWrapper *>(work->data);
        napi_env env = workWrapper->env;
        auto closeScope = [env](napi_handle_scope scope) { NapiUtils::CloseScope(env, scope); };
        std::unique_ptr<napi_handle_scope__, decltype(closeScope)> scope(NapiUtils::OpenScope(env), closeScope);

        napi_value obj = MakeJsValue(env, static_cast<NetworkType *>(workWrapper->data));

        std::pair<napi_value, napi_value> arg = {NapiUtils::GetUndefined(workWrapper->env), obj};
        workWrapper->manager->Emit(workWrapper->type, arg);

        delete workWrapper;
        delete work;
    }

    EventManager *manager_;
};

extern std::map<EventManager *, sptr<NetworkObserver>> g_observerMap;
} // namespace OHOS::NetManagerStandard
#endif /* NETMANAGER_BASE_NETWORK_OBSERVER_H */
