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

#include <cstring>
#include <memory>

#include "http_constant.h"

#include "http_request_utils.h"

namespace OHOS {
namespace ACELite {
std::vector<std::string> Split(const std::string &str, const std::string &sep)
{
    std::string s = str;
    std::vector<std::string> res;
    while (!s.empty()) {
        auto pos = s.find(sep);
        if (pos == std::string::npos) {
            res.emplace_back(s);
            break;
        }
        res.emplace_back(s.substr(0, pos));
        s = s.substr(pos + sep.size());
    }
    return res;
}

std::string Strip(const std::string &str, char ch)
{
    int64_t i = 0;
    while (i < str.size() && str[i] == ch) {
        ++i;
    }
    int64_t j = static_cast<int64_t>(str.size()) - 1;
    while (j > 0 && str[j] == ch) {
        --j;
    }
    if (i >= 0 && i < str.size() && j >= 0 && j < str.size() && j - i + 1 > 0) {
        return str.substr(i, j - i + 1);
    }
    return "";
}

bool MethodForGet(const std::string &method)
{
    return (method == HttpConstant::HTTP_METHOD_HEAD || method == HttpConstant::HTTP_METHOD_OPTIONS ||
            method == HttpConstant::HTTP_METHOD_DELETE || method == HttpConstant::HTTP_METHOD_TRACE ||
            method == HttpConstant::HTTP_METHOD_GET);
}

bool MethodForPost(const std::string &method)
{
    return (method == HttpConstant::HTTP_METHOD_POST || method == HttpConstant::HTTP_METHOD_PUT);
}

std::string MakeUrl(const std::string &url, std::string param, const std::string &extraParam)
{
    if (param.empty()) {
        param += extraParam;
    } else {
        param += HttpConstant::HTTP_URL_PARAM_DELIMITER;
        param += extraParam;
    }

    if (param.empty()) {
        return url;
    }

    return url + HttpConstant::HTTP_URL_PARAM_SEPARATOR + param;
}

bool EncodeUrlParam(CURL *curl, std::string &url)
{
    auto index = url.find(HttpConstant::HTTP_URL_PARAM_SEPARATOR);
    if (index == std::string::npos) {
        return false;
    }

    std::string param = url.substr(index + 1);
    if (param.empty()) {
        return false;
    }

    std::unique_ptr<char, decltype(&curl_free)> encodeOut(
        curl_easy_escape(curl, param.c_str(), static_cast<int>(strlen(param.c_str()))), curl_free);
    if (encodeOut == nullptr || strlen(encodeOut.get()) == 0) {
        return false;
    }
    url.resize(index);
    url.append(HttpConstant::HTTP_URL_PARAM_SEPARATOR).append(encodeOut.get());
    return true;
}
} // namespace ACELite
} // namespace OHOS