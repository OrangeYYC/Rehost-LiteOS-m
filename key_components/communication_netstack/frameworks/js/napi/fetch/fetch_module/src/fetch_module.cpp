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

#include "fetch_module.h"

#include "fetch_async_work.h"
#include "fetch_exec.h"
#include "netstack_log.h"
#include "module_template.h"

namespace OHOS::NetStack {
napi_value FetchModule::InitFetchModule(napi_env env, napi_value exports)
{
    std::initializer_list<napi_property_descriptor> properties = {
        DECLARE_NAPI_FUNCTION(FUNCTION_FETCH, Fetch),
    };
    NapiUtils::DefineProperties(env, exports, properties);

    return exports;
}

napi_value FetchModule::Fetch(napi_env env, napi_callback_info info)
{
    NETSTACK_LOGI("FetchModule::Fetch is called");
    return ModuleTemplate::Interface<FetchContext>(
        env, info, FUNCTION_FETCH, [](napi_env, napi_value, FetchContext *) -> bool { return FetchExec::Initialize(); },
        FetchAsyncWork::ExecFetch, FetchAsyncWork::FetchCallback);
}

NAPI_MODULE(fetch, FetchModule::InitFetchModule)
} // namespace OHOS::NetStack
