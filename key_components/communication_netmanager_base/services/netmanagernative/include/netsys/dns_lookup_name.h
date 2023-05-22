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

#ifndef DNS_LOOKUP_NAME_H
#define DNS_LOOKUP_NAME_H

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <features.h>
#include <netdb.h>
#include <netinet/in.h>
#include <resolv.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "dns_lookup_parse.h"
#include "netnative_log_wrapper.h"

namespace OHOS {
namespace nmd {
static constexpr int32_t PORT_NUM = 65535;
static constexpr int32_t SECOND_ADDR_IN_BUFF = 2;
static constexpr int32_t CANON_LINE = 256;
static constexpr int32_t ADDR_FIRST_BIT = 1;
static constexpr int32_t MAXSERVS = 2;

struct AddrInfoBuf {
    addrinfo ai;
    union sa {
        struct sockaddr_in sin;
        struct sockaddr_in6 sin6;
    } sa;
    volatile int32_t lock[1];
    int16_t slot;
    int16_t ref;
};

struct ScokAddrCopy {
    int32_t lookUpNameFd;
    sockaddr_in6 sa6;
    sockaddr_in6 da6;
    sockaddr_in sa4;
    socklen_t saLen;
    socklen_t daLen;
    int32_t dLabel;
    int32_t family;
};

class DnsLookUpName {
public:
    DnsLookUpName() = default;
    ~DnsLookUpName() = default;

    /**
     * look up server
     *
     * @param buf is search server fo port proto sockType
     * @param name hostname
     * @param proto socket protocol
     * @param socktype socket type
     * @param flags how to deal with ip or hostname
     * @return int32_t 0-success or cnt num, <0-failed
     */
    int32_t LookUpServer(ServData buf[MAXSERVS], const std::string name, int32_t proto, int32_t socktype,
                         int32_t flags);

    /**
     * look up name or ip
     *
     * @param buf is search name's data
     * @param canon
     * @param name host name
     * @param family address family
     * @param flags how to deal with ip or hostname
     * @param netId network ID
     * @return int 0-success <0-failed
     */
    int32_t LookUpName(AddrData buf[MAXADDRS], char canon[CANON_LINE], const std::string &name, int32_t family,
                       int32_t flags, uint16_t netId);

private:
    static int32_t NameFromNull(AddrData buf[SECOND_ADDR_IN_BUFF], const std::string name, int32_t family,
                                int32_t flags);
    static int32_t NameFromNumeric(AddrData buf[ADDR_FIRST_BIT], const std::string name, int32_t family);
    static int32_t NameFromDnsSearch(AddrData buf[MAXADDRS], char canon[CANON_LINE], const std::string name,
                                     int32_t family, uint16_t netId);
    static int32_t NameFromDns(AddrData buf[MAXADDRS], char canon[CANON_LINE], const std::string name, int32_t family,
                               const ResolvConf *conf, uint16_t netId);
    static const struct policy *PolicyOf(const in6_addr *in6Addr);
    static int32_t LabelOf(const in6_addr *in6Addr);
    static int32_t ScopeOf(const in6_addr *in6Addr);
    static int32_t PreFixMatch(const in6_addr *s, const in6_addr *d);
    static int32_t AddrCmp(const void *addrA, const void *addrB);
    static int32_t SwitchSocketType(int32_t socktype, const std::string name, int32_t &proto, ServData *buf);
    static int32_t CheckNameParam(const std::string name, int32_t &flags, int32_t &family, char *canon);
    static bool UpdateBuf(int32_t flags, int32_t family, AddrData *buf, int32_t &cnt);
    static void RefreshBuf(AddrData *buf, int32_t num, int32_t &cnt);
    void SockAddrCopy(ScokAddrCopy addrBuff, void *da, void *sa, int32_t &dScope, int32_t &preFixLen, uint32_t &key);
    static int32_t FindName(AddrData *buf, char *canon, const std::string name, int32_t family, int32_t flags,
                            uint16_t netId);
    void LookUpNameParam(AddrData *buf, int32_t cnt, int32_t netId);
    int32_t MemcpySockaddr(sockaddr_in6 &sa6, sockaddr_in6 &da6, sockaddr_in &da4, AddrData *buf, uint32_t cnt);
};
} // namespace nmd
} // namespace OHOS
#endif // DNS_LOOKUP_NAME_H
