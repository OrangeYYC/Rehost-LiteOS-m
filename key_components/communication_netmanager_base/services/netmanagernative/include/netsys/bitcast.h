/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef INCLUDE_BITCAST_H__
#define INCLUDE_BITCAST_H__

#include <cstring>

namespace OHOS {
namespace nmd {
namespace common {
namespace internal_casts {
template<class Dest, class Source>
struct is_bitcastable
    : std::integral_constant<bool,
          sizeof(Dest) == sizeof(Source) && std::is_trivially_copyable<Source>::value &&
              std::is_trivially_copyable<Dest>::value && std::is_default_constructible<Dest>::value> {};
} // namespace internal_casts
} // namespace common
} // namespace nmd
} // namespace OHOS
#endif // !INCLUDE_BITCAST_H__
