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

#ifdef ENABLE_ELFIO
#include "bpf_map_creator.h"

#include <algorithm>
#endif

namespace OHOS {
namespace Bpf {
#ifdef ENABLE_ELFIO
bool BpfMapCreator::CreateMaps(BpfMapData *maps, int32_t nrMaps)
{
    for (int32_t i = 0; i < nrMaps; i++) {
        int32_t numaNode = (maps[i].def.mapFlags & BPF_F_NUMA_NODE) ? maps[i].def.numaNode : -1;
        mapFd_[i] = BpfCreateMapNode(static_cast<bpf_map_type>(maps[i].def.type), maps[i].name, maps[i].def.keySize,
                                     maps[i].def.valueSize, maps[i].def.maxEntries, maps[i].def.mapFlags, numaNode);
        if (mapFd_[i] < 0) {
            NETNATIVE_LOGE("Failed to create map %{public}d (%{public}s): %{public}d %{public}s", i,
                           maps[i].name.c_str(), errno, strerror(errno));
            return false;
        }
        maps[i].fd = mapFd_[i];

        std::string mapPinLocation = std::string("/sys/fs/bpf/netsys_") + maps[i].name;
        if (access(mapPinLocation.c_str(), F_OK) == 0) {
            NETNATIVE_LOGI("map: %{public}s has already been pinned", mapPinLocation.c_str());
        } else {
            if (SysBpfObjPin(mapFd_[i], mapPinLocation)) {
                NETNATIVE_LOGE("Failed to pin map: %{public}s, errno = %{public}d", mapPinLocation.c_str(), errno);
                return false;
            }
        }
    }

    return true;
}

int32_t BpfMapCreator::LoadElfMapsSection(BpfMapData *maps, const ELFIO::elfio &elfio) const
{
    ELFIO::section *mapsSection = nullptr;

    const auto &it = std::find_if(elfio.sections.begin(), elfio.sections.end(),
                                  [](const auto &section) { return section->get_name() == "maps"; });

    if (it == elfio.sections.end()) {
        NETNATIVE_LOGE("Failed to get maps section");
        return 0;
    }
    mapsSection = *it;
    auto defs = reinterpret_cast<const BpfLoadMapDef *>(mapsSection->get_data());
    auto nrMaps = mapsSection->get_size() / sizeof(BpfLoadMapDef);
    for (int32_t i = 0; i < nrMaps; i++) {
        maps[i].def = defs[i];
    }

    int32_t mapIndex = 0;
    for (const auto &section : elfio.sections) {
        if (SHT_SYMTAB == section->get_type() || SHT_DYNSYM == section->get_type()) {
            ELFIO::symbol_section_accessor symbols(elfio, section);
            for (ELFIO::Elf_Xword i = 0; i < symbols.get_symbols_num(); i++) {
                std::string name;
                ELFIO::Elf64_Addr value = 0;
                ELFIO::Elf_Xword size = 0;
                unsigned char bind = 0;
                unsigned char type = 0;
                ELFIO::Elf_Half elfSection = 0;
                unsigned char other = 0;
                symbols.get_symbol(i, name, value, size, bind, type, elfSection, other);
                if (type == STT_OBJECT && name.find("_map") != std::string::npos) {
                    maps[mapIndex].name = name;
                    NETNATIVE_LOGI("maps[%{public}d].name = %{public}s", mapIndex, maps[mapIndex].name.c_str());
                    mapIndex++;
                    continue;
                }
            }
        }
    }
    return nrMaps;
}

int32_t BpfMapCreator::BpfCreateMapNode(bpf_map_type mapType, std::string &name, int32_t keySize, int32_t valueSize,
                                        int32_t maxEntries, uint32_t mapFlags, int32_t node) const
{
    BpfCreateMapAttr mapAttr = {};
    mapAttr.name = name;
    mapAttr.mapType = mapType;
    mapAttr.mapFlags = mapFlags;
    mapAttr.keySize = keySize;
    mapAttr.valueSize = valueSize;
    mapAttr.maxEntries = maxEntries;
    if (node >= 0) {
        mapAttr.numaNode = node;
        mapAttr.mapFlags |= BPF_F_NUMA_NODE;
    }

    return BpfCreateMapXattr(&mapAttr);
}

int32_t BpfMapCreator::BpfCreateMapXattr(const BpfCreateMapAttr *createAttr) const
{
    bpf_attr attr;

    (void)memset_s(&attr, sizeof(attr), '\0', sizeof(attr));

    attr.map_type = createAttr->mapType;
    attr.key_size = createAttr->keySize;
    attr.value_size = createAttr->valueSize;
    attr.max_entries = createAttr->maxEntries;
    attr.map_flags = createAttr->mapFlags;
    if (!createAttr->name.empty()) {
        createAttr->name.copy(
            attr.map_name,
            (createAttr->name.size() < (BPF_OBJ_NAME_LEN - 1) ? createAttr->name.size() : (BPF_OBJ_NAME_LEN - 1)));
    }

    attr.numa_node = createAttr->numaNode;
    attr.btf_fd = createAttr->btfFd;
    attr.btf_key_type_id = createAttr->btfKeyTypeId;
    attr.btf_value_type_id = createAttr->btfValueTypeId;
    attr.map_ifindex = createAttr->mapIfindex;
    attr.inner_map_fd = createAttr->innerMapFd;

    return SysBpf(BPF_MAP_CREATE, &attr, sizeof(attr));
}
#endif
} // namespace Bpf
} // namespace OHOS
