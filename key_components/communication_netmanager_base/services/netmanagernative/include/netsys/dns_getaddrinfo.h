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

#ifndef DNS_GETADDRINFO_H
#define DNS_GETADDRINFO_H

#include "dns_lookup_name.h"

#include <endian.h>

namespace OHOS {
namespace nmd {
class DnsGetAddrInfo {
public:
    DnsGetAddrInfo() = default;
    ~DnsGetAddrInfo() = default;

    /**
     * Get the Addr Info object
     *
     * @param host host name
     * @param serv server
     * @param hint limits
     * @param res after look up back data
     * @param netId designated network
     * @return int 0-success, <0-failed
     */
    int32_t GetAddrInfo(const std::string &hostName, const std::string &serverName, const AddrInfo &hints,
                        uint16_t netId, std::vector<AddrInfo> &res);

private:
    static void ParseAddr(int32_t nAddrs, int32_t nServ, ServData (&ports)[MAXSERVS], AddrData (&addrs)[MAXADDRS],
                          char *outCanon, int32_t canonLen, std::vector<AddrInfo> &out);
    static int32_t CheckHints(const AddrInfo &hints);
    static void SwitchFamilyInet(AddrInfo &node, AddrData addrs, uint16_t port);
    static void SwitchFamilyInet6(AddrInfo &node, AddrData addrs, uint16_t port);
};
} // namespace nmd
} // namespace OHOS
#endif // DNS_GETADDRINFO_H
