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

#include "constant.h"

namespace OHOS::NetStack {
const char *const FetchConstant::HTTP_METHOD_GET = "GET";
const char *const FetchConstant::HTTP_METHOD_HEAD = "HEAD";
const char *const FetchConstant::HTTP_METHOD_OPTIONS = "OPTIONS";
const char *const FetchConstant::HTTP_METHOD_TRACE = "TRACE";
const char *const FetchConstant::HTTP_METHOD_DELETE = "DELETE";
const char *const FetchConstant::HTTP_METHOD_POST = "POST";
const char *const FetchConstant::HTTP_METHOD_PUT = "PUT";
const char *const FetchConstant::HTTP_METHOD_CONNECT = "CONNECT";

const char *const FetchConstant::PARAM_KEY_URL = "url";
const char *const FetchConstant::PARAM_KEY_METHOD = "method";
const char *const FetchConstant::PARAM_KEY_DATA = "data";
const char *const FetchConstant::PARAM_KEY_HEADER = "header";
const char *const FetchConstant::PARAM_KEY_SUCCESS = "success";
const char *const FetchConstant::PARAM_KEY_FAIL = "fail";
const char *const FetchConstant::PARAM_KEY_COMPLETE = "complete";
const char *const FetchConstant::PARAM_KEY_RESPONSE_TYPE = "responseType";

const char *const FetchConstant::RESPONSE_KEY_DATA = "data";
const char *const FetchConstant::RESPONSE_KEY_CODE = "code";
const char *const FetchConstant::RESPONSE_KEY_HEADERS = "headers";

const char *const FetchConstant::HTTP_URL_PARAM_START = "?";
const char *const FetchConstant::HTTP_URL_PARAM_SEPARATOR = "&";
const char *const FetchConstant::HTTP_URL_NAME_VALUE_SEPARATOR = "=";
const char *const FetchConstant::HTTP_HEADER_SEPARATOR = ":";
const char *const FetchConstant::HTTP_LINE_SEPARATOR = "\r\n";

const char *const FetchConstant::HTTP_DEFAULT_USER_AGENT = "libcurl-agent/1.0";
const char *const FetchConstant::HTTP_DEFAULT_CA_PATH = "/etc/ssl/certs/cacert.pem";

const char *const FetchConstant::HTTP_CONTENT_TYPE = "content-type";
const char *const FetchConstant::HTTP_CONTENT_TYPE_URL_ENCODE = "application/x-www-form-urlencoded";
const char *const FetchConstant::HTTP_CONTENT_TYPE_JSON = "application/json";

const char *const FetchConstant::HTTP_RESPONSE_TYPE_TEXT = "text";
const char *const FetchConstant::HTTP_RESPONSE_TYPE_JSON = "json";
} // namespace OHOS::NetStack
