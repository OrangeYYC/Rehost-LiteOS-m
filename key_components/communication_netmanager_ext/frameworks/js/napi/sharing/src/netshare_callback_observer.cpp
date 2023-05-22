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

#include "netshare_callback_observer.h"

#include <vector>

#include "constant.h"
#include "netmanager_ext_log.h"
#include "net_manager_ext_constants.h"
#include "netshare_observer_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
namespace {
struct SharingState {
    SharingIfaceType type;
    std::string iface;
    SharingIfaceState state;
};
} // namespace

void NetShareCallbackObserver::OnSharingStateChanged(const bool &isRunning)
{
    if (!DelayedSingleton<NetShareObserverWrapper>::GetInstance()->GetEventManager()->HasEventListener(
        static_cast<std::string>(EVENT_SHARE_STATE_CHANGE))) {
        NETMANAGER_EXT_LOGE("no event listener find event sharingStateChange");
        return;
    }

    bool *running = new bool(isRunning);
    DelayedSingleton<NetShareObserverWrapper>::GetInstance()->GetEventManager()->EmitByUv(
        static_cast<std::string>(EVENT_SHARE_STATE_CHANGE), reinterpret_cast<void *>(running),
        SharingStateChangedCallback);
}

void NetShareCallbackObserver::OnInterfaceSharingStateChanged(const SharingIfaceType &type, const std::string &iface,
                                                              const SharingIfaceState &state)
{
    if (!DelayedSingleton<NetShareObserverWrapper>::GetInstance()->GetEventManager()->HasEventListener(
        static_cast<std::string>(EVENT_IFACE_SHARE_STATE_CHANGE))) {
        NETMANAGER_EXT_LOGE("no event listener find interfaceSharingStateChange");
        return;
    }

    SharingState *data = new SharingState();
    data->type = type;
    data->iface = iface;
    data->state = state;
    DelayedSingleton<NetShareObserverWrapper>::GetInstance()->GetEventManager()->EmitByUv(
        static_cast<std::string>(EVENT_IFACE_SHARE_STATE_CHANGE), reinterpret_cast<void *>(data),
        InterfaceSharingStateChangedCallback);
}

void NetShareCallbackObserver::OnSharingUpstreamChanged(const sptr<NetHandle> netHandle)
{
    if (!DelayedSingleton<NetShareObserverWrapper>::GetInstance()->GetEventManager()->HasEventListener(
        static_cast<std::string>(EVENT_SHARE_UPSTREAM_CHANGE))) {
        NETMANAGER_EXT_LOGE("no event listener find sharingUpstreamChange");
        return;
    }
    DelayedSingleton<NetShareObserverWrapper>::GetInstance()->GetEventManager()->EmitByUv(
        static_cast<std::string>(EVENT_SHARE_UPSTREAM_CHANGE), netHandle, SharingUpstreamChangedCallback);
}

napi_value NetShareCallbackObserver::CreateSharingStateChangedParam(napi_env env, void *data)
{
    if (data == nullptr) {
        return nullptr;
    }
    auto inputBoolean = static_cast<bool *>(data);
    napi_value boolean = NapiUtils::GetBoolean(env, *inputBoolean);
    delete inputBoolean;
    return boolean;
}

napi_value NetShareCallbackObserver::CreateInterfaceSharingStateChangedParam(napi_env env, void *data)
{
    if (data == nullptr) {
        return nullptr;
    }
    auto sharingState = static_cast<SharingState *>(data);
    napi_value obj = NapiUtils::CreateObject(env);
    NapiUtils::SetInt32Property(env, obj, "type", static_cast<int32_t>(sharingState->type));
    NapiUtils::SetStringPropertyUtf8(env, obj, "iface", sharingState->iface);
    NapiUtils::SetInt32Property(env, obj, "state", static_cast<int32_t>(sharingState->state));
    delete sharingState;
    return obj;
}

napi_value NetShareCallbackObserver::CreateSharingUpstreamChangedParam(napi_env env, void *data)
{
    (void)data;
    return NapiUtils::GetUndefined(env);
}

void NetShareCallbackObserver::SharingStateChangedCallback(uv_work_t *work, int status)
{
    CallbackTemplate<CreateSharingStateChangedParam>(work, status);
}

void NetShareCallbackObserver::InterfaceSharingStateChangedCallback(uv_work_t *work, int status)
{
    CallbackTemplate<CreateInterfaceSharingStateChangedParam>(work, status);
}

void NetShareCallbackObserver::SharingUpstreamChangedCallback(uv_work_t *work, int status)
{
    CallbackTemplate<CreateSharingUpstreamChangedParam>(work, status);
}
} // namespace NetManagerStandard
} // namespace OHOS
