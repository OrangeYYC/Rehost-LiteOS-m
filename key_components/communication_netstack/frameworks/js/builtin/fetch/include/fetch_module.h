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

#ifndef OHOS_ACELITE_FETCH_MODULE_H
#define OHOS_ACELITE_FETCH_MODULE_H

#include "jsi.h"
#include "memory_heap.h"
#include "non_copyable.h"
#include "request_data.h"

namespace OHOS {
namespace ACELite {

class FetchModule final : public MemoryHeap {
public:
    ACE_DISALLOW_COPY_AND_MOVE(FetchModule);

    FetchModule() = default;

    ~FetchModule() = default;

    static JSIValue StringToArrayBuffer(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);

    static JSIValue Fetch(const JSIValue thisVal, const JSIValue *args, uint8_t argsNum);

public:
    static const char *const HTTP_API_KEY_FETCH;
    static const char *const HTTP_API_KEY_STRING_TO_ARRAY_BUFFER;

private:
    static bool JsObjectToRequestData(JSIValue options, RequestData *req);
    static void GetNameValue(JSIValue nv, std::map<std::string, std::string> &map);
    static void GetRequestBody(JSIValue options, RequestData *requestData);
    static void ParseExtraData(JSIValue options, RequestData *requestData);
    static std::string GetMethodFromOptions(JSIValue options);
    static std::string GetResponseTypeFromOptions(JSIValue options);
};
} // namespace ACELite
} // namespace OHOS

#endif /* OHOS_ACELITE_FETCH_MODULE_H */
