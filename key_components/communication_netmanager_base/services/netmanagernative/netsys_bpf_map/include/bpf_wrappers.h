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

#ifndef OHOS_NETMANAGERSTANDARD_BPFWRAPPER_H
#define OHOS_NETMANAGERSTANDARD_BPFWRAPPER_H

#include <cerrno>
#include <linux/bpf.h>
#include <linux/unistd.h>
#include <stdint.h>
#include <string>
#include <unistd.h>

#include "netnative_log_wrapper.h"
#include "securec.h"

namespace OHOS {
namespace NetManagerStandard {
static constexpr int IFNAMESIZE = 16;
static constexpr int INVALID = -1;

struct StatsValue {
    uint64_t rxPackets;
    uint64_t rxBytes;
    uint64_t txPackets;
    uint64_t txBytes;
    uint32_t uid;
    std::string iface;

    StatsValue &operator+=(const StatsValue &other)
    {
        rxPackets += other.rxPackets;
        rxBytes += other.rxBytes;
        txPackets += other.txPackets;
        txBytes += other.txBytes;
        return *this;
    }
};

struct StatsInfo {
    uint32_t uid;
    std::string iface;
    StatsValue stats;
};

typedef struct {
    std::string name;
} IfaceName;

template <class Key, class Value> class BpfWrappers {
public:
    BpfWrappers<Key, Value>() = default;

    /**
     * Bpf Syscall
     *
     * @param cmd which command need to execute
     * @param attr union consists of various anonymous structures
     * @return int return the result of executing the command
     */
    static inline int BpfSyscall(int cmd, const bpf_attr &attr)
    {
        int result = syscall(__NR_bpf, cmd, &attr, sizeof(attr));
        NETNATIVE_LOGI("cmd = %{public}d,result = %{public}d", cmd, result);
        if (result < 0) {
            NETNATIVE_LOGE("BpfSyscall: errno = %{public}d,failed : %{public}s", errno, strerror(errno));
            result = -errno;
        }
        return result;
    }

    /**
     * Create A Bpf Map but for test only
     *
     * @param mapType map type
     * @param keySize key size in bytes
     * @param valueSize value size in bytes
     * @param maxEntries maximum number of elements
     * @param mapFlags map flag
     * @return int return a map file descriptor
     */
    static int CreateMap(bpf_map_type mapType, uint32_t keySize, uint32_t valueSize, uint32_t maxEntries,
                         uint32_t mapFlags)
    {
        bpf_attr bpfAttr;
        (void)memset_s(&bpfAttr, sizeof(bpfAttr), 0, sizeof(bpfAttr));
        bpfAttr.map_type = mapType;
        bpfAttr.key_size = keySize;
        bpfAttr.value_size = valueSize;
        bpfAttr.max_entries = maxEntries;
        bpfAttr.map_flags = mapFlags;
        return BpfSyscall(BPF_MAP_CREATE, bpfAttr);
    }

    /**
     * Write Value To Bpf Map
     *
     * @param mapFd map fd
     * @param key the key of Bpf Map
     * @param value the value of Bpf Map
     * @param flags map flag
     * @return int true:write success false:failure
     */
    static int WriteValueToMap(const int mapFd, const Key &key, const Value &value, uint64_t flags)
    {
        bpf_attr bpfAttr;
        (void)memset_s(&bpfAttr, sizeof(bpfAttr), 0, sizeof(bpfAttr));
        bpfAttr.map_fd = BpfFdToU32(mapFd);
        bpfAttr.key = BpfMapKeyToU64(key);
        bpfAttr.value = BpfMapValueToU64(value);
        bpfAttr.flags = flags;
        return BpfSyscall(BPF_MAP_UPDATE_ELEM, bpfAttr);
    }

    /**
     * LookUp Elem From Map
     *
     * @param mapFd map fd
     * @param key the key of Bpf Map
     * @param value the value of Bpf Map
     * @return int true:find success false:failure
     */
    static int LookUpElem(const int mapFd, const Key &key, const Value &value)
    {
        bpf_attr bpfAttr;
        (void)memset_s(&bpfAttr, sizeof(bpfAttr), 0, sizeof(bpfAttr));
        bpfAttr.map_fd = BpfFdToU32(mapFd);
        bpfAttr.key = BpfMapKeyToU64(key);
        bpfAttr.value = BpfMapValueToU64(value);
        return BpfSyscall(BPF_MAP_LOOKUP_ELEM, bpfAttr);
    }

    /**
     * Delete Elem From Map
     *
     * @param mapFd map fd
     * @param key the key of Bpf Map
     * @return int true:delete success false:failure
     */
    static int DeleteElem(const int mapFd, const Key &key)
    {
        bpf_attr bpfAttr;
        (void)memset_s(&bpfAttr, sizeof(bpfAttr), 0, sizeof(bpfAttr));
        bpfAttr.map_fd = BpfFdToU32(mapFd);
        bpfAttr.key = BpfMapKeyToU64(key);
        return BpfSyscall(BPF_MAP_DELETE_ELEM, bpfAttr);
    }

    /**
     * Get the Next Key From Map
     *
     * @param mapFd map fd
     * @param key the key of Bpf Map
     * @param next_key the key of Bpf Map
     * @return int return next key
     */
    static int GetNextKey(const int mapFd, const Key &key, Key &next_key)
    {
        bpf_attr bpfAttr;
        (void)memset_s(&bpfAttr, sizeof(bpfAttr), 0, sizeof(bpfAttr));
        bpfAttr.map_fd = BpfFdToU32(mapFd);
        bpfAttr.key = BpfMapKeyToU64(key);
        bpfAttr.next_key = BpfMapKeyToU64(next_key);
        return BpfSyscall(BPF_MAP_GET_NEXT_KEY, bpfAttr);
    }

    /**
     * Get the First Key From Map
     *
     * @param mapFd map fd
     * @param firstKey the first key of Bpf Map
     * @return int return first key
     */
    static int GetFirstKey(const int mapFd, Key &key)
    {
        return GetNextKey(mapFd, INVALID, key);
    }

    /**
     * Attach Program To Map
     *
     * @param type bpf attach type
     * @param progFd eBPF program to attach
     * @param cgFd container object to attach to
     * @return int true:attach success false:failure
     */
    static int AttachProgram(bpf_attach_type type, const int progFd, const int cgFd)
    {
        bpf_attr bpfAttr;
        (void)memset_s(&bpfAttr, sizeof(bpfAttr), 0, sizeof(bpfAttr));
        bpfAttr.target_fd = BpfFdToU32(cgFd);
        bpfAttr.attach_bpf_fd = BpfFdToU32(progFd);
        bpfAttr.attach_type = type;
        return BpfSyscall(BPF_PROG_ATTACH, bpfAttr);
    }

    /**
     * Detach Program From Map
     *
     * @param type bpf detach type
     * @param cgFd container object to detach to
     * @return int true:detach success false:failure
     */
    static int DetachProgram(bpf_attach_type type, const int cgFd)
    {
        bpf_attr bpfAttr;
        (void)memset_s(&bpfAttr, sizeof(bpfAttr), 0, sizeof(bpfAttr));
        bpfAttr.target_fd = BpfFdToU32(cgFd);
        bpfAttr.attach_type = type;
        return BpfSyscall(BPF_PROG_DETACH, bpfAttr);
    }

    /**
     * Pin Bpf Object To File node
     *
     * @param pathName path the bpf map pinned
     * @param bfdFd bfd fd
     * @return int true:pin success false:failure
     */
    static int BpfObjPin(const std::string &pathName, int bfdFd)
    {
        bpf_attr bpfAttr;
        (void)memset_s(&bpfAttr, sizeof(bpfAttr), 0, sizeof(bpfAttr));
        bpfAttr.pathname = BpfMapPathNameToU64(pathName);
        bpfAttr.bpf_fd = BpfFdToU32(bfdFd);
        return BpfSyscall(BPF_OBJ_PIN, bpfAttr);
    }

    /**
     * Get Bpf Object By PathName
     *
     * @param pathName bpf map path
     * @param fileFlags file flags
     * @return int return map file descriptor
     */
    static int BpfObjGet(const std::string &pathName, uint32_t fileFlags)
    {
        bpf_attr bpfAttr;
        (void)memset_s(&bpfAttr, sizeof(bpfAttr), 0, sizeof(bpfAttr));
        bpfAttr.pathname = BpfMapPathNameToU64(pathName);
        bpfAttr.file_flags = fileFlags;
        return BpfSyscall(BPF_OBJ_GET, bpfAttr);
    }

    /**
     * Get the Map Fd
     *
     * @param pathName bpf map path
     * @param objFlags obj flags
     * @return int return map file descriptor
     */
    static int GetMap(const std::string &pathName, uint32_t objFlags)
    {
        return BpfObjGet(pathName, objFlags);
    }

    /**
     * Get the Map Fd
     *
     * @param pathName bpf map path
     * @return int return map file descriptor
     */
    static int GetRWMap(const std::string &pathName)
    {
        return GetMap(pathName, 0);
    }

    /**
     * Get the Read—Only Map Fd
     *
     * @param pathName bpf map path
     * @return int return map file descriptor
     */
    static int GetROMap(const std::string &pathName)
    {
        return GetMap(pathName, BPF_F_RDONLY);
    }

    /**
     * Get the Write—Only Map Fd
     *
     * @param pathName bpf map path
     * @return int return map file descriptor
     */
    static int GetWOMap(const std::string &pathName)
    {
        return GetMap(pathName, BPF_F_WRONLY);
    }

private:
    static inline __u32 BpfFdToU32(const int mapFd)
    {
        return static_cast<__u32>(mapFd);
    }

    static inline uint64_t BpfMapPathNameToU64(const std::string &pathName)
    {
        return static_cast<uint64_t>(reinterpret_cast<uintptr_t>(pathName.c_str()));
    }

    static inline uint64_t BpfMapKeyToU64(const Key &key)
    {
        return static_cast<uint64_t>(reinterpret_cast<uintptr_t>(&key));
    }

    static inline uint64_t BpfMapValueToU64(const Value &value)
    {
        return static_cast<uint64_t>(reinterpret_cast<uintptr_t>(&value));
    }
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // OHOS_NETMANAGERSTANDARD_BPFWRAPPER_H
