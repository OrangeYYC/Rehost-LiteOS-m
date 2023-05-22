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

#ifndef OHOS_ACELITE_HTTP_CONSTANT_H
#define OHOS_ACELITE_HTTP_CONSTANT_H

#include "non_copyable.h"

namespace OHOS {
namespace ACELite {
class HttpConstant final {
public:
    ACE_DISALLOW_COPY_AND_MOVE(HttpConstant);
    static const int HTTP_RESPONSE_CODE_INVALID;

    static const char *const HTTP_RESPONSE_TYPE_JSON;

    static const char *const HTTP_HEADER_SEPARATOR;

    static const char *const HTTP_DEFAULT_USER_AGENT;

    static const char *const HTTP_DEFAULT_CONTENT_TYPE;
    static const char *const HTTP_HEADER_KEY_CONTENT_TYPE;
    static const char *const HTTP_CONTENT_TYPE_URL_ENCODE;
    static const char *const HTTP_CONTENT_TYPE_JSON;

    static const char *const HTTP_URL_PARAM_SEPARATOR;
    static const char *const HTTP_URL_PARAM_DELIMITER;

    static const char *const HTTP_METHOD_GET;
    static const char *const HTTP_METHOD_HEAD;
    static const char *const HTTP_METHOD_OPTIONS;
    static const char *const HTTP_METHOD_TRACE;
    static const char *const HTTP_METHOD_DELETE;
    static const char *const HTTP_METHOD_POST;
    static const char *const HTTP_METHOD_PUT;

    static const char *const KEY_HTTP_RESPONSE_CODE;
    static const char *const KEY_HTTP_RESPONSE_DATA;
    static const char *const KEY_HTTP_RESPONSE_HEADERS;

    static const char *const KEY_HTTP_REQUEST_URL;
    static const char *const KEY_HTTP_REQUEST_DATA;
    static const char *const KEY_HTTP_REQUEST_HEADER;
    static const char *const KEY_HTTP_REQUEST_METHOD;
    static const char *const KEY_HTTP_REQUEST_RESPONSE_TYPE;

};
} // namespace ACELite
} // namespace OHOS

#endif /* OHOS_ACELITE_HTTP_CONSTANT_H */
