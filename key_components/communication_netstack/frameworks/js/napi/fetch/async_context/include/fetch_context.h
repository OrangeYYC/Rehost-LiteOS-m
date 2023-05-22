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

#ifndef COMMUNICATIONNETSTACK_FETCH_CONTEXT_H
#define COMMUNICATIONNETSTACK_FETCH_CONTEXT_H

#include "fetch_request.h"
#include "fetch_response.h"
#include "base_context.h"
#include "nocopyable.h"

namespace OHOS::NetStack {
class FetchContext final : public BaseContext {
public:
    DISALLOW_COPY_AND_MOVE(FetchContext);

    FetchContext() = delete;

    explicit FetchContext(napi_env env, EventManager *manager);

    void ParseParams(napi_value *params, size_t paramsCount) override;

    [[nodiscard]] napi_value GetSuccessCallback() const;

    [[nodiscard]] napi_value GetFailCallback() const;

    [[nodiscard]] napi_value GetCompleteCallback() const;

    [[nodiscard]] std::string GetResponseType() const;

    FetchRequest request;

    FetchResponse response;

private:
    napi_status SetSuccessCallback(napi_value callback);

    napi_status SetFailCallback(napi_value callback);

    napi_status SetCompleteCallback(napi_value callback);

    bool CheckParamsType(napi_value *params, size_t paramsCount);

    void ParseHeader(napi_value optionsValue);

    bool ParseData(napi_value optionsValue);

    bool GetRequestBody(napi_value data);

    bool GetUrlParam(napi_value data);

    std::string responseType_;

    napi_ref successCallback_;

    napi_ref failCallback_;

    napi_ref completeCallback_;
};
} // namespace OHOS::NetStack

#endif /* COMMUNICATIONNETSTACK_FETCH_CONTEXT_H */
