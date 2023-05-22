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

#ifndef COMMUNICATIONNETSTACK_CONSTANT_H
#define COMMUNICATIONNETSTACK_CONSTANT_H

#include <cstdint>

#include "nocopyable.h"

namespace OHOS::NetStack {
class FetchConstant final {
public:
    DISALLOW_COPY_AND_MOVE(FetchConstant);

    /* Http Method */
    static const char *const HTTP_METHOD_GET;
    static const char *const HTTP_METHOD_HEAD;
    static const char *const HTTP_METHOD_OPTIONS;
    static const char *const HTTP_METHOD_TRACE;
    static const char *const HTTP_METHOD_DELETE;
    static const char *const HTTP_METHOD_POST;
    static const char *const HTTP_METHOD_PUT;
    static const char *const HTTP_METHOD_CONNECT;

    /* options key */
    static const char *const PARAM_KEY_URL;
    static const char *const PARAM_KEY_METHOD;
    static const char *const PARAM_KEY_DATA;
    static const char *const PARAM_KEY_HEADER;
    static const char *const PARAM_KEY_SUCCESS;
    static const char *const PARAM_KEY_FAIL;
    static const char *const PARAM_KEY_COMPLETE;
    static const char *const PARAM_KEY_RESPONSE_TYPE;

    static const char *const RESPONSE_KEY_DATA;
    static const char *const RESPONSE_KEY_CODE;
    static const char *const RESPONSE_KEY_HEADERS;

    static const char *const HTTP_URL_PARAM_START;
    static const char *const HTTP_URL_PARAM_SEPARATOR;
    static const char *const HTTP_URL_NAME_VALUE_SEPARATOR;
    static const char *const HTTP_HEADER_SEPARATOR;
    static const char *const HTTP_LINE_SEPARATOR;

    static const char *const HTTP_DEFAULT_USER_AGENT;
    static const char *const HTTP_DEFAULT_CA_PATH;

    static const char *const HTTP_CONTENT_TYPE;
    static const char *const HTTP_CONTENT_TYPE_URL_ENCODE;
    static const char *const HTTP_CONTENT_TYPE_JSON;

    static const char *const HTTP_RESPONSE_TYPE_TEXT;
    static const char *const HTTP_RESPONSE_TYPE_JSON;
};
} // namespace OHOS::NetStack

#endif /* COMMUNICATIONNETSTACK_CONSTANT_H */
