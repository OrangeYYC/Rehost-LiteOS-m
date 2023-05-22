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

#ifndef BPF_MAP_CREATOR_H
#define BPF_MAP_CREATOR_H

#ifdef ENABLE_ELFIO
#include "bpf_utils.h"
#include "elfio/elfio.hpp"
#endif

namespace OHOS {
namespace Bpf {
constexpr int32_t MAX_MAP_COUNT = 32;

class BpfMapCreator {
public:
    /**
     * Construct a new Bpf Map Creator object
     *
     */
    BpfMapCreator() = default;

#ifdef ENABLE_ELFIO
    /**
     * Create Map nodes and pin them to the file system.
     *
     * @param maps An array that stores Maps.
     * @param len The count of the Maps.
     * @return Returns true on success, false on failure.
     */
    bool CreateMaps(BpfMapData *maps, int32_t len);

    /**
     * Load the maps Section.
     *
     * @param maps The attributes of the map.
     * @param elfio The object of the elfio.
     * @return Returns the number of maps.
     */
    int32_t LoadElfMapsSection(BpfMapData *maps, const ELFIO::elfio &elfio) const;

private:
    /**
     * Create Map nodes.
     *
     * @param mapType The type of the Map.
     * @param name The name of the Map.
     * @param keySize The size of key.
     * @param valueSize The size of value.
     * @param maxEntries The max entries stored in the Map.
     * @param mapFlags The flag of the Map.
     * @param node It's valid only when BPF_F_NUMA_NODE FLAG is set.
     * @return Returns fd of the Map that was created successfully.
     */
    int32_t BpfCreateMapNode(bpf_map_type mapType, std::string &name, int32_t keySize, int32_t valueSize,
                             int32_t maxEntries, uint32_t mapFlags, int32_t node) const;

    /**
     * Construct the attributes that creates the Map.
     *
     * @param createAttr The attributes that creates the Map.
     * @return Returns fd of the Map that was created successfully.
     */
    int32_t BpfCreateMapXattr(const BpfCreateMapAttr *createAttr) const;
#endif

    int32_t mapFd_[MAX_MAP_COUNT] = {};
};
} // namespace Bpf
} // namespace OHOS
#endif // BPF_MAP_CREATOR_H
