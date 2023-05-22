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

#include "http_constant.h"
#include "http_request_utils.h"

#include "response_data.h"

namespace OHOS {
namespace ACELite {

ResponseData::ResponseData()
{
    code = HttpConstant::HTTP_RESPONSE_CODE_INVALID;
}

void ResponseData::SetCode(int32_t codePara)
{
    code = codePara;
}

void ResponseData::SetErrString(const std::string &err)
{
    errString = err;
}

void ResponseData::ParseHeaders(const std::string &headersStr)
{
    std::vector<std::string> vec = Split(headersStr, "\n");
    for (auto header : vec) {
        header = Strip(Strip(header), '\r');
        if (header.empty()) {
            continue;
        }
        auto index = header.find(HttpConstant::HTTP_HEADER_SEPARATOR);
        if (index == std::string::npos) {
            statusLine = Strip(header);
            continue;
        }
        headers[Strip(header.substr(0, index))] = Strip(header.substr(index + 1));
    }
}

int32_t ResponseData::GetCode() const
{
    return code;
}

const std::string &ResponseData::GetErrString() const
{
    return errString;
}

void ResponseData::AppendData(const char *dataPara, size_t size)
{
    data.append(dataPara, size);
}

const std::string &ResponseData::GetData() const
{
    return data;
}

const std::string &ResponseData::GetStatusLine() const
{
    return statusLine;
}

const std::map<std::string, std::string> &ResponseData::GetHeaders() const
{
    return headers;
}
} // namespace ACELite
} // namespace OHOS
