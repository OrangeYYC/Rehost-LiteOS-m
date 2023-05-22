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

#ifndef NETMANAGER_EXT_MDNS_CALLBACK_OBSERVER_H
#define NETMANAGER_EXT_MDNS_CALLBACK_OBSERVER_H

#include <condition_variable>
#include <mutex>
#include <string>

#include "event_manager.h"
#include "iremote_stub.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_utils.h"

#include "i_mdns_event.h"
#include "mdns_event_stub.h"
#include "mdns_service_info.h"

namespace OHOS {
namespace NetManagerStandard {

class MDnsRegistrationObserver : public RegistrationCallbackStub {
public:
    MDnsRegistrationObserver() = default;
    ~MDnsRegistrationObserver() = default;
    void HandleRegister(const MDnsServiceInfo &serviceInfo, int32_t retCode) override;
    void HandleUnRegister(const MDnsServiceInfo &serviceInfo, int32_t retCode) override;
    void HandleRegisterResult(const MDnsServiceInfo &serviceInfo, int32_t retCode) override;
};

class MDnsDiscoveryObserver : public DiscoveryCallbackStub {
public:
    MDnsDiscoveryObserver() = default;
    ~MDnsDiscoveryObserver() = default;
    void HandleStartDiscover(const MDnsServiceInfo &serviceInfo, int32_t retCode) override;
    void HandleStopDiscover(const MDnsServiceInfo &serviceInfo, int32_t retCode) override;
    void HandleServiceFound(const MDnsServiceInfo &serviceInfo, int32_t retCode) override;
    void HandleServiceLost(const MDnsServiceInfo &serviceInfo, int32_t retCode) override;

    void EmitStartDiscover(const MDnsServiceInfo &serviceInfo, int32_t retCode);
    void EmitStopDiscover(const MDnsServiceInfo &serviceInfo, int32_t retCode);

private:
    template <napi_value (*MakeJsValue)(napi_env, void *)> static void CallbackTemplate(uv_work_t *work, int32_t status)
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
    static napi_value CreateStartDiscoveryService(napi_env env, void *data);
    static void StartDiscoveryServiceCallback(uv_work_t *work, int32_t status);

    static napi_value CreateStopDiscoveryService(napi_env env, void *data);
    static void StopDiscoveryServiceCallback(uv_work_t *work, int32_t status);

    static napi_value CreateServiceFound(napi_env env, void *data);
    static void ServiceFoundCallback(uv_work_t *work, int32_t status);

    static napi_value CreateServiceLost(napi_env env, void *data);
    static void ServiceLostCallback(uv_work_t *work, int32_t status);
};

class MDnsResolveObserver : public ResolveCallbackStub {
public:
    MDnsResolveObserver() = default;
    ~MDnsResolveObserver() = default;
    void HandleResolveResult(const MDnsServiceInfo &serviceInfo, int32_t retCode) override;

    std::mutex mutex_;
    std::condition_variable cv_;
    bool resolved_ = false;
    MDnsServiceInfo serviceInfo_;
    int32_t retCode_ = NET_MDNS_ERR_UNKNOWN;
};

} // namespace NetManagerStandard
} // namespace OHOS
#endif // NETMANAGER_EXT_MDNS_CALLBACK_OBSERVER_H
