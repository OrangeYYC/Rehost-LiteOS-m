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

#ifndef OHOS_ACELITE_RESPONSE_DATA_H
#define OHOS_ACELITE_RESPONSE_DATA_H

#include <map>
#include <string>

namespace OHOS {
namespace ACELite {
class ResponseData {
public:
    ResponseData();

    void SetCode(int32_t codePara);

    void SetErrString(const std::string &err);

    void ParseHeaders(const std::string &headersStr);

    void AppendData(const char *dataPara, size_t size);

    [[nodiscard]] int32_t GetCode() const;

    [[nodiscard]] const std::string &GetErrString() const;

    [[nodiscard]] const std::string &GetData() const;

    [[nodiscard]] const std::string &GetStatusLine() const;

    [[nodiscard]] const std::map<std::string, std::string> &GetHeaders() const;

private:
    int32_t code;
    std::string data;
    std::string statusLine;
    std::string errString;
    std::map<std::string, std::string> headers;
};
} // namespace ACELite
} // namespace OHOS

#endif /* OHOS_ACELITE_RESPONSE_DATA_H */
