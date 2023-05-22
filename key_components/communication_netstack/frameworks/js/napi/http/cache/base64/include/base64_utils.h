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

#ifndef COMMUNICATIONNETSTACK_BASE64_H
#define COMMUNICATIONNETSTACK_BASE64_H

#include <string>

namespace OHOS::NetStack::Base64 {
std::string Encode(const std::string &source);
std::string Decode(const std::string &encoded);
} // namespace OHOS::NetStack::Base64

#endif /* COMMUNICATIONNETSTACK_BASE64_H */
