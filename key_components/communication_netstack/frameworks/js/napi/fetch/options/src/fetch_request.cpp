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

#include "fetch_request.h"

#include "constant.h"
#include "netstack_common_utils.h"

namespace OHOS::NetStack {
FetchRequest::FetchRequest() : method_(FetchConstant::HTTP_METHOD_GET)
{
    header_[CommonUtils::ToLower(FetchConstant::HTTP_CONTENT_TYPE)] = FetchConstant::HTTP_CONTENT_TYPE_JSON; // default
}

void FetchRequest::SetUrl(const std::string &url)
{
    url_ = url;
}

void FetchRequest::SetMethod(const std::string &method)
{
    method_ = method;
}

void FetchRequest::SetBody(const void *data, size_t length)
{
    body_.append(static_cast<const char *>(data), length);
}

void FetchRequest::SetHeader(const std::string &key, const std::string &val)
{
    header_[key] = val;
}

const std::string &FetchRequest::GetUrl() const
{
    return url_;
}

const std::string &FetchRequest::GetMethod() const
{
    return method_;
}

const std::string &FetchRequest::GetBody() const
{
    return body_;
}

const std::map<std::string, std::string> &FetchRequest::GetHeader() const
{
    return header_;
}
} // namespace OHOS::NetStack