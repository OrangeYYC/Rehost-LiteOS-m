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

#ifndef NETMANAGER_BASE_BPF_LOADER_H
#define NETMANAGER_BASE_BPF_LOADER_H

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <functional>
#include <linux/bpf.h>
#include <linux/rtnetlink.h>
#include <linux/types.h>
#include <memory.h>
#include <net/if.h>
#include <string>
#include <sys/mount.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <vector>

#include "securec.h"

#include "elfio/elf_types.hpp"
#include "elfio/elfio.hpp"
#include "elfio/elfio_relocation.hpp"

namespace OHOS {
namespace Bpf {
static constexpr const char *SECTION_NAMES[] = {
    "kprobe/", "kretprobe/", "tracepoint/", "raw_tracepoint/", "xdp",        "perf_event/",       "socket",
    "cgroup/", "sockops",    "sk_skb",      "sk_msg",          "cgroup_skb", "xdp_packet_parser",
};

static constexpr struct {
    const char *event;
    bpf_prog_type progType;
} PROG_TYPES[] = {
    {"socket", BPF_PROG_TYPE_SOCKET_FILTER},
    {"cgroup_skb", BPF_PROG_TYPE_CGROUP_SKB},
    {"xdp_packet_parser", BPF_PROG_TYPE_XDP},
};

enum ELfLoadError {
    ELF_LOAD_ERR_NONE = 0,
    ELF_LOAD_ERR_PATH_INVALID,
    ELF_LOAD_ERR_MOUNT_FAIL,
    ELF_LOAD_ERR_LOAD_FILE_FAIL,
    ELF_LOAD_ERR_GET_VERSION_FAIL,
    ELF_LOAD_ERR_SELECT_LICENSE_AND_VERSION_FAIL,
    ELF_LOAD_ERR_LOAD_MAP_SECTION_FAIL,
    ELF_LOAD_ERR_SET_RLIMIT_FAIL,
    ELF_LOAD_ERR_CREATE_MAP_FAIL,
    ELF_LOAD_ERR_PARSE_RELOCATION_FAIL,
    ELF_LOAD_ERR_LOAD_PROGS_FAIL,
};

struct BpfLoadMapDef {
    unsigned int type; // actual type is bpf_map_type
    unsigned int keySize;
    unsigned int valueSize;
    unsigned int maxEntries;
    unsigned int mapFlags;
    unsigned int innerMapIdx;
    unsigned int numaNode;
};

struct BpfMapData {
    BpfMapData() : fd(0)
    {
        (void)memset_s(&def, sizeof(def), 0, sizeof(def));
    }

    int32_t fd;
    std::string name;
    BpfLoadMapDef def{};
};

class BpfLoader {
public:
    /**
     * Construct a BpfLoader object
     * @param path the .o file which is need to be loaded
     */
    explicit BpfLoader(std::string path);

    /**
     * Load the file specified in path_.
     * @return return ELF_LOAD_ERR_NONE if no error.
     */
    ELfLoadError Load();

private:
    bool CheckPath();

    bool IsPathValid();

    bool LoadElfFile();

    bool IsVersionValid();

    bool SetRlimit();

    bool IsMouted(const std::string &dir);

    bool MountFs();

    bool SetLicenseAndVersion();

    bool LoadElfMapsSection();

    bool CreateMaps();

    bool ApplyRelocation(bpf_insn *insn, ELFIO::section *section);

    bool LoadProg(const std::string &event, const bpf_insn *insn, size_t insnCnt);

    bool ParseRelocation();

    bool LoadProgs();

    int32_t BpfCreateMapNode(const BpfMapData &map);

    int32_t BpfLoadProgram(bpf_prog_type type, const bpf_insn *insns, size_t insnsCnt);

    bpf_prog_type ConvertEventToProgType(const std::string &event);

    std::string path_;
    ELFIO::elfio elfIo_;
    std::string license_;
    int32_t kernVersion_;
    std::vector<BpfMapData> maps_;
    std::function<ELfLoadError()> isPathValid_;
    std::function<ELfLoadError()> mountFs_;
    std::function<ELfLoadError()> loadElfFile_;
    std::function<ELfLoadError()> isVersionValid_;
    std::function<ELfLoadError()> setLicenseAndVersion_;
    std::function<ELfLoadError()> loadElfMapsSection_;
    std::function<ELfLoadError()> setRlimit_;
    std::function<ELfLoadError()> createMaps_;
    std::function<ELfLoadError()> parseRelocation_;
    std::function<ELfLoadError()> loadProgs_;
};
} // namespace Bpf
} // namespace OHOS
#endif /* NETMANAGER_BASE_BPF_LOADER_H */
