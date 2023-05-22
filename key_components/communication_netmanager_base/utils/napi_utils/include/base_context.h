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

#ifndef COMMUNICATIONNETMANAGER_BASE_BASE_CONTEXT_H
#define COMMUNICATIONNETMANAGER_BASE_BASE_CONTEXT_H

#include <list>

#include <napi/native_api.h>
#include <napi/native_common.h>

#include "errorcode_convertor.h"
#include "event_manager.h"
#include "node_api_types.h"
#include "nocopyable.h"

namespace OHOS {
namespace NetManagerStandard {
using AsyncWorkExecutor = void (*)(napi_env env, void *data);
using AsyncWorkCallback = void (*)(napi_env env, napi_status status, void *data);

class BaseContext {
public:
    DISALLOW_COPY_AND_MOVE(BaseContext);

    BaseContext() = delete;
    explicit BaseContext(napi_env env, EventManager *manager);
    virtual ~BaseContext();

    void SetParseOK(bool parseOK);
    void SetExecOK(bool requestOK);
    void SetErrorCode(int32_t errorCode);
    void SetError(int32_t errorCode, const std::string &errorMessage);
    void DeleteCallback();
    void CreateAsyncWork(const std::string &name, AsyncWorkExecutor executor, AsyncWorkCallback callback);
    void DeleteAsyncWork();
    void Emit(const std::string &type, const std::pair<napi_value, napi_value> &argv);
    void SetNeedPromise(bool needPromise);
    void SetNeedThrowException(bool needThrowException);
    napi_status SetCallback(napi_value callback);
    napi_value CreatePromise();
    [[nodiscard]] bool IsParseOK() const;
    [[nodiscard]] bool IsExecOK() const;
    [[nodiscard]] napi_env GetEnv() const;
    [[nodiscard]] int32_t GetErrorCode() const;
    [[nodiscard]] const std::string &GetErrorMessage() const;
    [[nodiscard]] napi_value GetCallback() const;
    [[nodiscard]] napi_deferred GetDeferred() const;
    [[nodiscard]] const std::string &GetAsyncWorkName() const;
    [[nodiscard]] EventManager *GetManager() const;
    [[nodiscard]] bool IsNeedPromise() const;
    [[nodiscard]] bool IsNeedThrowException() const;

protected:
    EventManager *manager_;

private:
    napi_env env_;
    bool parseOK_;
    bool requestOK_;
    int32_t errorCode_;
    std::string errorMessage_;
    napi_ref callback_;
    napi_async_work asyncWork_;
    napi_deferred deferred_;
    std::string asyncWorkName_;
    bool needPromise_;
    bool needThrowException_;
    NetBaseErrorCodeConvertor convertor_;
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif // COMMUNICATIONNETMANAGER_BASE_BASE_CONTEXT_H
