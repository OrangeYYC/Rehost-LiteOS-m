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

#ifndef NET_CAPS_H
#define NET_CAPS_H

#include <set>
#include <string>

#include "net_all_capabilities.h"

namespace OHOS {
namespace NetManagerStandard {
class NetCaps {
public:
    NetCaps() = default;
    ~NetCaps() = default;
    explicit NetCaps(const std::set<NetCap> &caps);

    bool operator==(const NetCaps &netCaps) const;

    /**
     * Determine if a NetCap is valid or not
     *
     * @param cap NetCap to check
     * @return bool cap is valid or not
     */
    bool IsValidNetCap(NetCap cap);

    /**
     * Insert a NetCap
     *
     * @param cap A NetCap to insert
     */
    void InsertNetCap(NetCap cap);

    /**
     * Remove a NetCap
     *
     * @param cap Netcap to remove
     */
    void RemoveNetCap(NetCap cap);

    /**
     * Determine NetCap exist or not
     *
     * @param cap Netcap to check
     * @return bool cap exist or not
     */
    bool HasNetCap(NetCap cap) const;

    /**
     * Determine NetCaps exist or not
     *
     * @param cap Netcaps to check
     * @return bool NetCaps exist or not
     */
    bool HasNetCaps(const std::set<NetCap> &caps) const;

    /**
     * Restorage all Netcap to a std::set<NetCap>
     *
     * @return std::set<NetCap> with all Netcap
     */
    std::set<NetCap> ToSet() const;

private:
    uint32_t caps_{0};
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_CAPS_H