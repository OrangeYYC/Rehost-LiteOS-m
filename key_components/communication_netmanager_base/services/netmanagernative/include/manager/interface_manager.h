/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef INCLUDE_INTERFACE_MANAGER_H
#define INCLUDE_INTERFACE_MANAGER_H

#include "interface_type.h"
#include <ostream>
#include <string>
#include <vector>

namespace OHOS {
namespace nmd {
static const uint32_t INTERFACE_ERR_MAX_LEN = 256;

class InterfaceManager {
public:
    InterfaceManager() = default;
    ~InterfaceManager() = default;
    /**
     * Set network device mtu
     *
     * @param interfaceName Network device name
     * @param mtuValue Value of mtu
     * @return Returns 0, set network device mtu successfully, otherwise it will fail
     */
    static int SetMtu(const char *interfaceName, const char *mtuValue);

    /**
     * Get network device mtu
     *
     * @param interfaceName Network device name
     * @return Returns value of mtu
     */
    static int GetMtu(const char *interfaceName);

    /**
     * Add local IP address to network
     *
     * @param interfaceName Network device name
     * @param addr Network IP address
     * @param prefixLen Length of the network number of the subnet mask
     * @return Returns 0, add local IP address to network successfully, otherwise it will fail
     */
    static int AddAddress(const char *interfaceName, const char *addr, int prefixLen);

    /**
     * Delete local IP address to network
     *
     * @param interfaceName Network device name
     * @param addr Network IP address
     * @param prefixLen Length of the network number of the subnet mask
     * @return Returns 0, delete local IP address to network successfully, otherwise it will fail
     */
    static int DelAddress(const char *interfaceName, const char *addr, int prefixLen);

    /**
     * Get the network interface names
     *
     * @return Network interface names
     */
    static std::vector<std::string> GetInterfaceNames();

    /**
     * Get the network interface config
     *
     * @param ifName Network device name
     * @return Interface configuration parcel
     */
    static InterfaceConfigurationParcel GetIfaceConfig(const std::string &ifName);

    /**
     * Set network interface config
     *
     * @param ifaceConfig Interface configuration parcel
     * @return Returns 1, set network interface config successfully, otherwise it will fail
     */
    static int SetIfaceConfig(const nmd::InterfaceConfigurationParcel &ifaceConfig);

    /**
     * Set network interface ip address
     *
     * @param ifaceName Network port device name
     * @param ipAddress Ip address
     * @return Returns 0, set IP address to network successfully, otherwise it will fail
     */
    static int SetIpAddress(const std::string &ifaceName, const std::string &ipAddress);

    /**
     * Set iface up
     *
     * @param ifaceName Network port device name
     * @return Returns 0, set up to network successfully, otherwise it will fail
     */
    static int SetIffUp(const std::string &ifaceName);

private:
    static int ModifyAddress(uint32_t action, const char *interfaceName, const char *addr, int prefixLen);
};
} // namespace nmd
} // namespace OHOS
#endif // INCLUDE_INTERFACE_MANAGER_H
