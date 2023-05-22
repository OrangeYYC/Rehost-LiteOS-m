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

#include "dns_getaddrinfo.h"

#include <netdb.h>

#include "fwmark_client.h"
#include "netnative_log_wrapper.h"
#include "securec.h"

namespace OHOS {
namespace nmd {
static constexpr int32_t CANNO_LEN = 256;
int32_t DnsGetAddrInfo::CheckHints(const AddrInfo &hints)
{
    uint32_t flags = hints.aiFlags;
    const uint32_t mask =
        AI_PASSIVE | AI_CANONNAME | AI_NUMERICHOST | AI_V4MAPPED | AI_ALL | AI_ADDRCONFIG | AI_NUMERICSERV;
    if ((flags & mask) != flags) {
        NETNATIVE_LOGE("flags in hints is Invalid, flags: [%{public}d]", flags);
        return EAI_BADFLAGS;
    }
    uint32_t family = hints.aiFamily;
    switch (family) {
        case AF_INET:
        case AF_INET6:
        case AF_UNSPEC:
            break;
        default:
            NETNATIVE_LOGE("family not supported, family : [%{public}d]", family);
            return EAI_FAMILY;
    }
    return DNS_ERR_NONE;
}

void DnsGetAddrInfo::SwitchFamilyInet(AddrInfo &node, AddrData addrs, uint16_t port)
{
    node.aiAddr.sin.sin_family = AF_INET;
    node.aiAddr.sin.sin_port = htons(port);
    if (memcpy_s(&node.aiAddr.sin.sin_addr, ADDR_A4_LEN, &addrs.addr, ADDR_A4_LEN) != 0) {
        return;
    }
}

void DnsGetAddrInfo::SwitchFamilyInet6(AddrInfo &node, AddrData addrs, uint16_t port)
{
    node.aiAddr.sin6.sin6_family = AF_INET6;
    node.aiAddr.sin6.sin6_port = htons(port);
    node.aiAddr.sin6.sin6_scope_id = addrs.scopeid;
    if (memcpy_s(&node.aiAddr.sin6.sin6_addr, ADDR_A6_LEN, &addrs.addr, ADDR_A6_LEN) != 0) {
        return;
    }
}

void DnsGetAddrInfo::ParseAddr(int32_t nAddrs, int32_t nServs, ServData (&ports)[MAXSERVS], AddrData (&addrs)[MAXADDRS],
                               char *outCanon, int32_t canonLen, std::vector<AddrInfo> &out)
{
    int16_t k = 0;
    for (int32_t i = 0; i < nAddrs; i++) {
        for (int32_t j = 0; j < nServs; j++, k++) {
            AddrInfo info = {};
            info.aiFamily = addrs[i].family;
            info.aiSockType = ports[j].sockType;
            info.aiProtocol = ports[j].proto;
            info.aiAddrLen = addrs[i].family == AF_INET ? static_cast<socklen_t>(sizeof(sockaddr_in))
                                                        : static_cast<socklen_t>(sizeof(sockaddr_in6));
            if (memcpy_s(info.aiCanonName, sizeof(info.aiCanonName), outCanon, canonLen) != 0) {
                return;
            }
            switch (addrs[i].family) {
                case AF_INET:
                    SwitchFamilyInet(info, addrs[i], ports[j].port);
                    break;
                case AF_INET6:
                    SwitchFamilyInet6(info, addrs[i], ports[j].port);
                    break;
                default:
                    break;
            }
            out.emplace_back(info);
        }
    }
}

int32_t DnsGetAddrInfo::GetAddrInfo(const std::string &hostName, const std::string &serverName, const AddrInfo &hints,
                                    uint16_t netId, std::vector<AddrInfo> &res)
{
    (void)this;
    if (hostName.empty() && serverName.empty()) {
        return EAI_NONAME;
    }
    uint32_t family = AF_UNSPEC;
    uint32_t flags = 0;
    uint32_t proto = 0;
    uint32_t sockType = 0;
    if (hints.aiFamily > 0) {
        int32_t error = CheckHints(hints);
        if (error < DNS_ERR_NONE) {
            return error;
        }
        family = hints.aiFamily;
        flags = hints.aiFlags;
        proto = hints.aiProtocol;
        sockType = hints.aiSockType;
    } else {
        family = AF_INET;
    }

    ServData servBuf[MAXSERVS] = {{0}};
    int32_t nServ = DnsLookUpName().LookUpServer(servBuf, serverName, static_cast<int32_t>(proto),
                                                 static_cast<int32_t>(sockType), static_cast<int32_t>(flags));
    if (nServ < 0) {
        NETNATIVE_LOGE("failed to LookupServ %{public}d", nServ);
        return nServ;
    }
    AddrData addrs[MAXADDRS] = {{0}};
    char canon[CANNO_LEN] = {0};
    int32_t nAddrs = DnsLookUpName().LookUpName(addrs, canon, hostName, static_cast<int32_t>(family),
                                                static_cast<int32_t>(flags), netId);
    if (nAddrs < 0) {
        NETNATIVE_LOGE("failed to LookupName %{public}d", nAddrs);
        return nAddrs;
    }

    char *outCanon = canon;
    auto canonLen = strlen(canon);
    ParseAddr(nAddrs, nServ, servBuf, addrs, outCanon, static_cast<int32_t>(canonLen), res);
    return DNS_ERR_NONE;
}
} // namespace nmd
} // namespace OHOS
