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

#ifndef HTTP_CACHE_CONSTANT_H
#define HTTP_CACHE_CONSTANT_H

static constexpr const int DECIMAL = 10;
static constexpr const char *SPLIT = ", ";
static constexpr const char EQUAL = '=';

static constexpr const char *NO_CACHE = "no-cache";
static constexpr const char *NO_STORE = "no-store";
static constexpr const char *NO_TRANSFORM = "no-transform";
static constexpr const char *ONLY_IF_CACHED = "only-if-cached";
static constexpr const char *MAX_AGE = "max-age";
static constexpr const char *MAX_STALE = "max-stale";
static constexpr const char *MIN_FRESH = "min-fresh";
static constexpr const char *CACHE_CONTROL = "cache-control";
static constexpr const char *IF_MODIFIED_SINCE = "if-modified-since";
static constexpr const char *IF_NONE_MATCH = "if-none-match";

static constexpr const char *MUST_REVALIDATE = "must-revalidate";
static constexpr const char *PUBLIC = "public";
static constexpr const char *PRIVATE = "private";
static constexpr const char *PROXY_REVALIDATE = "proxy-revalidate";
static constexpr const char *S_MAXAGE = "s-maxage";
static constexpr const char *EXPIRES = "expires";
static constexpr const char *LAST_MODIFIED = "last-modified";
static constexpr const char *ETAG = "etag";
static constexpr const char *AGE = "age";
static constexpr const char *DATE = "date";

static constexpr const int INVALID_TIME = -1;

#endif /* HTTP_CACHE_CONSTANT_H */
