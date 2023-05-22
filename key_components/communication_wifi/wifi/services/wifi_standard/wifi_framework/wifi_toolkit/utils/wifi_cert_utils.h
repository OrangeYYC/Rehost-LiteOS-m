/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_WIFI_CERT_UTILS_H
#define OHOS_WIFI_CERT_UTILS_H

#include <cstdint>
#include <string>
#include<vector>

namespace OHOS {
namespace Wifi {
class WifiCertUtils {
public:
    static int InstallCert(const std::vector<uint8_t>& certEntry,
            const std::string& pwd, std::string& alias, std::string& uri);
    static int UninstallCert(std::string& uri);
};
} // namespace Wifi
} // namespace OHOS
#endif
