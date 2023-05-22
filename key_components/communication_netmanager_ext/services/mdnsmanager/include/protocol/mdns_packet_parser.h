/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#ifndef MDNS_PACKET_PARSER_H
#define MDNS_PACKET_PARSER_H

#include <any>
#include <string>
#include <string_view>
#include <vector>

#include <netinet/in.h>

#include "mdns_common.h"

namespace OHOS {
namespace NetManagerStandard {

namespace DNSProto {

constexpr uint16_t HEADER_FLAGS_QR_MASK = 0x8000;
constexpr uint16_t HEADER_FLAGS_OPCODE_MASK = 0x7800;
constexpr uint16_t HEADER_FLAGS_AA_MASK = 0x0400;
constexpr uint16_t HEADER_FLAGS_TC_MASK = 0x0200;
constexpr uint16_t HEADER_FLAGS_RD_MASK = 0x0100;
constexpr uint16_t HEADER_FLAGS_RA_MASK = 0x0080;
constexpr uint16_t HEADER_FLAGS_Z_MASK = 0x0040;
constexpr uint16_t HEADER_FLAGS_AD_MASK = 0x0020;
constexpr uint16_t HEADER_FLAGS_CD_MASK = 0x0010;
constexpr uint16_t HEADER_FLAGS_RCODE_MASK = 0x000f;

constexpr uint16_t HEADER_FLAGS_QR_OFFSET = 15;
constexpr uint16_t HEADER_FLAGS_OPCODE_OFFSET = 11;
constexpr uint16_t HEADER_FLAGS_AA_OFFSET = 10;
constexpr uint16_t HEADER_FLAGS_TC_OFFSET = 9;
constexpr uint16_t HEADER_FLAGS_RD_OFFSET = 8;
constexpr uint16_t HEADER_FLAGS_RA_OFFSET = 7;
constexpr uint16_t HEADER_FLAGS_Z_OFFSET = 6;
constexpr uint16_t HEADER_FLAGS_AD_OFFSET = 5;
constexpr uint16_t HEADER_FLAGS_CD_OFFSET = 4;
constexpr uint16_t HEADER_FLAGS_RCODE_OFFSET = 0;

// QR = 1, AA = 1
static constexpr uint16_t MDNS_ANSWER_FLAGS = 0x8400;

// DNS Header format (https://www.rfc-editor.org/rfc/rfc1035#section-4.1.1)
//                                 1  1  1  1  1  1
//   0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
// |                      ID                       |
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
// |QR|   OpCode  |AA|TC|RD|RA| Z|AD|CD|   RCODE   |
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
// |                QDCOUNT/ZOCOUNT                |
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
// |                ANCOUNT/PRCOUNT                |
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
// |                NSCOUNT/UPCOUNT                |
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
// |                    ARCOUNT                    |
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
struct Header {
    uint16_t id = 0x0;
    uint16_t flags = 0x0;
    uint16_t qdcount = 0x0;
    uint16_t ancount = 0x0;
    uint16_t nscount = 0x0;
    uint16_t arcount = 0x0;
};


// RR Type (https://www.rfc-editor.org/rfc/rfc1035#section-3.2.2)
enum RRType {
    RRTYPE_IGNORE = 0,
    RRTYPE_A = 1,
    RRTYPE_PTR = 12,
    RRTYPE_TXT = 16,
    RRTYPE_AAAA = 28,
    RRTYPE_SRV = 33,
    RRTYPE_ANY = 255
};

// RR Class (https://www.rfc-editor.org/rfc/rfc1035#section-3.2.4)
enum RRClass {
    RRCLASS_IN = 1,
    RRCLASS_ANY = 255
};

// DNS Question format (https://www.rfc-editor.org/rfc/rfc1035#section-4.1.2)
//                                 1  1  1  1  1  1
//   0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
// |                                               |
// /                     QNAME                     /
// /                                               /
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
// |                     QTYPE                     |
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
// |                     QCLASS                    |
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
struct Question {
    std::string name;
    uint16_t qtype;
    uint16_t qclass;
};

// DNS SRV RR format (https://www.rfc-editor.org/rfc/rfc2782)
struct RDataSrv {
    uint16_t priority;
    uint16_t weight;
    uint16_t port;
    std::string name;
};

// DNS RR format (https://www.rfc-editor.org/rfc/rfc1035#section-3.2.1)
//                                 1  1  1  1  1  1
//   0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
// |                                               |
// /                                               /
// /                      NAME                     /
// |                                               |
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
// |                      TYPE                     |
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
// |                     CLASS                     |
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
// |                      TTL                      |
// |                                               |
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
// |                   RDLENGTH                    |
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--|
// /                     RDATA                     /
// /                                               /
// +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
struct ResourceRecord {
    std::string name;
    uint16_t rtype;
    uint16_t rclass;
    uint32_t ttl;
    uint16_t length;
    std::any rdata;
};

// Messages format (https://www.rfc-editor.org/rfc/rfc1035#section-4.1)
struct Message {
    Header header;
    std::vector<Question> questions;
    std::vector<ResourceRecord> answers;
    std::vector<ResourceRecord> authorities;
    std::vector<ResourceRecord> additional;
};

} // namespace DNSProto

using MDnsMessage = DNSProto::Message;

// Parse mDNS Message from UDP payload or serialize DNS Message to bytes
// This parser could record error location
class MDnsPayloadParser {
public:
    enum {
        PARSE_OK = 0,
        PARSE_ERROR = 0xffff,
        PARSE_ERROR_BAD_SIZE = 0x1,
        PARSE_ERROR_BAD_STR = 0x2,
        PARSE_ERROR_BAD_STRPTR = 0x4,
        PARSE_WARNING = 0xffff0000,
        PARSE_WARNING_BAD_ENTRY_SIZE = 0x40000000,
        PARSE_WARNING_BAD_RRTYPE = 0x80000000,
    };

    MDnsPayloadParser() = default;
    ~MDnsPayloadParser() = default;

    /**
     * parse function
     *
     * @param udp payload as bytes
     * @return dns message struct
     */
    MDnsMessage FromBytes(const MDnsPayload &payload);

    /**
     * parse function
     *
     * @param dns message struct
     * @return dns message as bytes for transmission
     */
    MDnsPayload ToBytes(const MDnsMessage &msg);

    /**
     * get parse error function
     * @return all error in uint32, use PARSE_ERROR_* to extract
     */
    uint32_t GetError() const;

private:
    const uint8_t *Parse(const uint8_t *begin, const MDnsPayload &payload, MDnsMessage &msg);
    const uint8_t *ParseHeader(const uint8_t *begin, const MDnsPayload &payload, DNSProto::Header &header);
    const uint8_t *ParseQuestion(const uint8_t *begin, const MDnsPayload &payload,
                                 std::vector<DNSProto::Question> &questions);
    const uint8_t *ParseRR(const uint8_t *begin, const MDnsPayload &payload,
                           std::vector<DNSProto::ResourceRecord> &answers);
    const uint8_t *ParseRData(const uint8_t *begin, const MDnsPayload &payload, int type, int length, std::any &data);
    const uint8_t *ParseSrv(const uint8_t *begin, const MDnsPayload &payload, std::any &data);
    const uint8_t *ParseTxt(const uint8_t *begin, const MDnsPayload &payload, int length, std::any &data);
    const uint8_t *ParseDnsString(const uint8_t *begin, const MDnsPayload &payload, std::string &str);

    void Serialize(const MDnsMessage &msg, MDnsPayload &payload);
    void SerializeHeader(const DNSProto::Header &header, const MDnsMessage &msg, MDnsPayload &payload);
    void SerializeQuestion(const DNSProto::Question &question, const MDnsMessage &msg, MDnsPayload &payload);
    void SerializeRR(const DNSProto::ResourceRecord &rr, const MDnsMessage &msg, MDnsPayload &payload);
    void SerializeRData(const std::any &rdata, const MDnsMessage &msg, MDnsPayload &payload);
    void SerializeDnsString(const std::string &str, const MDnsMessage &msg, MDnsPayload &payload);

    uint32_t errorFlags_ = 0;
    const uint8_t *pos_ = nullptr;
    MDnsPayload *cachedPayload_ = nullptr;
    std::map<std::string, uint16_t> strCacheMap_;
};

} // namespace NetManagerStandard
} // namespace OHOS
#endif /* MDNS_PACKET_PARSER_H */
