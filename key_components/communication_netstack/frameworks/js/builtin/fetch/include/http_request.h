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

#ifndef OHOS_ACELITE_HTTP_REQUEST_H
#define OHOS_ACELITE_HTTP_REQUEST_H

#include <mutex>
#include <vector>

#include "curl/curl.h"
#include "non_copyable.h"
#include "request_data.h"
#include "response_data.h"

namespace OHOS {
namespace ACELite {

class HttpRequest final {
public:
    ACE_DISALLOW_COPY_AND_MOVE(HttpRequest);

    HttpRequest() = default;

    ~HttpRequest() = default;

    static bool Request(RequestData *requestData, ResponseData *responseData);

private:
    static bool Initialize();

    static bool SetOption(RequestData *requestData, CURL *curl, ResponseData *responseData);

    static size_t OnWritingMemoryBody(const void *data, size_t size, size_t memBytes, void *userData);

    static size_t OnWritingMemoryHeader(const void *data, size_t size, size_t memBytes, void *userData);

    static struct curl_slist *MakeHeaders(const std::vector<std::string> &vec);

private:
    static std::mutex mutex;

    static bool initialized;
};

} // namespace ACELite
} // namespace OHOS

#endif /* OHOS_ACELITE_HTTP_REQUEST_H */
