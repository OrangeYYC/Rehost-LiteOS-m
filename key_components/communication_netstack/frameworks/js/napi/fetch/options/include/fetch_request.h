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

#ifndef COMMUNICATIONNETSTACK_FETCH_REQUEST_H
#define COMMUNICATIONNETSTACK_FETCH_REQUEST_H

#include <map>
#include <string>

namespace OHOS::NetStack {
class FetchRequest final {
public:
    FetchRequest();

    void SetUrl(const std::string &url);

    void SetMethod(const std::string &method);

    void SetBody(const void *data, size_t length);

    void SetHeader(const std::string &key, const std::string &val);

    [[nodiscard]] const std::string &GetUrl() const;

    [[nodiscard]] const std::string &GetMethod() const;

    [[nodiscard]] const std::string &GetBody() const;

    [[nodiscard]] const std::map<std::string, std::string> &GetHeader() const;

private:
    std::string url_;

    std::string body_;

    std::string method_;

    std::map<std::string, std::string> header_;
};
} // namespace OHOS::NetStack

#endif /* COMMUNICATIONNETSTACK_FETCH_REQUEST_H */
