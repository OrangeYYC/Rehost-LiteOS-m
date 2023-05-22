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

#include "mdns_service_info.h"

namespace OHOS {
namespace NetManagerStandard {

namespace {
constexpr uint8_t MDNS_TXT_KEY_LENGTH_MASK = 0xff;
constexpr size_t MDNS_TXT_KEY_LENGTH_LIMIT = 9;
constexpr uint16_t MDNS_TXT_KEY_RANGE_LOWER = 0x20;
constexpr uint16_t MDNS_TXT_KEY_RANGE_UPPER = 0x7e;
constexpr char MDNS_TXT_KV_EQ = '=';
} // namespace

bool MDnsServiceInfo::Marshalling(Parcel &parcel) const
{
    return parcel.WriteString(name) && parcel.WriteString(type) && parcel.WriteInt32(family) &&
           parcel.WriteString(addr) && parcel.WriteInt32(port) && parcel.WriteUInt8Vector(txtRecord);
}

bool MDnsServiceInfo::Marshalling(Parcel &data, const sptr<MDnsServiceInfo> &obj)
{
    return (obj != nullptr) && obj->Marshalling(data);
}

sptr<MDnsServiceInfo> MDnsServiceInfo::Unmarshalling(Parcel &parcel)
{
    sptr<MDnsServiceInfo> ptr = new (std::nothrow) MDnsServiceInfo;
    if (ptr == nullptr) {
        return nullptr;
    }
    bool allOK = parcel.ReadString(ptr->name) && parcel.ReadString(ptr->type) && parcel.ReadInt32(ptr->family) &&
                 parcel.ReadString(ptr->addr) && parcel.ReadInt32(ptr->port) && parcel.ReadUInt8Vector(&ptr->txtRecord);
    return allOK ? ptr : nullptr;
}

bool MDnsServiceInfo::IsKeyValueVaild(const ::std::string &key, const std::vector<uint8_t> &value)
{
    if (key.length() == 0) {
        return false;
    }
    if (key.length() > MDNS_TXT_KEY_LENGTH_LIMIT) {
        NETMGR_EXT_LOG_W("Key lengths > 9 are discouraged: %{public}s", key.c_str());
    }
    for (size_t i = 0; i < key.size(); ++i) {
        char character = key[i];
        if (character < MDNS_TXT_KEY_RANGE_LOWER || character > MDNS_TXT_KEY_RANGE_UPPER) {
            return false;
        }
        if (character == MDNS_TXT_KV_EQ) {
            return false;
        }
    }
    return key.length() + value.size() < MDNS_TXT_KEY_LENGTH_MASK;
}

TxtRecord MDnsServiceInfo::GetAttrMap()
{
    TxtRecord map;
    size_t pos = 0;
    while (pos < txtRecord.size()) {
        size_t recordLen = txtRecord[pos] & MDNS_TXT_KEY_LENGTH_MASK;
        pos += 1;
        if (recordLen == 0) {
            NETMGR_EXT_LOG_W("Zero sized txt record detected");
        } else if (pos + recordLen > txtRecord.size()) {
            NETMGR_EXT_LOG_W("Bad record size");
            recordLen = txtRecord.size() - pos;
        }
        std::string key;
        std::vector<uint8_t> value;
        auto start = txtRecord.begin() + static_cast<int>(pos);
        auto cur = std::find(start, start + static_cast<int>(recordLen), MDNS_TXT_KV_EQ);
        key = std::string(start, cur);
        ++cur;
        if (cur < start + static_cast<int>(recordLen)) {
            value = std::vector<uint8_t>(cur, start + static_cast<int>(recordLen));
        }
        if (map.count(key) != 0) {
            continue;
        }
        if (!IsKeyValueVaild(key, value)) {
            NETMGR_EXT_LOG_W("Key -> value is not valid");
            continue;
        }
        map[key] = value;
        pos += recordLen;
    }
    return map;
}

void MDnsServiceInfo::SetAttrMap(const TxtRecord &map)
{
    txtRecord.clear();
    for (const auto &[key, value] : map) {
        if (!IsKeyValueVaild(key, value)) {
            NETMGR_EXT_LOG_W("Key -> value is not valid");
            continue;
        }
        txtRecord.emplace_back((key.size() + value.size() + 1) & MDNS_TXT_KEY_LENGTH_MASK);
        txtRecord.insert(txtRecord.end(), key.begin(), key.end());
        txtRecord.emplace_back(MDNS_TXT_KV_EQ);
        txtRecord.insert(txtRecord.end(), value.begin(), value.end());
    }
}

} // namespace NetManagerStandard
} // namespace OHOS