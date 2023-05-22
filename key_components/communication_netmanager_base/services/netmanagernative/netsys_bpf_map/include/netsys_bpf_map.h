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

#ifndef OHOS_NETMANAGERSTANDARD_NERSYSBPFMAP_H
#define OHOS_NETMANAGERSTANDARD_NERSYSBPFMAP_H

#include <errno.h>
#include <functional>

#include "bpf_wrappers.h"
#include "netnative_log_wrapper.h"
#include "unique_fd.h"

namespace OHOS {
namespace NetManagerStandard {
template <class Key, class Value> class NetsysBpfMap {
public:
    NetsysBpfMap<Key, Value>() = default;
    NetsysBpfMap<Key, Value>(const std::string &pathName, uint32_t flags)
    {
        int mapFd = BpfWrappers<Key, Value>::GetMap(pathName, flags);
        if (mapFd >= 0) {
            mapFd_ = mapFd;
        }
    }

    NetsysBpfMap<Key, Value>(bpf_map_type mapType, uint32_t maxEntries, uint32_t mapFlags)
    {
        int mapFd = BpfWrappers<Key, Value>::CreateMap(mapType, sizeof(Key), sizeof(Value), maxEntries, mapFlags);
        if (IsValid()) {
            mapFd_ = mapFd;
        }
    }

    /**
     * Is has map fd
     *
     * @return bool true:has map fd false:not have
     */
    bool IsValid() const
    {
        return mapFd_ >= 0;
    }

    /**
     * Read Value From Map
     *
     * @param key the key of map
     * @return Value value corresponding to key
     */
    Value ReadValueFromMap(const Key key) const
    {
        Value value;
        if (BpfWrappers<Key, Value>::LookUpElem(mapFd_, key, value)) {
            NETNATIVE_LOGE("Read Value failed: errno = %{public}d,failed : %{public}s", errno, strerror(errno));
        }
        return value;
    }

    /**
     * WriteValue
     *
     * @param key the key want to write
     * @param value the value want to write
     * @param flags map flag
     * @return bool true:write success false:failure
     */
    bool WriteValue(const Key &key, const Value &value, uint64_t flags) const
    {
        return BpfWrappers<Key, Value>::WriteValueToMap(mapFd_, key, value, flags) == 0;
    }

    /**
     * Iterate through each element in the map
     *
     * @param counter lambda expression to be executed
     */
    void Iterate(const std::function<void(const Key &key, const NetsysBpfMap<Key, Value> &map)> &counter) const
    {
        Key curKey;
        if (BpfWrappers<Key, Value>::GetFirstKey(mapFd_, curKey)) {
            NETNATIVE_LOGE("Get FirstKey failed and curKey is %{public}u", static_cast<uint32_t>(curKey));
            return;
        }
        Key nextKey;
        while (BpfWrappers<Key, Value>::GetNextKey(mapFd_, curKey, nextKey) == 0) {
            counter(curKey, *this);
            curKey = nextKey;
        }
        if (BpfWrappers<Key, Value>::GetNextKey(mapFd_, curKey, nextKey) < 0 && (errno == ENOENT)) {
            counter(curKey, *this);
        }
    }

    /**
     * Get the Next Key From Map
     *
     * @param curkey current key
     * @return int next Key
     */
    int GetNextKeyFromMap(Key &curkey) const
    {
        Key nextKey;
        if (BpfWrappers<Key, Value>::GetNextKey(mapFd_, &curkey, &nextKey)) {
            NETNATIVE_LOGE("Get Next Key failed: errno = %{public}d,failed : %{public}s", errno, strerror(errno));
            return errno;
        }
        return nextKey;
    }

    /**
     * Get the Next Key From Stats Map
     *
     * @param curkey current key
     * @param nextKey reference of next Key
     * @return int next Key
     */
    int GetNextKeyFromStatsMap(const Key &curkey, Key &nextKey) const
    {
        return BpfWrappers<Key, Value>::GetNextKey(mapFd_, curkey, nextKey);
    }

    /**
     * BpfMapFdPin
     *
     * @param pathName the path that map needs to pin to
     * @return bool true:pin success false:failure
     */
    bool BpfMapFdPin(const std::string &pathName) const
    {
        return BpfWrappers<Key, Value>::BpfObjPin(pathName, mapFd_) == 0;
    }

    /**
     * DeleteEntryFromMap
     *
     * @param deleteKey the key need to delete
     * @return bool true:delete success false:failure
     */
    bool DeleteEntryFromMap(const Key &deleteKey)
    {
        return BpfWrappers<Key, Value>::DeleteElem(mapFd_, deleteKey) == 0;
    }

private:
    int32_t mapFd_ = 0;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // OHOS_NETMANAGERSTANDARD_NERSYSBPFMAP_H
