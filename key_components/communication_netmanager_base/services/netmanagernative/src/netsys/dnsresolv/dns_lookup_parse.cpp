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

#include "dns_lookup_parse.h"

#include <net/if.h>

#include "fwmark_client.h"
#include "net_manager_constants.h"

namespace OHOS {
namespace nmd {
using namespace NetManagerStandard;
static constexpr int32_t HOST_BIT_SIZE = 0x80;

static constexpr int32_t MAX_SPACE_SIZE = 254;
static constexpr int32_t INVALID_LENGTH = -1;
static constexpr int32_t STEP_SIZE = 2;
static constexpr uint8_t MASK_HIGH_TWO_BITS = 0xc0;
static constexpr uint8_t MASK_LOW_SIX_BITS = 0x3f;
static constexpr uint8_t BIT_NUM_OF_BYTE = 8;
static constexpr uint32_t HOST_NAME_LEN = 1;
static constexpr uint32_t HOST_NAME_LEN_MAX = 62;
static constexpr uint64_t MAX_BIT = 65536;
static constexpr int32_t NAME_MAX_BIT = 256;

static constexpr int32_t TIMEMSTOS = 1000;
static constexpr int32_t TIMEUSTOS = 1000000;
static constexpr int32_t DEFAULT_TIME_OUT = 5;
static constexpr int32_t DEFAULT_DOTS = 1;
static constexpr int32_t DEFAULT_ATTEMPTS = 2;
static constexpr int32_t DEFAULT_MAX_ATTEMPTS = 10;
static constexpr int32_t MINE_CLASS_MAX = 255;
static constexpr int32_t ONE_MINUTE = 60;
static constexpr int32_t HOSTNAME_LEN_DIFFER = 17;
static constexpr int32_t HOSTNAME_SIZE_DIFFER = 13;
static constexpr int32_t HOSTNAME_BUFF_COMPUTE = 8;
static constexpr int32_t OP_MAX = 15;
static constexpr int32_t TMP_LINE = 256;
static constexpr int32_t TYPE_MAX = 255;

static constexpr int32_t RR_CNAME = 5;
static constexpr int32_t CTX_DEFAULT_SCOPEID = 0;

static constexpr int32_t ANSWERS_OPERATION = 15;
static constexpr int32_t DEFAULT_PORT = 53;

static constexpr int32_t ADDR_A6_NOTES_LEN = 12;
static constexpr int32_t RLEN_MAXNS = 12;

static constexpr int32_t COUNT_CONVERT = 256;
static constexpr int32_t COUNT_MAX_LEN = 64;
static constexpr int32_t ANSWER_MAX_LEN = 254;
static constexpr int32_t ANSWER_LEN = 193;
static constexpr int32_t ANSWER_SIZE = 127;
static constexpr int32_t ANSWER_SIZE_DIFFER = 6;
static constexpr int32_t ANSWER_LEN_DIFFER = 10;
static constexpr int32_t ANSWER_STR = 5;
static constexpr int32_t NAME_IS_IPV4 = 1;

static constexpr int32_t MAX_FOR_KEY = 0x10000000;

#ifdef SERVER_SUPPORT_IPV6
static constexpr int32_t HOSTNAME_NOTES_LEN = 10;
static constexpr int32_t NAME_MAX_LEN = 64;
constexpr char SEP = '%';
#endif

int32_t DnsLookUpParse::LookupIpLiteral(struct AddrData buf[ARG_INDEX_1], const std::string name, int32_t family)
{
    if (name[0] == 0) {
        return DNS_ERR_NONE;
    }
    const char *hostName = const_cast<char *>(name.c_str());
    in_addr a4{};
    if (inet_aton(hostName, &a4) > 0) {
        if (family == AF_INET6) {
            return EAI_NONAME;
        }
        if (memcpy_s(&buf[ARG_INDEX_0].addr, sizeof(a4), &a4, sizeof(a4)) != 0) {
            NETNATIVE_LOGE("memcpy_s faild");
            return NETMANAGER_ERR_MEMCPY_FAIL;
        }
        buf[ARG_INDEX_0].family = AF_INET;
        buf[ARG_INDEX_0].scopeid = 0;
        return NAME_IS_IPV4;
    }

#ifdef SERVER_SUPPORT_IPV6
    char tmp[NAME_MAX_LEN] = {0};
    char *p = const_cast<char *>(strchr(hostName, SEP));
    if (p && (p - hostName < NAME_MAX_LEN)) {
        if (memcpy_s(tmp, p - hostName, hostName, p - hostName) != 0) {
            return EAI_AGAIN;
        }
        tmp[p - hostName] = 0;
        hostName = tmp;
    }
    in6_addr a6{};
    if (inet_pton(AF_INET6, hostName, &a6) <= 0) {
        return DNS_ERR_NONE;
    }
    if (family == AF_INET) {
        NETNATIVE_LOGE("family wrong;");
        return EAI_NONAME;
    }

    if (memcpy_s(&buf[ARG_INDEX_0].addr, sizeof(a6), &a6, sizeof(a6)) != 0) {
        NETNATIVE_LOGE("memcpy_s faild");
        return NETMANAGER_ERR_MEMCPY_FAIL;
    }
    buf[ARG_INDEX_0].family = AF_INET6;
    uint64_t scopeid = 0;
    if (p) {
        char *z;
        if (isdigit(*++p)) {
            scopeid = strtoull(p, &z, HOSTNAME_NOTES_LEN);
        } else {
            z = p - 1;
        }
        if (*z) {
            if (!IN6_IS_ADDR_LINKLOCAL(&a6) && !IN6_IS_ADDR_MC_LINKLOCAL(&a6)) {
                return EAI_NONAME;
            }
            scopeid = if_nametoindex(p);
            if (!scopeid) {
                return EAI_NONAME;
            }
        }
        if (scopeid > UINT_MAX) {
            return EAI_NONAME;
        }
    }
    buf[ARG_INDEX_0].scopeid = scopeid;
#endif
    return DNS_ERR_NONE;
}

int32_t DnsLookUpParse::GetResolvConf(struct ResolvConf *conf, char *search, size_t search_sz, uint16_t netId)
{
    (void)search_sz;
    conf->nDots = DEFAULT_DOTS;
    conf->timeOut = DEFAULT_TIME_OUT;
    conf->attempts = DEFAULT_ATTEMPTS;
    if (search) {
        *search = 0;
    }

    uint16_t timeOutMs = 0;
    uint8_t retry = 0;
    std::vector<std::string> nameServers;
    std::vector<std::string> domains;
    int32_t ret = OHOS::DelayedSingleton<OHOS::nmd::DnsParamCache>::GetInstance()->GetResolverConfig(
        netId, nameServers, domains, timeOutMs, retry);
    if (ret < 0) {
        return -1;
    }

    int32_t timeOutSecond = timeOutMs / TIMEMSTOS;
    if (timeOutSecond > 0) {
        if (timeOutSecond >= ONE_MINUTE) {
            conf->timeOut = ONE_MINUTE;
        } else {
            conf->timeOut = static_cast<uint32_t>(timeOutSecond);
        }
    }
    if (retry > 0) {
        if (retry >= DEFAULT_MAX_ATTEMPTS) {
            conf->attempts = DEFAULT_MAX_ATTEMPTS;
        } else {
            conf->attempts = retry;
        }
    }
    uint32_t nns = 0;
    for (auto &nameServer : nameServers) {
        if (LookupIpLiteral(conf->ns + nns, nameServer, AF_UNSPEC) > 0) {
            nns++;
        }
    }
    conf->nns = nns;
    return DNS_ERR_NONE;
}

uint64_t DnsLookUpParse::mTime()
{
    timespec ts{};
    clock_gettime(CLOCK_REALTIME, &ts);
    return static_cast<uint64_t>(ts.tv_sec) * TIMEMSTOS + static_cast<uint64_t>(ts.tv_nsec) / TIMEUSTOS;
}

void DnsLookUpParse::GetNsFromConf(const ResolvConf *conf, uint32_t &nns, int32_t &family, socklen_t &saLen)
{
    for (nns = 0; nns < conf->nns; nns++) {
        const AddrData *ipLit = &conf->ns[nns];
        if (ipLit->family == AF_INET) {
            if (memcpy_s(&nSockAddr[nns].sin.sin_addr, ADDR_A4_LEN, ipLit->addr, ADDR_A4_LEN)) {
                NETNATIVE_LOGE("memcpy_s faild");
                return;
            }
            nSockAddr[nns].sin.sin_port = htons(DEFAULT_PORT);
            nSockAddr[nns].sin.sin_family = AF_INET;
        } else {
            saLen = sizeof(sockAddr.sin6);
            if (memcpy_s(&nSockAddr[nns].sin6.sin6_addr, ADDR_A6_LEN, ipLit->addr, ADDR_A6_LEN) != 0) {
                NETNATIVE_LOGE("memcpy_s faild");
                return;
            }
            nSockAddr[nns].sin6.sin6_port = htons(DEFAULT_PORT);
            nSockAddr[nns].sin6.sin6_scope_id = ipLit->scopeid;
            nSockAddr[nns].sin6.sin6_family = family = AF_INET6;
        }
    }
}

void DnsLookUpParse::SetSocAddr(int32_t fd, uint32_t &nns)
{
    int32_t opt = 0;
    if (setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &opt, sizeof(opt)) == -1) {
        NETNATIVE_LOGE("setsockopt failed error: [%{public}d]", errno);
        return;
    }
    for (uint32_t i = 0; i < nns; i++) {
        if (nSockAddr[i].sin.sin_family != AF_INET) {
            continue;
        }
        int32_t ret = memcpy_s(nSockAddr[i].sin6.sin6_addr.s6_addr + ADDR_A6_NOTES_LEN, ADDR_A4_LEN,
                               &nSockAddr[i].sin.sin_addr, ADDR_A4_LEN);
        ret += memcpy_s(nSockAddr[i].sin6.sin6_addr.s6_addr, ADDR_A6_NOTES_LEN, ADDR_BUF, ADDR_A6_NOTES_LEN);
        if (ret != 0) {
            NETNATIVE_LOGE("memcpy_s faild");
            return;
        }
        nSockAddr[i].sin6.sin6_family = AF_INET6;
        nSockAddr[i].sin6.sin6_flowinfo = 0;
        nSockAddr[i].sin6.sin6_scope_id = 0;
    }
}

void DnsLookUpParse::SearchNameServer(GetAnswers *getAnswers, int32_t *answersLens, const uint8_t *const *queries,
                                      const int32_t *queriesLens)
{
    for (int32_t i = 0; i < getAnswers->queriesNum; i++) {
        if (answersLens[i]) {
            break;
        }
        for (uint32_t j = 0; j < getAnswers->nns; j++) {
            if (sendto(getAnswers->fd, queries[i], queriesLens[i], MSG_NOSIGNAL,
                       reinterpret_cast<sockaddr *>(&nSockAddr[j]), getAnswers->saLen) > 0) {
                break;
            }
        }
    }
}

void DnsLookUpParse::DnsGetAnswers(GetAnswers getAnswers, const uint8_t *const *queries, const int32_t *queriesLens,
                                   uint8_t *const *answers, int32_t *answersLens, int32_t servFailRetry)
{
    socklen_t psl[1] = {getAnswers.saLen};
    int32_t recvLen = 0;
    int32_t next = 0;
    while ((recvLen = recvfrom(getAnswers.fd, answers[next], getAnswers.answersSize, 0,
                               reinterpret_cast<sockaddr *>(&sockAddr), psl)) >= 0) {
        psl[0] = getAnswers.saLen;

        uint32_t j = 0;
        for (; j < getAnswers.nns && memcmp(nSockAddr + j, &sockAddr, getAnswers.saLen); j++)
            ;
        if (j == getAnswers.nns) {
            continue;
        }
        int32_t i = next;
        for (; i < getAnswers.queriesNum && (answers[next][0] != queries[i][0] || answers[next][1] != queries[i][1]);
             i++)
            ;
        if (i == getAnswers.queriesNum || answersLens[i]) {
            continue;
        }

        switch (answers[next][ARG_INDEX_3] & ANSWERS_OPERATION) {
            case ARG_INDEX_0:
            case ARG_INDEX_3:
                break;
            case ARG_INDEX_2:
                if (servFailRetry) {
                    (void)sendto(getAnswers.fd, queries[i], queriesLens[i], MSG_NOSIGNAL,
                                 reinterpret_cast<sockaddr *>(&nSockAddr[j]), getAnswers.saLen);
                    servFailRetry--;
                }
                [[fallthrough]];
            default:
                continue;
        }

        answersLens[i] = recvLen;
        if (i == next) {
            for (; next < getAnswers.queriesNum && answersLens[next]; next++)
                ;
        } else {
            if (memcpy_s(answers[i], recvLen, answers[next], recvLen) != 0) {
                NETNATIVE_LOGE("memcpy_s faild");
                return;
            }
        }

        if (next == getAnswers.queriesNum) {
            return;
        }
    }
}

int32_t DnsLookUpParse::DnsSendQueries(GetAnswers getAnswers, const uint8_t *const *queries, const int32_t *queriesLens,
                                       uint8_t *const *answers, int32_t *answersLens)
{
    pollfd pfd{};
    pfd.fd = getAnswers.fd;
    pfd.events = POLLIN;
    int32_t retryInterval = getAnswers.timeOut / getAnswers.attempts;
    uint64_t time0 = mTime();
    uint64_t time2 = mTime();
    uint64_t time1 = time2 - static_cast<uint64_t>(retryInterval);
    int32_t servFailRetry = 0;
    for (; time2 - time0 < static_cast<uint64_t>(getAnswers.timeOut); time2 = mTime()) {
        if (time2 - time1 >= static_cast<uint64_t>(retryInterval)) {
            SearchNameServer(&getAnswers, answersLens, queries, queriesLens);
            time1 = time2;
            servFailRetry = ARG_INDEX_2 * getAnswers.queriesNum;
        }

        if (poll(&pfd, 1, time1 + retryInterval - time2) <= 0) {
            continue;
        }
        DnsGetAnswers(getAnswers, queries, queriesLens, answers, answersLens, servFailRetry);
    }
    return DNS_ERR_NONE;
}

int32_t DnsLookUpParse::ResMSendRc(int32_t queriesNum, const uint8_t *const *queries, const int32_t *queriesLens,
                                   uint8_t *const *answers, int32_t *answersLens, int32_t answersSize,
                                   const struct ResolvConf *conf, uint16_t netId)
{
    (void)memset_s(static_cast<void *>(&sockAddr), sizeof(sockAddr), 0x00, sizeof(sockAddr));
    for (auto &i : nSockAddr) {
        (void)memset_s(static_cast<void *>(&i), sizeof(sockAddr), 0x00, sizeof(sockAddr));
    }

    int32_t timeOut = TIMEMSTOS * conf->timeOut;
    int32_t attempts = conf->attempts;
    socklen_t saLen = sizeof(sockAddr.sin);
    uint32_t nns = 0;
    int32_t family = AF_INET;
    GetNsFromConf(conf, nns, family, saLen);
    sockAddr.sin.sin_family = family;
    int32_t fd = socket(family, SOCK_DGRAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0);
    if (fd < 0 && family == AF_INET6 && errno == EAFNOSUPPORT) {
        NETNATIVE_LOGE("socker creat or bind error: [%{public}d], %{public}s", errno, strerror(errno));
        fd = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC | SOCK_NONBLOCK, 0);
        family = AF_INET;
    }

    if (FwmarkClient().BindSocket(fd, netId) < 0) {
        NETNATIVE_LOGE("BindSocket error: [%{public}d]", errno);
        return -1;
    }
    if (fd < 0 || bind(fd, reinterpret_cast<sockaddr *>(&sockAddr), saLen) < 0) {
        NETNATIVE_LOGE("socker creat or bind error: [%{public}d], %{public}s", errno, strerror(errno));
        if (fd >= 0) {
            close(fd);
        }
        return -1;
    }

    if (family == AF_INET6) {
        SetSocAddr(fd, nns);
    }

    int32_t answersAllLens = sizeof(*answersLens) * queriesNum;
    (void)memset_s(answersLens, answersAllLens, 0, answersAllLens);

    GetAnswers getAnswers = {
        .queriesNum = queriesNum,
        .answersSize = answersSize,
        .nns = nns,
        .fd = fd,
        .attempts = attempts,
        .timeOut = timeOut,
        .saLen = saLen,
    };
    DnsSendQueries(getAnswers, queries, queriesLens, answers, answersLens);
    (void)close(fd);

    return DNS_ERR_NONE;
}

int32_t DnsLookUpParse::IsValidHostname(const std::string host)
{
    if (strnlen(host.c_str(), HOST_MAX_LEN) - LOOKUP_NAME_ONE >= HOST_MAX_LEN_MINUS_ONE ||
        static_cast<int>(mbstowcs(0, host.c_str(), 0)) == -1) {
        return DNS_ERR_NONE;
    }
    uint8_t *hostBit = reinterpret_cast<uint8_t *>(const_cast<char *>(host.c_str()));

    for (; *hostBit >= HOST_BIT_SIZE || *hostBit == '.' || *hostBit == '-' || isalnum(*hostBit); hostBit++) {
    };
    return !*hostBit;
}

int32_t DnsLookUpParse::DnsParseCallback(void *c, int32_t rr, const void *data, int32_t len, const void *packet)
{
    auto *ctx = static_cast<DpcCtx *>(c);
    if (ctx->cnt >= MAXADDRS) {
        return -1;
    }
    char tmp[TMP_LINE] = {0};
    switch (rr) {
        case RR_A:
            if (len != ADDR_A4_LEN) {
                return -1;
            }
            ctx->addrs[ctx->cnt].family = AF_INET;
            ctx->addrs[ctx->cnt].scopeid = CTX_DEFAULT_SCOPEID;
            if (memcpy_s(ctx->addrs[ctx->cnt++].addr, ADDR_A4_LEN, data, ADDR_A4_LEN) != 0) {
                NETNATIVE_LOGE("memcpy_s faild");
                return NETMANAGER_ERR_MEMCPY_FAIL;
            }
            break;
        case RR_AAAA:
            if (len != ADDR_A6_LEN) {
                return -1;
            }
            ctx->addrs[ctx->cnt].family = AF_INET6;
            ctx->addrs[ctx->cnt].scopeid = CTX_DEFAULT_SCOPEID;
            if (memcpy_s(ctx->addrs[ctx->cnt++].addr, ADDR_A6_LEN, data, ADDR_A6_LEN) != 0) {
                NETNATIVE_LOGE("memcpy_s faild");
                return NETMANAGER_ERR_MEMCPY_FAIL;
            }
            break;
        case RR_CNAME:
            if (DnsLookUpParse().DnsExpand(static_cast<uint8_t *>(const_cast<void *>(packet)),
                                           static_cast<const uint8_t *>(packet) + PACKET_LINE,
                                           static_cast<uint8_t *>(const_cast<void *>(data)), tmp, sizeof(tmp)) > 0 &&
                IsValidHostname(tmp)) {
                if (strcpy_s(ctx->canon, sizeof(tmp), tmp) != 0) {
                    return EAI_AGAIN;
                }
            }
            break;
    }
    return DNS_ERR_NONE;
}

int32_t DnsLookUpParse::DnsParse(const uint8_t *answers, int32_t answersLen,
                                 int32_t (*callback)(void *, int, const void *, int, const void *), void *ctx)
{
    if (answersLen < RLEN_MAXNS) {
        NETNATIVE_LOGE("answersLen is less than %{public}d", RLEN_MAXNS);
        return -1;
    }
    if ((answers[ARG_INDEX_3] & ANSWERS_OPERATION)) {
        return DNS_ERR_NONE;
    }
    const uint8_t *anSize = answers + RLEN_MAXNS;
    int32_t qdCount = answers[ARG_INDEX_4] * COUNT_CONVERT + answers[ARG_INDEX_5];
    int32_t anCount = answers[ARG_INDEX_6] * COUNT_CONVERT + answers[ARG_INDEX_7];
    if (qdCount + anCount > COUNT_MAX_LEN) {
        NETNATIVE_LOGE("get data count greater than %{public}d", COUNT_MAX_LEN);
        return -1;
    }
    while (qdCount--) {
        while ((anSize - answers < answersLen) && (*anSize - 1U < ANSWER_SIZE)) {
            anSize++;
        }
        if (*anSize > ANSWER_LEN || (*anSize == ANSWER_LEN && anSize[ARG_INDEX_1] > ANSWER_MAX_LEN) ||
            anSize > answers + answersLen - ANSWER_SIZE_DIFFER) {
            NETNATIVE_LOGE("qdCount error");
            return -1;
        }
        anSize += ANSWER_STR + !!*anSize;
    }
    while (anCount--) {
        while (anSize - answers < answersLen && *anSize - 1U < ANSWER_SIZE) {
            anSize++;
        }
        if (*anSize > ANSWER_LEN || (*anSize == ANSWER_LEN && anSize[ARG_INDEX_1] > ANSWER_MAX_LEN) ||
            anSize > answers + answersLen - ANSWER_SIZE_DIFFER) {
            return -1;
        }
        anSize += 1 + !!*anSize;
        int32_t len = anSize[ARG_INDEX_8] * COUNT_CONVERT + anSize[ARG_INDEX_9];
        if (anSize + len > answers + answersLen) {
            NETNATIVE_LOGE("anSize has greater than answers len");
            return -1;
        }
        if (callback(ctx, anSize[ARG_INDEX_1], anSize + ANSWER_LEN_DIFFER, len, answers) < 0) {
            NETNATIVE_LOGE("DnsParseCallback failed");
            return -1;
        }
        anSize += ANSWER_LEN_DIFFER + len;
    }
    return DNS_ERR_NONE;
}

int32_t DnsLookUpParse::DnsExpand(const uint8_t *base, const uint8_t *end, const uint8_t *src, char *dest,
                                  int32_t space)
{
    const uint8_t *snapSrc = src;
    if (snapSrc == end || space <= 0) {
        NETNATIVE_LOGE("snapSrc is null");
        return INVALID_LENGTH;
    }
    int32_t len = INVALID_LENGTH;
    char *dBegin = dest;
    char *dEnd = dest + (space > MAX_SPACE_SIZE ? MAX_SPACE_SIZE : space);
    for (int32_t i = 0; i < end - base; i += STEP_SIZE) {
        if (*snapSrc & MASK_HIGH_TWO_BITS) {
            if (snapSrc + 1 == end) {
                NETNATIVE_LOGE("snapSrc is invalid");
                return INVALID_LENGTH;
            }
            int32_t j = ((snapSrc[ARG_INDEX_0] & MASK_LOW_SIX_BITS) << BIT_NUM_OF_BYTE) | snapSrc[1];
            if (len < 0) {
                len = static_cast<int>(snapSrc + STEP_SIZE - src);
            }
            if (j >= end - base) {
                NETNATIVE_LOGE("space length error");
                return INVALID_LENGTH;
            }
            snapSrc = base + j;
        } else if (*snapSrc) {
            if (dest != dBegin) {
                *dest = DOT;
                ++dest;
            }
            int32_t j = *snapSrc;
            ++snapSrc;
            if (j >= end - snapSrc || j >= dEnd - dest) {
                NETNATIVE_LOGE("space length error");
                return INVALID_LENGTH;
            }
            while (j) {
                *dest = static_cast<char>(*snapSrc);
                ++dest;
                ++snapSrc;
                --j;
            }
        } else {
            *dest = 0;
            if (len < 0) {
                len = static_cast<int>(snapSrc + 1 - src);
            }
            return len;
        }
    }
    return INVALID_LENGTH;
}

int32_t DnsLookUpParse::ResMkQuery(int32_t op, const std::string dName, int32_t mineClass, int32_t type,
                                   const uint8_t *data, int32_t dataLen, const uint8_t *newrr, uint8_t *buf,
                                   int32_t bufLen)
{
    size_t nameLen = dName.length() > HOST_MAX_LEN ? HOST_MAX_LEN : dName.length();
    if (nameLen && dName[nameLen - 1] == DOT) {
        nameLen--;
    }
    int32_t nameNum = HOSTNAME_LEN_DIFFER + static_cast<int32_t>(nameLen) + !!nameLen;
    if (nameLen > HOST_MAX_LEN_MINUS_TWO || bufLen < nameNum || op > OP_MAX || mineClass > MINE_CLASS_MAX ||
        type > TYPE_MAX) {
        NETNATIVE_LOGE("make query failed");
        return -1;
    }

    uint8_t snapName[BUFF_MAX_LEN];
    if (memset_s(snapName, BUFF_MAX_LEN, 0, BUFF_MAX_LEN) != 0) {
        return EAI_AGAIN;
    }
    snapName[ARG_INDEX_2] = op * HOSTNAME_BUFF_COMPUTE + 1;
    snapName[ARG_INDEX_5] = 1;
    if (memcpy_s(reinterpret_cast<char *>(snapName) + HOSTNAME_SIZE_DIFFER, nameLen, dName.c_str(), nameLen) != 0) {
        NETNATIVE_LOGE("memcpy_s faild");
        return NETMANAGER_ERR_MEMCPY_FAIL;
    }
    uint32_t i = 0;
    uint32_t j = 0;
    for (i = HOSTNAME_SIZE_DIFFER; snapName[i]; i = j + 1) {
        for (j = i; snapName[j] && snapName[j] != DOT; j++) {
        };
        if (j - i - HOST_NAME_LEN > HOST_NAME_LEN_MAX) {
            NETNATIVE_LOGE("make query is to long");
            return -1;
        }
        snapName[i - ARG_INDEX_1] = j - i;
    }
    snapName[i + ARG_INDEX_1] = type;
    snapName[i + ARG_INDEX_3] = mineClass;

    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    int32_t id = ts.tv_nsec + (static_cast<uint64_t>(ts.tv_nsec / MAX_BIT) & MAX_FOR_KEY);
    snapName[ARG_INDEX_0] = id / NAME_MAX_BIT;
    snapName[ARG_INDEX_1] = id;

    if (memcpy_s(buf, nameNum, snapName, nameNum) != 0) {
        NETNATIVE_LOGE("memcpy_s faild");
        return NETMANAGER_ERR_MEMCPY_FAIL;
    }
    return nameNum;
}
} // namespace nmd
} // namespace OHOS
