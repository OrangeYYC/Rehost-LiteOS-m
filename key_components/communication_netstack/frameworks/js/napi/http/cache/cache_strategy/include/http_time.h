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

#ifndef HTTP_TIME_H
#define HTTP_TIME_H

#include <cstring>
#include <iostream>

namespace OHOS::NetStack {
class HttpTime {
public:
    static time_t StrTimeToTimestamp(const std::string &time_str);

    static time_t GetNowTimeSeconds();

    static std::string GetNowTimeGMT();
};
} // namespace OHOS::NetStack
#endif // HTTP_TIME_H
