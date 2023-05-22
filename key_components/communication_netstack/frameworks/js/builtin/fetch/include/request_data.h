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

#ifndef OHOS_ACELITE_REQUEST_DATA_H
#define OHOS_ACELITE_REQUEST_DATA_H

#include <map>
#include <string>

namespace OHOS {
namespace ACELite {
class RequestData {
public:
    RequestData();

    void SetUrl(const std::string &urlPara);

    void SetMethod(const std::string &methodPara);

    void SetHeader(const std::string &key, const std::string &val);

    void SetBody(const std::string &bodyPara);

    void SetResponseType(const std::string &respType);

    [[nodiscard]] const std::string &GetUrl() const;

    [[nodiscard]] const std::map<std::string, std::string> &GetHeader() const;

    [[nodiscard]] const std::string &GetMethod() const;

    [[nodiscard]] const std::string &GetBody() const;

    [[nodiscard]] const std::string &GetResponseType() const;

private:
    std::string url;
    std::map<std::string, std::string> header;
    std::string method;
    std::string body;
    std::string responseType;
};
} // namespace ACELite
} // namespace OHOS

#endif /* OHOS_ACELITE_REQUEST_DATA_H */
