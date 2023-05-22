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

#include "mdns_packet_parser.h"

#include <cstring>

namespace OHOS {
namespace NetManagerStandard {

namespace {

constexpr size_t MDNS_STR_INITIAL_SIZE = 16;

constexpr uint8_t DNS_STR_PTR_U8_MASK = 0xc0;
constexpr uint16_t DNS_STR_PTR_U16_MASK = 0xc000;
constexpr uint16_t DNS_STR_PTR_LENGTH = 0x3f;
constexpr uint8_t DNS_STR_EOL = '\0';

template <class T> void WriteRawData(const T &data, MDnsPayload &payload)
{
    const uint8_t *begin = reinterpret_cast<const uint8_t *>(&data);
    payload.insert(payload.end(), begin, begin + sizeof(T));
}

template <class T> void WriteRawData(const T &data, uint8_t *ptr)
{
    const uint8_t *begin = reinterpret_cast<const uint8_t *>(&data);
    for (size_t i = 0; i < sizeof(T); ++i) {
        ptr[i] = *begin++;
    }
}

template <class T> const uint8_t *ReadRawData(const uint8_t *raw, T &data)
{
    data = *reinterpret_cast<const T *>(raw);
    return raw + sizeof(T);
}

const uint8_t *ReadNUint16(const uint8_t *raw, uint16_t &data)
{
    const uint8_t *tmp = ReadRawData(raw, data);
    data = ntohs(data);
    return tmp;
}

const uint8_t *ReadNUint32(const uint8_t *raw, uint32_t &data)
{
    const uint8_t *tmp = ReadRawData(raw, data);
    data = ntohl(data);
    return tmp;
}

std::string UnDotted(const std::string &name)
{
    return EndsWith(name, MDNS_DOMAIN_SPLITER_STR) ? name.substr(0, name.size() - 1) : name;
}

} // namespace

MDnsMessage MDnsPayloadParser::FromBytes(const MDnsPayload &payload)
{
    MDnsMessage msg;
    errorFlags_ = PARSE_OK;
    pos_ = Parse(payload.data(), payload, msg);
    return msg;
}

MDnsPayload MDnsPayloadParser::ToBytes(const MDnsMessage &msg)
{
    MDnsPayload payload;
    cachedPayload_ = &payload;
    strCacheMap_.clear();
    Serialize(msg, payload);
    cachedPayload_ = nullptr;
    strCacheMap_.clear();
    return payload;
}

const uint8_t *MDnsPayloadParser::Parse(const uint8_t *begin, const MDnsPayload &payload, MDnsMessage &msg)
{
    begin = ParseHeader(begin, payload, msg.header);
    if ((errorFlags_ & PARSE_ERROR) != PARSE_OK) {
        return begin;
    }
    for (int i = 0; i < msg.header.qdcount; ++i) {
        begin = ParseQuestion(begin, payload, msg.questions);
        if ((errorFlags_ & PARSE_ERROR) != PARSE_OK) {
            return begin;
        }
    }
    for (int i = 0; i < msg.header.ancount; ++i) {
        begin = ParseRR(begin, payload, msg.answers);
        if ((errorFlags_ & PARSE_ERROR) != PARSE_OK) {
            return begin;
        }
    }
    for (int i = 0; i < msg.header.nscount; ++i) {
        begin = ParseRR(begin, payload, msg.authorities);
        if ((errorFlags_ & PARSE_ERROR) != PARSE_OK) {
            return begin;
        }
    }
    for (int i = 0; i < msg.header.arcount; ++i) {
        begin = ParseRR(begin, payload, msg.additional);
        if ((errorFlags_ & PARSE_ERROR) != PARSE_OK) {
            return begin;
        }
    }
    return begin;
}

const uint8_t *MDnsPayloadParser::ParseHeader(const uint8_t *begin, const MDnsPayload &payload,
                                              DNSProto::Header &header)
{
    const uint8_t *end = payload.data() + payload.size();
    if (end - begin < static_cast<int>(sizeof(DNSProto::Header))) {
        errorFlags_ |= PARSE_ERROR_BAD_SIZE;
        return begin;
    }

    begin = ReadNUint16(begin, header.id);
    begin = ReadNUint16(begin, header.flags);
    begin = ReadNUint16(begin, header.qdcount);
    begin = ReadNUint16(begin, header.ancount);
    begin = ReadNUint16(begin, header.nscount);
    begin = ReadNUint16(begin, header.arcount);
    return begin;
}

const uint8_t *MDnsPayloadParser::ParseQuestion(const uint8_t *begin, const MDnsPayload &payload,
                                                std::vector<DNSProto::Question> &questions)
{
    questions.emplace_back();
    begin = ParseDnsString(begin, payload, questions.back().name);
    questions.back().name = UnDotted(questions.back().name);
    if ((errorFlags_ & PARSE_ERROR) != PARSE_OK) {
        questions.pop_back();
        return begin;
    }

    const uint8_t *end = payload.data() + payload.size();
    if (end - begin < sizeof(uint16_t) + sizeof(uint16_t)) {
        errorFlags_ |= PARSE_ERROR_BAD_SIZE;
        questions.pop_back();
        return begin;
    }

    begin = ReadNUint16(begin, questions.back().qtype);
    begin = ReadNUint16(begin, questions.back().qclass);
    return begin;
}

const uint8_t *MDnsPayloadParser::ParseRR(const uint8_t *begin, const MDnsPayload &payload,
                                          std::vector<DNSProto::ResourceRecord> &answers)
{
    answers.emplace_back();
    begin = ParseDnsString(begin, payload, answers.back().name);
    answers.back().name = UnDotted(answers.back().name);
    if ((errorFlags_ & PARSE_ERROR) != PARSE_OK) {
        answers.pop_back();
        return begin;
    }

    const uint8_t *end = payload.data() + payload.size();
    if (end - begin < sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint32_t) + sizeof(uint16_t)) {
        errorFlags_ |= PARSE_ERROR_BAD_SIZE;
        answers.pop_back();
        return begin;
    }
    begin = ReadNUint16(begin, answers.back().rtype);
    begin = ReadNUint16(begin, answers.back().rclass);
    begin = ReadNUint32(begin, answers.back().ttl);
    begin = ReadNUint16(begin, answers.back().length);
    return ParseRData(begin, payload, answers.back().rtype, answers.back().length, answers.back().rdata);
}

const uint8_t *MDnsPayloadParser::ParseRData(const uint8_t *begin, const MDnsPayload &payload, int type, int length,
                                             std::any &data)
{
    switch (type) {
        case DNSProto::RRTYPE_A: {
            const uint8_t *end = payload.data() + payload.size();
            if (end - begin < sizeof(in_addr) || length != sizeof(in_addr)) {
                errorFlags_ |= PARSE_ERROR_BAD_SIZE;
                return begin;
            }
            in_addr addr;
            begin = ReadRawData(begin, addr);
            data = addr;
            return begin;
        }
        case DNSProto::RRTYPE_AAAA: {
            const uint8_t *end = payload.data() + payload.size();
            if (end - begin < sizeof(in6_addr) || length != sizeof(in6_addr)) {
                errorFlags_ |= PARSE_ERROR_BAD_SIZE;
                return begin;
            }
            in6_addr addr;
            begin = ReadRawData(begin, addr);
            data = addr;
            return begin;
        }
        case DNSProto::RRTYPE_PTR: {
            std::string str;
            begin = ParseDnsString(begin, payload, str);
            str = UnDotted(str);
            if ((errorFlags_ & PARSE_ERROR) != PARSE_OK) {
                return begin;
            }
            data = str;
            return begin;
        }
        case DNSProto::RRTYPE_SRV: {
            return ParseSrv(begin, payload, data);
        }
        case DNSProto::RRTYPE_TXT: {
            return ParseTxt(begin, payload, length, data);
        }
        default: {
            errorFlags_ |= PARSE_WARNING_BAD_RRTYPE;
            return begin + length;
        }
    }
}

const uint8_t *MDnsPayloadParser::ParseSrv(const uint8_t *begin, const MDnsPayload &payload, std::any &data)
{
    const uint8_t *end = payload.data() + payload.size();
    if (end - begin < sizeof(uint16_t) + sizeof(uint16_t) + sizeof(uint16_t)) {
        errorFlags_ |= PARSE_ERROR_BAD_SIZE;
        return begin;
    }

    DNSProto::RDataSrv srv;
    begin = ReadNUint16(begin, srv.priority);
    begin = ReadNUint16(begin, srv.weight);
    begin = ReadNUint16(begin, srv.port);
    begin = ParseDnsString(begin, payload, srv.name);
    srv.name = UnDotted(srv.name);
    if ((errorFlags_ & PARSE_ERROR) != PARSE_OK) {
        return begin;
    }
    data = srv;
    return begin;
}

const uint8_t *MDnsPayloadParser::ParseTxt(const uint8_t *begin, const MDnsPayload &payload, int length, std::any &data)
{
    const uint8_t *end = payload.data() + payload.size();
    if (end - begin < length) {
        errorFlags_ |= PARSE_ERROR_BAD_SIZE;
        return begin;
    }

    data = TxtRecordEncoded(begin, begin + length);
    return begin + length;
}

const uint8_t *MDnsPayloadParser::ParseDnsString(const uint8_t *begin, const MDnsPayload &payload, std::string &str)
{
    const uint8_t *end = payload.data() + payload.size();
    const uint8_t *p = begin;
    str.reserve(MDNS_STR_INITIAL_SIZE);
    while (p < end) {
        if (*p == 0) {
            return p + 1;
        }
        if (*p <= MDNS_MAX_DOMAIN_LABEL && p + *p < end) {
            str.append(reinterpret_cast<const char *>(p) + 1, *p);
            str.push_back(MDNS_DOMAIN_SPLITER);
            p += (*p + 1);
        } else if ((*p & DNS_STR_PTR_U8_MASK) == DNS_STR_PTR_U8_MASK) {
            uint16_t offset;
            const uint8_t *tmp = ReadNUint16(p, offset);
            offset = offset & ~DNS_STR_PTR_U16_MASK;
            if (offset >= payload.size()) {
                errorFlags_ |= PARSE_ERROR_BAD_STRPTR;
                return begin;
            }
            ParseDnsString(payload.data() + (offset & ~DNS_STR_PTR_U16_MASK), payload, str);
            if ((errorFlags_ & PARSE_ERROR) != PARSE_OK) {
                return begin;
            }
            return tmp;
        } else {
            errorFlags_ |= PARSE_ERROR_BAD_STR;
            return p;
        }
    }
    return p;
}

void MDnsPayloadParser::Serialize(const MDnsMessage &msg, MDnsPayload &payload)
{
    payload.reserve(sizeof(DNSProto::Message));
    DNSProto::Header header = msg.header;
    header.qdcount = msg.questions.size();
    header.ancount = msg.answers.size();
    header.nscount = msg.authorities.size();
    header.arcount = msg.additional.size();
    SerializeHeader(header, msg, payload);
    for (uint16_t i = 0; i < header.qdcount; ++i) {
        SerializeQuestion(msg.questions[i], msg, payload);
    }
    for (uint16_t i = 0; i < header.ancount; ++i) {
        SerializeRR(msg.answers[i], msg, payload);
    }
    for (uint16_t i = 0; i < header.nscount; ++i) {
        SerializeRR(msg.authorities[i], msg, payload);
    }
    for (uint16_t i = 0; i < header.arcount; ++i) {
        SerializeRR(msg.additional[i], msg, payload);
    }
}

void MDnsPayloadParser::SerializeHeader(const DNSProto::Header &header, const MDnsMessage &msg, MDnsPayload &payload)
{
    WriteRawData(htons(header.id), payload);
    WriteRawData(htons(header.flags), payload);
    WriteRawData(htons(header.qdcount), payload);
    WriteRawData(htons(header.ancount), payload);
    WriteRawData(htons(header.nscount), payload);
    WriteRawData(htons(header.arcount), payload);
}

void MDnsPayloadParser::SerializeQuestion(const DNSProto::Question &question, const MDnsMessage &msg,
                                          MDnsPayload &payload)
{
    SerializeDnsString(question.name, msg, payload);
    WriteRawData(htons(question.qtype), payload);
    WriteRawData(htons(question.qclass), payload);
}

void MDnsPayloadParser::SerializeRR(const DNSProto::ResourceRecord &rr, const MDnsMessage &msg, MDnsPayload &payload)
{
    SerializeDnsString(rr.name, msg, payload);
    WriteRawData(htons(rr.rtype), payload);
    WriteRawData(htons(rr.rclass), payload);
    WriteRawData(htonl(rr.ttl), payload);
    size_t lenStart = payload.size();
    WriteRawData(htons(rr.length), payload);
    SerializeRData(rr.rdata, msg, payload);
    uint16_t len = payload.size() - lenStart - sizeof(uint16_t);
    WriteRawData(htons(len), payload.data() + lenStart);
}

void MDnsPayloadParser::SerializeRData(const std::any &rdata, const MDnsMessage &msg, MDnsPayload &payload)
{
    if (std::any_cast<const in_addr>(&rdata)) {
        WriteRawData(*std::any_cast<const in_addr>(&rdata), payload);
    } else if (std::any_cast<const in6_addr>(&rdata)) {
        WriteRawData(*std::any_cast<const in6_addr>(&rdata), payload);
    } else if (std::any_cast<const std::string>(&rdata)) {
        SerializeDnsString(*std::any_cast<const std::string>(&rdata), msg, payload);
    } else if (std::any_cast<const DNSProto::RDataSrv>(&rdata)) {
        const DNSProto::RDataSrv *srv = std::any_cast<const DNSProto::RDataSrv>(&rdata);
        WriteRawData(htons(srv->priority), payload);
        WriteRawData(htons(srv->weight), payload);
        WriteRawData(htons(srv->port), payload);
        SerializeDnsString(srv->name, msg, payload);
    } else if (std::any_cast<TxtRecordEncoded>(&rdata)) {
        const auto *txt = std::any_cast<TxtRecordEncoded>(&rdata);
        payload.insert(payload.end(), txt->begin(), txt->end());
    }
}

void MDnsPayloadParser::SerializeDnsString(const std::string &str, const MDnsMessage &msg, MDnsPayload &payload)
{
    size_t pos = 0;
    while (pos < str.size()) {
        if (cachedPayload_ == &payload && strCacheMap_.find(str.substr(pos)) != strCacheMap_.end()) {
            return WriteRawData(htons(strCacheMap_[str.substr(pos)]), payload);
        }

        size_t nextDot = str.find(MDNS_DOMAIN_SPLITER, pos);
        if (nextDot == std::string::npos) {
            nextDot = str.size();
        }
        uint8_t segLen = (nextDot - pos) & DNS_STR_PTR_LENGTH;
        uint16_t strptr = payload.size();
        WriteRawData(segLen, payload);
        for (int i = 0; i < segLen; ++i) {
            WriteRawData(str[pos + i], payload);
        }
        strCacheMap_[str.substr(pos)] = strptr | DNS_STR_PTR_U16_MASK;
        pos = nextDot + 1;
    }
    WriteRawData(DNS_STR_EOL, payload);
}

uint32_t MDnsPayloadParser::GetError() const
{
    return errorFlags_ & PARSE_ERROR;
}

} // namespace NetManagerStandard
} // namespace OHOS
