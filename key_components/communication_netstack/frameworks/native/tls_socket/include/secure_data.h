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

#ifndef COMMUNICATION_NETSTACK_SECURE_DATA_H
#define COMMUNICATION_NETSTACK_SECURE_DATA_H

#include <cstddef>
#include <memory>
#include <string>

namespace OHOS {
namespace NetStack {
class SecureData {
public:
    SecureData();
    ~SecureData();
    explicit SecureData(const std::string &secureData);
    SecureData(const uint8_t *SecureData, size_t length);

    SecureData(const SecureData &secureData);
    SecureData &operator=(const SecureData &secureData);

public:
    const char *Data() const;
    size_t Length() const;

private:
    size_t length_ = 0;
    std::unique_ptr<char[]> data_ = nullptr;
};
} // namespace NetStack
} // namespace OHOS
#endif // COMMUNICATION_NETSTACK_SECURE_DATA_H