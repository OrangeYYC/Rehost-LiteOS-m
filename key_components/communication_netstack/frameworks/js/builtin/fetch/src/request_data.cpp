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

#include "request_data.h"

namespace OHOS {
namespace ACELite {

RequestData::RequestData()
{
    method = HttpConstant::HTTP_METHOD_GET;
    header[HttpConstant::HTTP_HEADER_KEY_CONTENT_TYPE] = HttpConstant::HTTP_DEFAULT_CONTENT_TYPE;
}

void RequestData::SetUrl(const std::string &urlPara)
{
    url = urlPara;
}

void RequestData::SetMethod(const std::string &methodPara)
{
    method = methodPara;
}

void RequestData::SetHeader(const std::string &key, const std::string &val)
{
    header[key] = val;
}

void RequestData::SetBody(const std::string &bodyPara)
{
    body = bodyPara;
}

void RequestData::SetResponseType(const std::string &respType)
{
    responseType = respType;
}

const std::string &RequestData::GetUrl() const
{
    return url;
}

const std::map<std::string, std::string> &RequestData::GetHeader() const
{
    return header;
}

const std::string &RequestData::GetMethod() const
{
    return method;
}

const std::string &RequestData::GetBody() const
{
    return body;
}

const std::string &RequestData::GetResponseType() const
{
    return responseType;
}
} // namespace ACELite
} // namespace OHOS
