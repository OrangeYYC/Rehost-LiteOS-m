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

#ifndef DNS_LOOKUP_PARSE_H
#define DNS_LOOKUP_PARSE_H

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <features.h>
#include <limits.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syscall.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "dns_param_cache.h"
#include "netnative_log_wrapper.h"
#include "securec.h"
#include "singleton.h"

namespace OHOS {
namespace nmd {
static constexpr int32_t LOOKUP_NAME_ZERO = 0;
static constexpr int32_t LOOKUP_NAME_ONE = 1;

static constexpr int32_t MAXADDRS = 48;
static constexpr int32_t HOSTS_MAXNS = 3;

static constexpr int32_t SERVER_FAILURE = 0;
static constexpr int32_t BUFF_MAX_LEN = 280;
static constexpr int32_t PACKET_LINE = 512;

static constexpr int32_t RR_A = 1;
static constexpr int32_t RR_AAAA = 28;

static constexpr int32_t HOST_MAX_LEN = 255;
static constexpr int32_t HOST_MAX_LEN_MINUS_ONE = 254;
static constexpr int32_t HOST_MAX_LEN_MINUS_TWO = 253;

static constexpr int32_t ADDR_A4_LEN = 4;
static constexpr int32_t ADDR_A6_LEN = 16;

static constexpr int32_t FAMILY_TYPE = 2;

static constexpr uint8_t DOT = '.';
static constexpr const char *ADDR_BUF = "\0\0\0\0\0\0\0\0\0\0\xff\xff";

struct GetAnswers {
    int32_t queriesNum;
    int32_t answersSize;
    uint32_t nns;
    int32_t fd;
    int32_t attempts;
    int32_t timeOut;
    socklen_t saLen;
};

enum {
    ARG_INDEX_0 = 0,
    ARG_INDEX_1,
    ARG_INDEX_2,
    ARG_INDEX_3,
    ARG_INDEX_4,
    ARG_INDEX_5,
    ARG_INDEX_6,
    ARG_INDEX_7,
    ARG_INDEX_8,
    ARG_INDEX_9,
};

struct AddrData {
    int32_t family;
    uint32_t scopeid;
    uint8_t addr[ADDR_A6_LEN];
    int32_t sortKey;
};

struct ServData {
    uint16_t port;
    uint8_t proto;
    uint8_t sockType;
};

struct ResolvConf {
    AddrData ns[HOSTS_MAXNS];
    uint32_t nns;
    uint32_t attempts;
    uint32_t nDots;
    uint32_t timeOut;
};

struct DpcCtx {
    AddrData *addrs;
    char *canon;
    int32_t cnt;
};

enum DnsResultCode {
    DNS_ERR_NONE = 0,
};

union {
    sockaddr_in sin;
    sockaddr_in6 sin6;
} sockAddr = {{0}}, nSockAddr[HOSTS_MAXNS] = {{{0}}};

class DnsLookUpParse {
public:
    DnsLookUpParse() = default;
    ~DnsLookUpParse() = default;

    int32_t LookupIpLiteral(struct AddrData buf[ARG_INDEX_1], const std::string name, int32_t family);
    int32_t GetResolvConf(struct ResolvConf *conf, char *search, size_t search_sz, uint16_t netId);
    int32_t ResMSendRc(int32_t queriesNum, const uint8_t *const *queries, const int *qlens, uint8_t *const *answers,
                       int32_t *alens, int32_t asize, const ResolvConf *conf, uint16_t netId);
    static int32_t DnsParse(const uint8_t *, int32_t, int32_t (*)(void *, int32_t, const void *, int32_t, const void *),
                            void *);
    int32_t DnsExpand(const uint8_t *base, const uint8_t *end, const uint8_t *src, char *dest, int32_t space);
    int32_t ResMkQuery(int32_t op, const std::string dname, int32_t mineClass, int32_t type, const uint8_t *data,
                       int32_t datalen, const uint8_t *newrr, uint8_t *buf, int32_t buflen);
    static int32_t IsValidHostname(const std::string host);
    static int32_t DnsParseCallback(void *c, int32_t rr, const void *data, int32_t len, const void *packet);

private:
    static uint64_t mTime();
    static void GetNsFromConf(const ResolvConf *conf, uint32_t &nns, int32_t &family, socklen_t &saLen);
    void SetSocAddr(int32_t fd, uint32_t &nns);
    void SearchNameServer(GetAnswers *getAnswers, int32_t *answersLens, const uint8_t *const *queries,
                          const int32_t *queriesLens);
    void DnsGetAnswers(GetAnswers getAnswers, const uint8_t *const *queries, const int32_t *queriesLens,
                       uint8_t *const *answers, int32_t *answersLens, int32_t servFailRetry);
    int32_t DnsSendQueries(GetAnswers getAnswers, const uint8_t *const *queries, const int32_t *queriesLens,
                           uint8_t *const *answers, int32_t *answersLens);
};
} // namespace nmd
} // namespace OHOS
#endif // DNS_LOOKUP_PARSE_H
