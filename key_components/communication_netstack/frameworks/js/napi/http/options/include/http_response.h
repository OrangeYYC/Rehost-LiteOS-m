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

#ifndef COMMUNICATIONNETSTACK_HTTP_RESPONSE_H
#define COMMUNICATIONNETSTACK_HTTP_RESPONSE_H

#include <map>
#include <string>

namespace OHOS::NetStack {
static constexpr const char *WARNING = "Warning";

class HttpResponse final {
public:
    HttpResponse();

    void AppendResult(const void *data, size_t length);

    void AppendRawHeader(const void *data, size_t length);

    void SetRawHeader(const std::string &header);

    void SetResponseCode(uint32_t responseCode);

    void ParseHeaders();

    void SetResult(const std::string &res);

    void SetCookies(const std::string &cookies);

    void AppendCookies(const void *data, size_t length);

    [[nodiscard]] const std::string &GetResult() const;

    [[nodiscard]] uint32_t GetResponseCode() const;

    [[nodiscard]] const std::map<std::string, std::string> &GetHeader() const;

    [[nodiscard]] const std::string &GetCookies() const;

    [[nodiscard]] const std::string &GetRawHeader() const;

    void SetResponseTime(const std::string &time);

    [[nodiscard]] const std::string &GetResponseTime() const;

    void SetRequestTime(const std::string &time);

    [[nodiscard]] const std::string &GetRequestTime() const;

    void SetWarning(const std::string &val);

private:
    std::string result_;

    std::string rawHeader_;

    uint32_t responseCode_;

    std::map<std::string, std::string> header_;

    std::string cookies_;

    std::string responseTime_;

    std::string requestTime_;
};
} // namespace OHOS::NetStack

#endif /* COMMUNICATIONNETSTACK_HTTP_RESPONSE_H */
