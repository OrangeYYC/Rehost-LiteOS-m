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

#ifndef BPF_PROG_LOADER_H
#define BPF_PROG_LOADER_H

#ifdef ENABLE_ELFIO
#include "bpf_utils.h"
#endif

namespace OHOS {
namespace Bpf {
class BpfProgLoader {
public:
    /**
     * Construct a new Bpf Prog Loader object
     *
     */
    BpfProgLoader() = default;

#ifdef ENABLE_ELFIO
    /**
     * Load the bpf progs and pin them to file system.
     *
     * @param event The type of prog.
     * @param prog EBPF bytecodes compiled by LLVM/Clang.
     * @param size The data size of section.
     * @return Returns true on success, false on failure.
     */
    bool LoadAndAttach(const std::string &event, const bpf_insn *prog, int32_t size);

private:
    int32_t BpfLoadProg(bpf_prog_type type, const bpf_insn *insns, size_t insnsCnt, std::string &license,
                        uint32_t kernVersion, std::string &logBuf, size_t logBufSz) const;
    void InitProgAttr(const BpfLoadProgAttr *loadAttr, bpf_attr &attr, const std::string &logBuf,
                      size_t logBufSz) const;
    int32_t BpfLoadProgXattr(const BpfLoadProgAttr *loadAttr, std::string &logBuf, size_t logBufSz) const;

    std::string bpfLogBuf_ = {};
#endif
};
} // namespace Bpf
} // namespace OHOS
#endif // BPF_PROG_LOADER_H
