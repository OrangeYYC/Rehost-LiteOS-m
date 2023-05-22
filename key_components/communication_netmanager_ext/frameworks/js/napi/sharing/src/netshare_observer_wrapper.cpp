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

#include "netshare_observer_wrapper.h"

#include "constant.h"
#include "module_template.h"
#include "net_manager_constants.h"
#include "netmanager_ext_log.h"
#include "networkshare_client.h"
#include "networkshare_constants.h"

namespace OHOS {
namespace NetManagerStandard {
NetShareObserverWrapper::NetShareObserverWrapper()
{
    observer_ = new NetShareCallbackObserver();
    manager_ = new EventManager();
}

NetShareObserverWrapper::~NetShareObserverWrapper()
{
    if (manager_ != nullptr) {
        delete manager_;
        manager_ = nullptr;
    }
}

napi_value NetShareObserverWrapper::On(napi_env env, napi_callback_info info,
                                       const std::initializer_list<std::string_view> &events, bool asyncCallback)
{
    size_t paramsCount = MAX_PARAM_NUM;
    napi_value params[MAX_PARAM_NUM] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &paramsCount, params, nullptr, nullptr));
    if (paramsCount != PARAM_OPTIONS_AND_CALLBACK || NapiUtils::GetValueType(env, params[ARG_INDEX_0]) != napi_string ||
        NapiUtils::GetValueType(env, params[ARG_INDEX_1]) != napi_function) {
        NETMANAGER_EXT_LOGE("on off once interface para: [string, function]");
        napi_throw_error(env, std::to_string(NETMANAGER_EXT_ERR_PARAMETER_ERROR).c_str(), "Parameter error");
        return NapiUtils::GetUndefined(env);
    }

    const std::string event = NapiUtils::GetStringFromValueUtf8(env, params[ARG_INDEX_0]);

    if (Register()) {
        manager_->AddListener(env, event, params[ARG_INDEX_1], false, asyncCallback);
    }

    return NapiUtils::GetUndefined(env);
}

napi_value NetShareObserverWrapper::Off(napi_env env, napi_callback_info info,
                                        const std::initializer_list<std::string_view> &events, bool asyncCallback)
{
    napi_value thisVal = nullptr;
    size_t paramsCount = MAX_PARAM_NUM;
    napi_value params[MAX_PARAM_NUM] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &paramsCount, params, &thisVal, nullptr));

    if ((paramsCount != PARAM_JUST_OPTIONS && paramsCount != PARAM_OPTIONS_AND_CALLBACK) ||
        NapiUtils::GetValueType(env, params[ARG_INDEX_0]) != napi_string) {
        NETMANAGER_EXT_LOGE("on off once interface para: [string, function?]");
        napi_throw_error(env, std::to_string(NETMANAGER_EXT_ERR_PARAMETER_ERROR).c_str(), "Parameter error");
        return NapiUtils::GetUndefined(env);
    }

    if (paramsCount == PARAM_OPTIONS_AND_CALLBACK &&
        NapiUtils::GetValueType(env, params[ARG_INDEX_1]) != napi_function) {
        NETMANAGER_EXT_LOGE("on off once interface para: [string, function]");
        return NapiUtils::GetUndefined(env);
    }

    const std::string event = NapiUtils::GetStringFromValueUtf8(env, params[ARG_INDEX_0]);
    auto event_iterator = std::find(events.begin(), events.end(), event);
    if (event_iterator == events.end()) {
        return NapiUtils::GetUndefined(env);
    }

    if (paramsCount == PARAM_OPTIONS_AND_CALLBACK) {
        manager_->DeleteListener(event, params[ARG_INDEX_1]);
    } else {
        manager_->DeleteListener(event);
    }

    if (manager_->IsListenerListEmpty()) {
        int32_t result = DelayedSingleton<NetworkShareClient>::GetInstance()->UnregisterSharingEvent(observer_);
        if (result != NETMANAGER_EXT_SUCCESS) {
            NETMANAGER_EXT_LOGE("unregister result = %{public}d", result);
            return NapiUtils::GetUndefined(env);
        }
        registed_ = false;
    }

    return NapiUtils::GetUndefined(env);
}

EventManager *NetShareObserverWrapper::GetEventManager() const
{
    return manager_;
}

bool NetShareObserverWrapper::Register()
{
    if (registed_) {
        return true;
    }

    int32_t result = DelayedSingleton<NetworkShareClient>::GetInstance()->RegisterSharingEvent(observer_);
    if (result != NETMANAGER_EXT_SUCCESS) {
        NETMANAGER_EXT_LOGE("RegisterSharingEvent error = %{public}d", result);
        return false;
    }
    return true;
}
} // namespace NetManagerStandard
} // namespace OHOS
