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

#ifndef NETMANAGER_BASE_BPF_SYSCALL_WRAPPER_H
#define NETMANAGER_BASE_BPF_SYSCALL_WRAPPER_H

#include <cerrno>
#include <linux/bpf.h>
#include <linux/unistd.h>
#include <stdint.h>
#include <string>
#include <unistd.h>

#include "securec.h"

namespace OHOS {
namespace Bpf {
template <class Key, class Value> class BpfSyscallWrapper {
public:
    BpfSyscallWrapper<Key, Value>() = default;

    /**
     * Bpf Syscall
     *
     * @param cmd which command need to execute
     * @param attr union consists of various anonymous structures
     * @return int return the result of executing the command
     */
    static inline int BpfSyscall(int cmd, const bpf_attr &attr);

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
                         uint32_t mapFlags);

    /**
     * Write Value To Bpf Map
     *
     * @param mapFd map fd
     * @param key the key of Bpf Map
     * @param value the value of Bpf Map
     * @param flags map flag
     * @return int true:write success false:failure
     */
    static int WriteValueToMap(const int mapFd, const Key &key, const Value &value, uint64_t flags);

    /**
     * LookUp Elem From Map
     *
     * @param mapFd map fd
     * @param key the key of Bpf Map
     * @param value the value of Bpf Map
     * @return int true:find success false:failure
     */
    static int LookUpElem(const int mapFd, const Key &key, const Value &value);

    /**
     * Delete Elem From Map
     *
     * @param mapFd map fd
     * @param key the key of Bpf Map
     * @return int true:delete success false:failure
     */
    static int DeleteElem(const int mapFd, const Key &key);

    /**
     * Get the Next Key From Map
     *
     * @param mapFd map fd
     * @param key the key of Bpf Map
     * @param next_key the key of Bpf Map
     * @return int return next key
     */
    static int GetNextKey(const int mapFd, const Key &key, Key &next_key);

    /**
     * Get the First Key From Map
     *
     * @param mapFd map fd
     * @param firstKey the first key of Bpf Map
     * @return int return first key
     */
    static int GetFirstKey(const int mapFd, Key &key);

    /**
     * Attach Program To Map
     *
     * @param type bpf attach type
     * @param progFd eBPF program to attach
     * @param cgFd container object to attach to
     * @return int true:attach success false:failure
     */
    static int AttachProgram(bpf_attach_type type, const int progFd, const int cgFd);

    /**
     * Detach Program From Map
     *
     * @param type bpf detach type
     * @param cgFd container object to detach to
     * @return int true:detach success false:failure
     */
    static int DetachProgram(bpf_attach_type type, const int cgFd);

    /**
     * Pin Bpf Object To File node
     *
     * @param pathName path the bpf map pinned
     * @param bfdFd bfd fd
     * @return int true:pin success false:failure
     */
    static int BpfObjPin(const std::string &pathName, int bfdFd);

    /**
     * Get Bpf Object By PathName
     *
     * @param pathName bpf map path
     * @param fileFlags file flags
     * @return int return map file descriptor
     */
    static int BpfObjGet(const std::string &pathName, uint32_t fileFlags);

    /**
     * Get the Map Fd
     *
     * @param pathName bpf map path
     * @param objFlags obj flags
     * @return int return map file descriptor
     */
    static int GetMap(const std::string &pathName, uint32_t objFlags);

    /**
     * Get the Map Fd
     *
     * @param pathName bpf map path
     * @return int return map file descriptor
     */
    static int GetRWMap(const std::string &pathName);

    /**
     * Get the Read—Only Map Fd
     *
     * @param pathName bpf map path
     * @return int return map file descriptor
     */
    static int GetROMap(const std::string &pathName);

    /**
     * Get the Write—Only Map Fd
     *
     * @param pathName bpf map path
     * @return int return map file descriptor
     */
    static int GetWOMap(const std::string &pathName);

private:
    static inline uint32_t BpfFdToU32(const int mapFd);

    static inline uint64_t BpfMapPathNameToU64(const std::string &pathName);

    static inline uint64_t BpfMapKeyToU64(const Key &key);

    static inline uint64_t BpfMapValueToU64(const Value &value);
};
} // namespace Bpf
} // namespace OHOS
#endif // NETMANAGER_BASE_BPF_SYSCALL_WRAPPER_H
