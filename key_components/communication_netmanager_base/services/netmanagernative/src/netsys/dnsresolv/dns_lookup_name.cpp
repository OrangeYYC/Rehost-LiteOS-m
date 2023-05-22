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

#include "dns_lookup_name.h"

#include "fwmark_client.h"
#include "net_manager_constants.h"

namespace OHOS {
namespace nmd {
using namespace NetManagerStandard;
static constexpr int32_t HOST_NAME_LEN = 10;
static constexpr int32_t TEMPORARY_FAILURE = 2;
static constexpr int32_t NO_ERROR = 3;

static constexpr uint32_t ADDR_SECOND_LAST_BIT = 15;
static constexpr int32_t PREFIX_SIZE = 128;
static constexpr int32_t PREFIX_LEN = 8;
static constexpr int32_t BUFF_NUM = 2;

static constexpr int32_t ALENS_MAX_LEN = 4;
static constexpr int32_t SEARCH_MAX_LEN = 256;

static constexpr int32_t DAS_USABLE = 0x40000000;
static constexpr int32_t DAS_MATCHINGSCOPE = 0x20000000;
static constexpr int32_t DAS_MATCHINGLABEL = 0x10000000;
static constexpr int32_t DAS_PREC_SHIFT = 20;
static constexpr int32_t DAS_SCOPE_SHIFT = 16;
static constexpr int32_t DAS_PREFIX_SHIFT = 8;
static constexpr int32_t DAS_ORDER_SHIFT = 0;
static constexpr int32_t NAMESERVICES_LEN = 12;
static constexpr int32_t ADDR_LEN = 4;
static constexpr int32_t CNT_NUM = 2;

static constexpr int32_t SCOPEOF_RESULT_2 = 2;
static constexpr int32_t SCOPEOF_RESULT_5 = 5;
static constexpr int32_t SCOPEOF_RESULT_14 = 14;
static constexpr int32_t DSCOPE_MAX_LEN = 15;

static const struct policy {
    uint8_t addr[ADDR_A6_LEN];
    uint8_t len;
    uint8_t mask;
    uint8_t prec;
    uint8_t label;
} DEF_POLICY[] = {
    {{'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\1'}, 15, 0xff, 50, 0},
    {"\0\0\0\0\0\0\0\0\0\0\xff\xff", 11, 0xff, 35, 4},
    {"\x20\2", 1, 0xff, 30, 2},
    {"\x20\1", 3, 0xff, 5, 5},
    {"\xfc", 0, 0xfe, 3, 13},
    {"", 0, 0, 40, 1},
};

int32_t DnsLookUpName::NameFromNull(AddrData buf[SECOND_ADDR_IN_BUFF], const std::string name, int32_t family,
                                    int32_t flags)
{
    if (!name.empty()) {
        return DNS_ERR_NONE;
    }
    int32_t cnt = 0;
    if (static_cast<uint32_t>(flags) & AI_PASSIVE) {
        if (family != AF_INET6) {
            buf[cnt++] = (AddrData){.family = AF_INET};
        }
        if (family != AF_INET) {
            buf[cnt++] = (AddrData){.family = AF_INET6};
        }
    } else {
        if (family != AF_INET6) {
            buf[cnt++] = (AddrData){.family = AF_INET, .addr = {127, 0, 0, 1}};
        }
        if (family != AF_INET) {
            buf[cnt] = (AddrData){.family = AF_INET6};
            buf[cnt].addr[ADDR_SECOND_LAST_BIT] = ADDR_FIRST_BIT;
            ++cnt;
        }
    }
    NETNATIVE_LOG_D("NameFromNull cnt %{public}d", cnt);
    return cnt;
}

int32_t DnsLookUpName::NameFromNumeric(AddrData buf[ADDR_FIRST_BIT], const std::string name, int32_t family)
{
    return DnsLookUpParse().LookupIpLiteral(buf, name, family);
}

int32_t DnsLookUpName::NameFromDns(AddrData buf[MAXADDRS], char canon[CANON_LINE], const std::string name,
                                   int32_t family, const ResolvConf *conf, uint16_t netId)
{
    static const struct {
        int32_t af;
        int32_t rr;
    } afrr[BUFF_NUM] = {
        {.af = AF_INET6, .rr = RR_A},
        {.af = AF_INET, .rr = RR_AAAA},
    };
    uint8_t queriesBuf[ARG_INDEX_2][BUFF_MAX_LEN], answersBuf[ARG_INDEX_2][PACKET_LINE];
    int32_t queriesLens[ARG_INDEX_2], answersLens[ARG_INDEX_2];
    int32_t queriesNum = 0;
    for (int32_t i = 0; i < BUFF_NUM; i++) {
        if (family != afrr[i].af) {
            queriesLens[queriesNum] = DnsLookUpParse().ResMkQuery(0, name, 1, afrr[i].rr, nullptr, 0, nullptr,
                                                                  queriesBuf[queriesNum], sizeof(*queriesBuf));
            if (queriesLens[queriesNum] == -1) {
                NETNATIVE_LOGE("NameFromDns failed  to make query");
                return EAI_NONAME;
            }
            queriesNum++;
        }
    }
    const uint8_t *queries[ARG_INDEX_2] = {queriesBuf[ARG_INDEX_0], queriesBuf[ARG_INDEX_1]};
    uint8_t *answers[ARG_INDEX_2] = {answersBuf[ARG_INDEX_0], answersBuf[ARG_INDEX_1]};
    if (DnsLookUpParse().ResMSendRc(queriesNum, queries, queriesLens, answers, answersLens, sizeof(*answersBuf), conf,
                                    netId) < 0) {
        NETNATIVE_LOGE("NameFromDns failed return to send or recv data");
        return EAI_SYSTEM;
    }

    DpcCtx ctx = {.addrs = buf, .canon = canon};
    for (int32_t i = 0; i < queriesNum; i++) {
        DnsLookUpParse().DnsParse(answersBuf[i], answersLens[i], DnsLookUpParse().DnsParseCallback, &ctx);
    }

    if (ctx.cnt) {
        return ctx.cnt;
    }
    if (answersLens[ARG_INDEX_0] < ALENS_MAX_LEN ||
        (answersBuf[ARG_INDEX_0][ARG_INDEX_3] & ADDR_SECOND_LAST_BIT) == TEMPORARY_FAILURE) {
        NETNATIVE_LOGE("NameFromDns failed try again");
        return EAI_AGAIN;
    }
    if ((answersBuf[ARG_INDEX_0][ARG_INDEX_3] & ADDR_SECOND_LAST_BIT) == SERVER_FAILURE) {
        NETNATIVE_LOGE("NameFromDns failed return server failure");
        return EAI_NONAME;
    }
    if ((answersBuf[ARG_INDEX_0][ARG_INDEX_3] & ADDR_SECOND_LAST_BIT) == NO_ERROR) {
        return DNS_ERR_NONE;
    }
    NETNATIVE_LOGE("NameFromDns failed non-recoverable failure in name res.");
    return EAI_FAIL;
}

int32_t DnsLookUpName::NameFromDnsSearch(AddrData buf[MAXADDRS], char canon[CANON_LINE], const std::string name,
                                         int32_t family, uint16_t netId)
{
    char search[SEARCH_MAX_LEN] = {0};
    ResolvConf conf{};
    if (DnsLookUpParse().GetResolvConf(&conf, search, sizeof(search), netId) < 0) {
        NETNATIVE_LOGE("Get resolv from conf failed");
        return EAI_NONAME;
    }
    if (name.empty()) {
        NETNATIVE_LOGE("the param name is empty.");
        return EAI_NONAME;
    }

    size_t nameLen;
    size_t dots;
    for (dots = nameLen = 0; name[nameLen]; nameLen++) {
        if (name[nameLen] == DOT) {
            dots++;
        }
    }
    if (dots >= conf.nDots || name[nameLen - LOOKUP_NAME_ONE] == DOT) {
        *search = LOOKUP_NAME_ZERO;
    }

    if (name[nameLen - LOOKUP_NAME_ONE] == DOT) {
        nameLen--;
    }
    if (!nameLen || name[nameLen - LOOKUP_NAME_ONE] == DOT || nameLen >= SEARCH_MAX_LEN) {
        NETNATIVE_LOGE("name is unknown");
        return EAI_NONAME;
    }
    if (strcpy_s(canon, name.length() + 1, name.c_str()) != 0) {
        return EAI_AGAIN;
    }
    canon[nameLen] = DOT;
    char *pos, *temp;
    for (pos = search; *pos; pos = temp) {
        for (; isspace(*pos); pos++) {
        };
        for (temp = pos; *temp && !isspace(*temp); temp++) {
        };
        if (temp == pos) {
            break;
        }
        if (temp - pos < static_cast<long>(SEARCH_MAX_LEN - LOOKUP_NAME_ONE) - static_cast<long>(nameLen)) {
            uint32_t cannonAddLen = nameLen + LOOKUP_NAME_ONE;
            uint32_t posLen = temp - pos;
            if (memcpy_s(canon + cannonAddLen, posLen, pos, posLen) != 0) {
                NETNATIVE_LOGE("memcpy_s faild");
                return NETMANAGER_ERR_MEMCPY_FAIL;
            }
            canon[temp - pos + LOOKUP_NAME_ONE + nameLen] = LOOKUP_NAME_ZERO;
            int32_t cnt = NameFromDns(buf, canon, canon, family, &conf, netId);
            if (cnt) {
                return cnt;
            }
        }
    }

    canon[nameLen] = LOOKUP_NAME_ZERO;
    return NameFromDns(buf, canon, name, family, &conf, netId);
}

const struct policy *DnsLookUpName::PolicyOf(const in6_addr *in6Addr)
{
    for (int32_t i = 0;; i++) {
        if (memcmp(in6Addr->s6_addr, DEF_POLICY[i].addr, DEF_POLICY[i].len)) {
            continue;
        }
        if ((in6Addr->s6_addr[DEF_POLICY[i].len] & DEF_POLICY[i].mask) != DEF_POLICY[i].addr[DEF_POLICY[i].len]) {
            continue;
        }
        return DEF_POLICY + i;
    }
    return {};
}

int32_t DnsLookUpName::LabelOf(const in6_addr *in6Addr)
{
    return PolicyOf(in6Addr)->label;
}

int32_t DnsLookUpName::ScopeOf(const in6_addr *in6Addr)
{
    if (IN6_IS_ADDR_MULTICAST(in6Addr)) {
        return in6Addr->s6_addr[ARG_INDEX_1] & DSCOPE_MAX_LEN;
    }
    if (IN6_IS_ADDR_LINKLOCAL(in6Addr)) {
        return SCOPEOF_RESULT_2;
    }
    if (IN6_IS_ADDR_LOOPBACK(in6Addr)) {
        return SCOPEOF_RESULT_2;
    }
    if (IN6_IS_ADDR_SITELOCAL(in6Addr)) {
        return SCOPEOF_RESULT_5;
    }
    return SCOPEOF_RESULT_14;
}

int32_t DnsLookUpName::PreFixMatch(const in6_addr *s, const in6_addr *d)
{
    int32_t i;
    for (i = 0; i < PREFIX_SIZE &&
                !((s->s6_addr[i / PREFIX_LEN] ^ d->s6_addr[i / PREFIX_LEN]) & (PREFIX_SIZE >> (i % PREFIX_LEN)));
         i++) {
    };
    return i;
}

int32_t DnsLookUpName::AddrCmp(const void *addrA, const void *addrB)
{
    const auto *a = static_cast<const AddrData *>(addrA);
    const auto *b = static_cast<const AddrData *>(addrB);
    return b->sortKey - a->sortKey;
}

int32_t DnsLookUpName::CheckNameParam(const std::string name, int32_t &flags, int32_t &family, char *canon)
{
    if (!name.empty()) {
        size_t len = name.length() > HOST_MAX_LEN ? HOST_MAX_LEN : name.length();
        if (len - 1 >= HOST_MAX_LEN_MINUS_ONE) {
            NETNATIVE_LOGE("name is too long : %{public}d", static_cast<int32_t>(len));
            return EAI_NONAME;
        }
        canon = const_cast<char *>(name.c_str());
    }

    if (static_cast<uint32_t>(flags) & AI_V4MAPPED) {
        if (family == AF_INET6) {
            family = AF_UNSPEC;
        } else {
            flags -= AI_V4MAPPED;
        }
    }
    return DNS_ERR_NONE;
}

void DnsLookUpName::RefreshBuf(AddrData *buf, int32_t num, int32_t &cnt)
{
    int32_t j = 0;
    for (j = 0; num < cnt; num++) {
        if (buf[num].family == AF_INET6) {
            buf[j++] = buf[num];
        }
    }
    cnt = j;
}

bool DnsLookUpName::UpdateBuf(int32_t flags, int32_t family, AddrData *buf, int32_t &cnt)
{
    if ((static_cast<uint32_t>(flags) & AI_V4MAPPED)) {
        int32_t i = 0;
        if (!(static_cast<uint32_t>(flags) & AI_ALL)) {
            for (; i < cnt && buf[i].family != AF_INET6; i++) {
            };
            if (i < cnt) {
                RefreshBuf(buf, i, cnt);
            }
        }
        for (i = 0; i < cnt; i++) {
            if (buf[i].family != AF_INET) {
                continue;
            }
            int32_t ret = memcpy_s(buf[i].addr + NAMESERVICES_LEN, ADDR_LEN, buf[i].addr, ADDR_LEN);
            ret += memcpy_s(buf[i].addr, NAMESERVICES_LEN, ADDR_BUF, NAMESERVICES_LEN);
            if (ret != 0) {
                NETNATIVE_LOGE("memcpy_s faild");
                return false;
            }
            buf[i].family = AF_INET6;
        }
    }
    if (cnt < CNT_NUM || family == AF_INET) {
        NETNATIVE_LOGE("cnt is less two or there are only IPv4 results, cnt : %{public}d", cnt);
        return false;
    }
    int32_t i = 0;
    for (i = 0; i < cnt; i++) {
        if (buf[i].family != AF_INET) {
            break;
        }
    }
    return i != cnt;
}

void DnsLookUpName::SockAddrCopy(ScokAddrCopy addrBuff, void *da, void *sa, int32_t &dScope, int32_t &preFixLen,
                                 uint32_t &key)
{
    if (!connect(addrBuff.lookUpNameFd, static_cast<sockaddr *>(da), addrBuff.daLen)) {
        key = key | DAS_USABLE;
        int32_t res = getsockname(addrBuff.lookUpNameFd, static_cast<sockaddr *>(sa), &addrBuff.saLen);
        if (res) {
            (void)close(addrBuff.lookUpNameFd);
            return;
        }
        if (addrBuff.family == AF_INET) {
            if (memcpy_s(addrBuff.sa6.sin6_addr.s6_addr + NAMESERVICES_LEN, ADDR_A4_LEN, &addrBuff.sa4.sin_addr,
                         ADDR_A4_LEN) != 0) {
                NETNATIVE_LOGE("memcpy_s faild");
                return;
            }
        }
        if (dScope == ScopeOf(&addrBuff.sa6.sin6_addr)) {
            key = key | DAS_MATCHINGSCOPE;
        }
        if (addrBuff.dLabel == LabelOf(&addrBuff.sa6.sin6_addr)) {
            key = key | DAS_MATCHINGLABEL;
        }
        preFixLen = PreFixMatch(&addrBuff.sa6.sin6_addr, &addrBuff.da6.sin6_addr);
    }
    (void)close(addrBuff.lookUpNameFd);
}

int32_t DnsLookUpName::FindName(AddrData *buf, char *canon, const std::string name, int32_t family, int32_t flags,
                                uint16_t netId)
{
    int32_t cnt = NameFromNull(buf, name, family, flags);
    if (!cnt) {
        cnt = NameFromNumeric(buf, name, family);
    }
    if (!cnt && !(static_cast<uint32_t>(flags) & AI_NUMERICHOST)) {
        cnt = NameFromDnsSearch(buf, canon, name.c_str(), family, netId);
    }
    NETNATIVE_LOG_D("FindName cnt : %{public}d", cnt);
    return cnt;
}

int32_t DnsLookUpName::MemcpySockaddr(sockaddr_in6 &sa6, sockaddr_in6 &da6, sockaddr_in &da4, AddrData *buf,
                                      uint32_t cnt)
{
    int32_t ret = memcpy_s(sa6.sin6_addr.s6_addr, NAMESERVICES_LEN, ADDR_BUF, NAMESERVICES_LEN);
    ret += memcpy_s(da6.sin6_addr.s6_addr + NAMESERVICES_LEN, ADDR_A4_LEN, buf[cnt].addr, ADDR_A4_LEN);
    ret += memcpy_s(da6.sin6_addr.s6_addr, NAMESERVICES_LEN, ADDR_BUF, NAMESERVICES_LEN);
    ret += memcpy_s(da6.sin6_addr.s6_addr + NAMESERVICES_LEN, ADDR_A4_LEN, buf[cnt].addr, ADDR_A4_LEN);
    ret += memcpy_s(&da4.sin_addr, ADDR_A4_LEN, buf[cnt].addr, ADDR_A4_LEN);
    if (ret != 0) {
        NETNATIVE_LOGE("memcpy_s faild");
        return -1;
    }
    return 0;
}

void DnsLookUpName::LookUpNameParam(AddrData *buf, int32_t cnt, int32_t netId)
{
    for (int32_t i = 0; i < cnt; i++) {
        int32_t family = buf[i].family;
        uint32_t key = 0;
        sockaddr_in6 sa6 = {0};
        sockaddr_in6 da6 = {
            .sin6_family = AF_INET6,
            .sin6_port = PORT_NUM,
            .sin6_scope_id = buf[i].scopeid,
        };
        sockaddr_in sa4 = {0};
        sockaddr_in da4 = {.sin_family = AF_INET, .sin_port = PORT_NUM};
        void *sa, *da;
        socklen_t saLen, daLen;
        if (family == AF_INET6) {
            if (memcpy_s(da6.sin6_addr.s6_addr, ADDR_A6_LEN, buf[i].addr, ADDR_A6_LEN) != 0) {
                NETNATIVE_LOGE("memcpy_s faild");
                return;
            }
            da = &da6;
            daLen = sizeof(da6);
            sa = &sa6;
            saLen = sizeof(da6);
        } else {
            if (MemcpySockaddr(sa6, da6, da4, buf, i) < 0) {
                return;
            }
            da = &da4;
            daLen = sizeof(da4);
            sa = &sa4;
            saLen = sizeof(sa4);
        }
        const policy *dPolicy = PolicyOf(&da6.sin6_addr);
        int32_t dScope = ScopeOf(&da6.sin6_addr);
        int32_t dLabel = dPolicy->label;
        uint32_t dPrec = dPolicy->prec;
        int32_t preFixLen = 0;
        int32_t lookUpNameFd = socket(family, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
        if (lookUpNameFd >= 0) {
            ScokAddrCopy addrBuff = {
                .lookUpNameFd = lookUpNameFd,
                .sa6 = sa6,
                .da6 = da6,
                .sa4 = sa4,
                .saLen = saLen,
                .daLen = daLen,
                .dLabel = dLabel,
                .family = family,
            };
            SockAddrCopy(addrBuff, da, sa, dScope, preFixLen, key);
        }
        key |= dPrec << DAS_PREC_SHIFT;
        key |= static_cast<uint32_t>(DSCOPE_MAX_LEN - dScope) << DAS_SCOPE_SHIFT;
        key |= static_cast<uint32_t>(preFixLen) << DAS_PREFIX_SHIFT;
        key |= static_cast<uint32_t>(MAXADDRS - i) << DAS_ORDER_SHIFT;
        buf[i].sortKey = static_cast<int32_t>(key);
    }
}

int32_t DnsLookUpName::LookUpName(AddrData buf[MAXADDRS], char canon[CANON_LINE], const std::string &name,
                                  int32_t family, int32_t flags, uint16_t netId)
{
    *canon = 0;
    int32_t error = CheckNameParam(name, flags, family, canon);
    if (error < 0) {
        return error;
    }
    int32_t cnt = FindName(buf, canon, name, family, flags, netId);
    if (cnt <= 0) {
        NETNATIVE_LOGE("find name failed: %{public}d", cnt);
        return cnt ? cnt : EAI_NONAME;
    }

    if (!UpdateBuf(flags, family, buf, cnt)) {
        return cnt;
    }

    LookUpNameParam(buf, cnt, netId);
    qsort(buf, cnt, sizeof(*buf), AddrCmp);

    return cnt;
}

int32_t DnsLookUpName::SwitchSocketType(int32_t sockType, const std::string name, int32_t &proto, ServData *buf)
{
    switch (sockType) {
        case SOCK_STREAM:
            switch (proto) {
                case 0:
                    proto = IPPROTO_TCP;
                    [[fallthrough]];
                case IPPROTO_TCP:
                    break;
                default:
                    return EAI_SERVICE;
            }
            break;
        case SOCK_DGRAM:
            switch (proto) {
                case 0:
                    proto = IPPROTO_UDP;
                    [[fallthrough]];
                case IPPROTO_UDP:
                    break;
                default:
                    return EAI_SERVICE;
            }
            [[fallthrough]];
        case 0:
            break;
        default:
            if (!name.empty()) {
                return EAI_SERVICE;
            }
            buf[0].port = 0;
            buf[0].proto = proto;
            buf[0].sockType = sockType;
            break;
    }
    return DNS_ERR_NONE;
}

int32_t DnsLookUpName::LookUpServer(ServData buf[MAXSERVS], const std::string name, int32_t proto, int32_t sockType,
                                    int32_t flags)
{
    auto end = std::make_unique<char>(name.length());
    unsigned long port = 0;
    int32_t error = SwitchSocketType(sockType, name, proto, buf);
    if (error < 0) {
        return error;
    }
    const char *serv = (name.length() > 0 ? name.c_str() : nullptr);
    if (serv) {
        if (!*serv) {
            return EAI_SERVICE;
        }
        port = strtoul(serv, reinterpret_cast<char **>(&end), HOST_NAME_LEN);
    }
    if (!*end) {
        if (port > PORT_NUM) {
            return EAI_SERVICE;
        }
        int32_t cnt = 0;
        if (proto != IPPROTO_UDP) {
            buf[cnt].port = port;
            buf[cnt].sockType = SOCK_STREAM;
            buf[cnt++].proto = IPPROTO_TCP;
        }
        if (proto != IPPROTO_TCP) {
            buf[cnt].port = port;
            buf[cnt].sockType = SOCK_DGRAM;
            buf[cnt++].proto = IPPROTO_UDP;
        }
        return cnt;
    }

    if (static_cast<uint32_t>(flags) & AI_NUMERICSERV) {
        return EAI_NONAME;
    }
    return EAI_SERVICE;
}
} // namespace nmd
} // namespace OHOS
