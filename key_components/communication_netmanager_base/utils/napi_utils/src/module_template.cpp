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

#include "module_template.h"
#include "netmanager_base_log.h"

#include <algorithm>

namespace OHOS {
namespace NetManagerStandard {
namespace ModuleTemplate {
namespace {
static constexpr const int EVENT_PARAM_NUM = 2;
} // namespace

napi_value On(napi_env env, napi_callback_info info, const std::initializer_list<std::string> &events,
              bool asyncCallback)
{
    napi_value thisVal = nullptr;
    size_t paramsCount = MAX_PARAM_NUM;
    napi_value params[MAX_PARAM_NUM] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &paramsCount, params, &thisVal, nullptr));

    if (paramsCount != EVENT_PARAM_NUM || NapiUtils::GetValueType(env, params[0]) != napi_string ||
        NapiUtils::GetValueType(env, params[1]) != napi_function) {
        NETMANAGER_BASE_LOGE("napi on interface para: [string, function]");
        return NapiUtils::GetUndefined(env);
    }

    std::string event = NapiUtils::GetStringFromValueUtf8(env, params[0]);
    if (std::find(events.begin(), events.end(), event) == events.end()) {
        return NapiUtils::GetUndefined(env);
    }

    EventManager *manager = nullptr;
    napi_unwrap(env, thisVal, reinterpret_cast<void **>(&manager));
    if (manager != nullptr) {
        manager->AddListener(env, event, params[1], false, asyncCallback);
    }

    return NapiUtils::GetUndefined(env);
}

napi_value Once(napi_env env, napi_callback_info info, const std::initializer_list<std::string> &events,
                bool asyncCallback)
{
    napi_value thisVal = nullptr;
    size_t paramsCount = MAX_PARAM_NUM;
    napi_value params[MAX_PARAM_NUM] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &paramsCount, params, &thisVal, nullptr));

    if (paramsCount != EVENT_PARAM_NUM || NapiUtils::GetValueType(env, params[0]) != napi_string ||
        NapiUtils::GetValueType(env, params[1]) != napi_function) {
        NETMANAGER_BASE_LOGE("napi once interface para: [string, function]");
        return NapiUtils::GetUndefined(env);
    }

    std::string event = NapiUtils::GetStringFromValueUtf8(env, params[0]);
    if (std::find(events.begin(), events.end(), event) == events.end()) {
        return NapiUtils::GetUndefined(env);
    }

    EventManager *manager = nullptr;
    napi_unwrap(env, thisVal, reinterpret_cast<void **>(&manager));
    if (manager != nullptr) {
        manager->AddListener(env, event, params[1], true, asyncCallback);
    }

    return NapiUtils::GetUndefined(env);
}

napi_value Off(napi_env env, napi_callback_info info, const std::initializer_list<std::string> &events)
{
    napi_value thisVal = nullptr;
    size_t paramsCount = MAX_PARAM_NUM;
    napi_value params[MAX_PARAM_NUM] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &paramsCount, params, &thisVal, nullptr));

    if ((paramsCount != 1 && paramsCount != EVENT_PARAM_NUM) ||
        NapiUtils::GetValueType(env, params[0]) != napi_string) {
        NETMANAGER_BASE_LOGE("napi off interface para: [string, function?]");
        return NapiUtils::GetUndefined(env);
    }

    if (paramsCount == EVENT_PARAM_NUM && NapiUtils::GetValueType(env, params[1]) != napi_function) {
        NETMANAGER_BASE_LOGE("napi off interface para: [string, function]");
        return NapiUtils::GetUndefined(env);
    }

    std::string event = NapiUtils::GetStringFromValueUtf8(env, params[0]);
    if (std::find(events.begin(), events.end(), event) == events.end()) {
        return NapiUtils::GetUndefined(env);
    }

    EventManager *manager = nullptr;
    napi_unwrap(env, thisVal, reinterpret_cast<void **>(&manager));
    if (manager != nullptr) {
        if (paramsCount == EVENT_PARAM_NUM) {
            manager->DeleteListener(event, params[1]);
        } else {
            manager->DeleteListener(event);
        }
    }

    return NapiUtils::GetUndefined(env);
}

void DefineClass(napi_env env, napi_value exports, const std::initializer_list<napi_property_descriptor> &properties,
                 const std::string &className)
{
    auto constructor = [](napi_env env, napi_callback_info info) -> napi_value {
        napi_value thisVal = nullptr;
        NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVal, nullptr));

        return thisVal;
    };

    napi_value jsConstructor = nullptr;

    napi_property_descriptor descriptors[properties.size()];
    std::copy(properties.begin(), properties.end(), descriptors);

    NAPI_CALL_RETURN_VOID(env, napi_define_class(env, className.c_str(), NAPI_AUTO_LENGTH, constructor, nullptr,
                                                 properties.size(), descriptors, &jsConstructor));

    NapiUtils::SetNamedProperty(env, exports, className, jsConstructor);
}

napi_value NewInstance(napi_env env, napi_callback_info info, const std::string &className,
                       void *(*MakeData)(napi_env, size_t, napi_value *, EventManager *), Finalizer finalizer)
{
    napi_value thisVal = nullptr;
    std::size_t argc = MAX_PARAM_NUM;
    napi_value argv[MAX_PARAM_NUM] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVal, nullptr));

    napi_value jsConstructor = NapiUtils::GetNamedProperty(env, thisVal, className);
    if (NapiUtils::GetValueType(env, jsConstructor) == napi_undefined) {
        return nullptr;
    }

    napi_value result = nullptr;
    NAPI_CALL(env, napi_new_instance(env, jsConstructor, 0, nullptr, &result));

    auto manager = new EventManager();
    if (MakeData != nullptr) {
        auto data = MakeData(env, argc, argv, manager);
        if (data == nullptr) {
            return NapiUtils::GetUndefined(env);
        }
        manager->SetData(data);
    }
    napi_wrap(env, result, reinterpret_cast<void *>(manager), finalizer, nullptr, nullptr);

    return result;
}
} // namespace ModuleTemplate
} // namespace NetManagerStandard
} // namespace OHOS
