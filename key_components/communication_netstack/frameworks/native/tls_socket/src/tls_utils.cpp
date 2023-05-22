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

#include "tls_utils.h"

#include <climits>
#include <cstdlib>

#include "tls.h"
#include "netstack_log.h"

namespace OHOS {
namespace NetStack {
bool IsValidPath(const std::string &filePath)
{
    std::string path = filePath.substr(0, CERT_PATH_LEN);
    return strcmp(path.c_str(), CERT_PATH) == 0;
}

bool CheckFilePath(std::string fileName, std::string &realPath)
{
    char tmpPath[PATH_MAX] = {0};
    if (!realpath(static_cast<const char *>(fileName.c_str()), tmpPath)) {
        NETSTACK_LOGE("file name is error");
        return false;
    }
    if (!IsValidPath(tmpPath)) {
        NETSTACK_LOGE("file path is error");
        return false;
    }
    realPath = tmpPath;
    return true;
}
} // namespace NetStack
} // namespace OHOS