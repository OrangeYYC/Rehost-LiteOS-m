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

#ifndef COMMUNICATIONNETSTACK_CACHE_PROXY_H
#define COMMUNICATIONNETSTACK_CACHE_PROXY_H

#include "http_cache_strategy.h"
#include "http_request_options.h"
#include "http_response.h"
#include "request_context.h"

namespace OHOS::NetStack {
class CacheProxy final {
public:
    CacheProxy() = delete;

    explicit CacheProxy(HttpRequestOptions &requestOptions);

    bool ReadResponseFromCache(RequestContext *context);

    void WriteResponseToCache(const HttpResponse &response);

    static void RunCacheWithSize(size_t capacity);

    static void RunCache();

    static void FlushCache();

    static void StopCacheAndDelete();

private:
    std::string key_;
    HttpCacheStrategy strategy_;
};
} // namespace OHOS::NetStack
#endif // COMMUNICATIONNETSTACK_CACHE_PROXY_H
