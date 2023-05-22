/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef NETMANAGER_BASE_BPF_READER_H
#define NETMANAGER_BASE_BPF_READER_H

#include <cerrno>
#include <functional>

#include "bpf_syscall_wrapper.h"

namespace OHOS {
namespace Bpf {
template <class Key, class Value> class NetsysBpfMap {
public:
    NetsysBpfMap<Key, Value>() = default;
    NetsysBpfMap<Key, Value>(const std::string &pathName, uint32_t flags);
    NetsysBpfMap<Key, Value>(bpf_map_type mapType, uint32_t maxEntries, uint32_t mapFlags);

    /**
     * Is has map fd
     *
     * @return bool true:has map fd false:not have
     */
    bool IsValid() const;

    /**
     * Read Value From Map
     *
     * @param key the key of map
     * @return Value value corresponding to key
     */
    Value ReadValueFromMap(const Key key) const;

    /**
     * WriteValue
     *
     * @param key the key want to write
     * @param value the value want to write
     * @param flags map flag
     * @return bool true:write success false:failure
     */
    bool WriteValue(const Key &key, const Value &value, uint64_t flags) const;

    /**
     * Iterate through each element in the map
     *
     * @param counter lambda expression to be executed
     */
    void Iterate(const std::function<void(const Key &key, const NetsysBpfMap<Key, Value> &map)> &counter) const;

    /**
     * Get the Next Key From Map
     *
     * @param curkey current key
     * @return int next Key
     */
    int GetNextKeyFromMap(Key &curkey) const;

    /**
     * Get the Next Key From Stats Map
     *
     * @param curkey current key
     * @param nextKey reference of next Key
     * @return int next Key
     */
    int GetNextKeyFromStatsMap(const Key &curkey, Key &nextKey) const;

    /**
     * BpfMapFdPin
     *
     * @param pathName the path that map needs to pin to
     * @return bool true:pin success false:failure
     */
    bool BpfMapFdPin(const std::string &pathName) const;

    /**
     * DeleteEntryFromMap
     *
     * @param deleteKey the key need to delete
     * @return bool true:delete success false:failure
     */
    bool DeleteEntryFromMap(const Key &deleteKey);

private:
    int32_t mapFd_ = 0;
};
} // namespace Bpf
} // namespace OHOS
#endif // NETMANAGER_BASE_BPF_READER_H
