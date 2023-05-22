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

#ifndef INCLUDE_INTERFACE_TYPE_H
#define INCLUDE_INTERFACE_TYPE_H

#include <string>
#include <vector>
#include <ostream>

namespace OHOS {
namespace nmd {
typedef struct InterfaceConfigurationParcel {
    std::string ifName;
    std::string hwAddr;
    std::string ipv4Addr;
    int prefixLength;
    std::vector<std::string> flags;
    friend std::ostream &operator<<(std::ostream &os, const nmd::InterfaceConfigurationParcel &parcel)
    {
        os << "ifName: " << parcel.ifName << "\n"
           << "hwAddr: " << parcel.hwAddr << "\n"
           << "ipv4Addr: " << parcel.ipv4Addr << "\n"
           << "prefixLength: " << parcel.prefixLength << "\n"
           << "flags: ["
           << "\n";
        for (unsigned long i = 0; i < parcel.flags.size(); i++) {
            os << "  " << parcel.flags[i] << "\n";
        }
        os << "] "
           << "\n";
        return os;
    }
} InterfaceConfigurationParcel;
} // namespace nmd
} // namespace OHOS
#endif // INCLUDE_INTERFACE_TYPE_H
