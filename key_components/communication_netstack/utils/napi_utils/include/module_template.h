/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef COMMUNICATIONNETSTACK_NETSTACK_MODULE_TEMPLATE_H
#define COMMUNICATIONNETSTACK_NETSTACK_MODULE_TEMPLATE_H

#include <cstddef>
#include <initializer_list>
#include <iosfwd>
#include <type_traits>
#include <vector>

#include "base_async_work.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "base_context.h"
#include "netstack_log.h"
#include "napi_utils.h"

namespace OHOS::NetStack {
class EventManager;
}

#define MAX_PARAM_NUM 64

namespace OHOS::NetStack::ModuleTemplate {
typedef void (*Finalizer)(napi_env, void *data, void *);

template <class Context>
napi_value Interface(napi_env env, napi_callback_info info, const std::string &asyncWorkName,
                     bool (*Work)(napi_env, napi_value, Context *), AsyncWorkExecutor executor,
                     AsyncWorkCallback callback)
{
    static_assert(std::is_base_of<BaseContext, Context>::value);

    napi_value thisVal = nullptr;
    size_t paramsCount = MAX_PARAM_NUM;
    napi_value params[MAX_PARAM_NUM] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &paramsCount, params, &thisVal, nullptr));

    EventManager *manager = nullptr;
    napi_unwrap(env, thisVal, reinterpret_cast<void **>(&manager));

    auto context = new Context(env, manager);
    context->ParseParams(params, paramsCount);
    if (!context->IsParseOK()) {
        context->SetError(PARSE_ERROR_CODE, PARSE_ERROR_MSG);
    }
    NETSTACK_LOGI("js params parse OK ? %{public}d", context->IsParseOK());
    if (context->IsNeedThrowException()) { // only api9 or later need throw exception.
        napi_throw_error(env, std::to_string(context->GetErrorCode()).c_str(), context->GetErrorMessage().c_str());
        delete context;
        context = nullptr;
        return NapiUtils::GetUndefined(env);
    }
    if (Work != nullptr) {
        if (!Work(env, thisVal, context)) {
            NETSTACK_LOGE("work failed error code = %{public}d", context->GetErrorCode());
        }
    }

    context->CreateReference(thisVal);
    context->CreateAsyncWork(asyncWorkName, executor, callback);
    if (NapiUtils::GetValueType(env, context->GetCallback()) != napi_function && context->IsNeedPromise()) {
        NETSTACK_LOGI("context->CreatePromise()");
        return context->CreatePromise();
    }
    return NapiUtils::GetUndefined(env);
}

template <class Context>
napi_value InterfaceWithOutAsyncWork(napi_env env, napi_callback_info info,
                                     bool (*Work)(napi_env, napi_value, Context *), const std::string &asyncWorkName,
                                     AsyncWorkExecutor executor, AsyncWorkCallback callback)
{
    static_assert(std::is_base_of<BaseContext, Context>::value);

    napi_value thisVal = nullptr;
    size_t paramsCount = MAX_PARAM_NUM;
    napi_value params[MAX_PARAM_NUM] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &paramsCount, params, &thisVal, nullptr));

    EventManager *manager = nullptr;
    napi_unwrap(env, thisVal, reinterpret_cast<void **>(&manager));

    auto context = new Context(env, manager);
    context->ParseParams(params, paramsCount);
    if (!context->IsParseOK()) {
        context->SetError(PARSE_ERROR_CODE, PARSE_ERROR_MSG);
    }
    napi_value ret = NapiUtils::GetUndefined(env);
    if (NapiUtils::GetValueType(env, context->GetCallback()) != napi_function && context->IsNeedPromise()) {
        ret = context->CreatePromise();
    }
    if (Work != nullptr) {
        if (!Work(env, thisVal, context)) {
            NETSTACK_LOGE("work failed error code = %{public}d", context->GetErrorCode());
        }
    }
    context->CreateReference(thisVal);
    if (context->IsPermissionDenied() || !context->IsParseOK()) {
        context->CreateAsyncWork(asyncWorkName, executor, callback);
    }
    return ret;
}

napi_value On(napi_env env, napi_callback_info info, const std::initializer_list<std::string> &events,
              bool asyncCallback);

napi_value Once(napi_env env, napi_callback_info info, const std::initializer_list<std::string> &events,
                bool asyncCallback);

napi_value Off(napi_env env, napi_callback_info info, const std::initializer_list<std::string> &events);

void DefineClass(napi_env env, napi_value exports, const std::initializer_list<napi_property_descriptor> &properties,
                 const std::string &className);

napi_value NewInstance(napi_env env, napi_callback_info info, const std::string &className, Finalizer finalizer);

napi_value NewInstanceNoManager(napi_env env, napi_callback_info info, const std::string &name, Finalizer finalizer);
} // namespace OHOS::NetStack::ModuleTemplate
#endif /* COMMUNICATIONNETSTACK_NETSTACK_MODULE_TEMPLATE_H */
