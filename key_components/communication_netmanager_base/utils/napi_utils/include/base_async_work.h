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

#ifndef COMMUNICATIONNETMANAGER_BASE_NETMANAGER_BASE_BASE_ASYNC_WORK_H
#define COMMUNICATIONNETMANAGER_BASE_NETMANAGER_BASE_BASE_ASYNC_WORK_H

#include <limits>
#include <memory>

#include <napi/native_api.h>
#include <napi/native_common.h>

#include "base_context.h"
#include "napi_constant.h"
#include "napi_utils.h"
#include "netmanager_base_log.h"
#include "nocopyable.h"

namespace OHOS {
namespace NetManagerStandard {
class BaseAsyncWork final {
public:
    DISALLOW_COPY_AND_MOVE(BaseAsyncWork);

    BaseAsyncWork() = delete;

    template <class Context, bool (*Executor)(Context *)> static void ExecAsyncWork(napi_env env, void *data)
    {
        static_assert(std::is_base_of<BaseContext, Context>::value);

        (void)env;

        auto context = reinterpret_cast<Context *>(data);
        if (context == nullptr || Executor == nullptr) {
            NETMANAGER_BASE_LOGE("context or Executor is nullptr");
            return;
        }
        context->SetExecOK(Executor(context));
        /* do not have async executor, execOK should be set in sync work */
    }

    template <class Context, napi_value (*Callback)(Context *)>
    static void AsyncWorkCallback(napi_env env, napi_status status, void *data)
    {
        static_assert(std::is_base_of<BaseContext, Context>::value);

        if ((status != napi_ok) || (data == nullptr)) {
            return;
        }
        auto deleter = [](Context *context) { delete context; };
        std::unique_ptr<Context, decltype(deleter)> context(static_cast<Context *>(data), deleter);
        size_t argc = 2;
        napi_value argv[2] = {nullptr};
        if (context->IsExecOK()) {
            argv[0] = NapiUtils::GetUndefined(env);

            if (Callback != nullptr) {
                argv[1] = Callback(context.get());
            } else {
                argv[1] = NapiUtils::GetUndefined(env);
            }
            if (argv[1] == nullptr) {
                NETMANAGER_BASE_LOGE("AsyncWorkName %{public}s createData fail", context->GetAsyncWorkName().c_str());
                return;
            }
        } else {
            argv[0] = NapiUtils::CreateErrorMessage(env, context->GetErrorCode(), context->GetErrorMessage());
            if (argv[0] == nullptr) {
                NETMANAGER_BASE_LOGE("AsyncWorkName %{public}s createErrorMessage fail",
                                     context->GetAsyncWorkName().c_str());
                return;
            }

            argv[1] = NapiUtils::GetUndefined(env);
        }

        if (context->GetDeferred() != nullptr) {
            if (context->IsExecOK()) {
                napi_resolve_deferred(env, context->GetDeferred(), argv[1]);
            } else {
                napi_reject_deferred(env, context->GetDeferred(), argv[0]);
            }
            return;
        }

        napi_value func = context->GetCallback();
        napi_value undefined = NapiUtils::GetUndefined(env);
        (void)NapiUtils::CallFunction(env, undefined, func, argc, argv);
    }

    template <class Context, napi_value (*Callback)(Context *)>
    static void AsyncWorkCallbackForSystem(napi_env env, napi_status status, void *data)
    {
        static_assert(std::is_base_of<BaseContext, Context>::value);

        if ((status != napi_ok) || (data == nullptr)) {
            return;
        }
        auto deleter = [](Context *context) { delete context; };
        std::unique_ptr<Context, decltype(deleter)> context(static_cast<Context *>(data), deleter);
        if (Callback != nullptr) {
            (void)Callback(context.get());
        }
    }
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif // COMMUNICATIONNETMANAGER_BASE_NETMANAGER_BASE_BASE_ASYNC_WORK_H
