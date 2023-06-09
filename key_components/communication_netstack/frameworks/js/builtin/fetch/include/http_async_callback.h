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

#ifndef OHOS_ACELITE_HTTP_ASYNC_CALLBACK_H
#define OHOS_ACELITE_HTTP_ASYNC_CALLBACK_H

#include "fetch_module.h"
#include "jsi.h"
#include "non_copyable.h"
#include "response_data.h"

namespace OHOS {
namespace ACELite {

class HttpAsyncCallback : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(HttpAsyncCallback);

    explicit HttpAsyncCallback(JSIValue thisValue);

public:
    static void AsyncExecHttpRequest(void *data);
    friend JSIValue FetchModule::Fetch(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);

private:
    RequestData requestData;
    std::map<std::string, JSIValue> responseCallback;
    JSIValue thisVal;

private:
    static void AsyncCallbackDeleter(HttpAsyncCallback *asyncCallback);

    JSIValue ResponseDataToJsValue(const ResponseData &responseData);
    void OnSuccess(const ResponseData &responseData);
    void OnFail(const char *errData, int32_t errCode);
    void OnComplete();
};
} // namespace ACELite
} // namespace OHOS

#endif /* OHOS_ACELITE_HTTP_ASYNC_CALLBACK_H */
