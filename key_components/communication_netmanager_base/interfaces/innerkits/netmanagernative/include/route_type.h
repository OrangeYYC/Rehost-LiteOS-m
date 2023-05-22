/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef INCLUDE_ROUTE_TYPE_H
#define INCLUDE_ROUTE_TYPE_H

#include <string>

namespace OHOS {
namespace nmd {
static const int LOCAL_NETWORK_NETID = 99;

typedef struct RouteInfoParcel {
    std::string destination;
    std::string ifName;
    std::string nextHop;
    int mtu;
} RouteInfoParcel;

typedef struct MarkMaskParcel {
    int mark;
    int mask;
} MarkMaskParcel;
} // namespace nmd
} // namespace OHOS
#endif // INCLUDE_ROUTE_TYPE_H
