/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef COMMUNICATIONNETSTACK_TCP_EXTRA_OPTIONS_H
#define COMMUNICATIONNETSTACK_TCP_EXTRA_OPTIONS_H

#include <cstdint>

#include "extra_options_base.h"

namespace OHOS::NetStack {
class TCPExtraOptions final : public ExtraOptionsBase {
private:
    class SocketLinger {
    public:
        SocketLinger();

        ~SocketLinger() = default;

        void SetOn(bool on);

        void SetLinger(uint32_t linger);

        [[nodiscard]] bool IsOn() const;

        [[nodiscard]] uint32_t GetLinger() const;

    private:
        bool on_;
        uint32_t linger_;
    };

public:
    TCPExtraOptions();

    ~TCPExtraOptions() = default;

    void SetKeepAlive(bool keepAlive);

    void SetOOBInline(bool OOBInline);

    void SetTCPNoDelay(bool TCPNoDelay);

    [[nodiscard]] bool IsKeepAlive() const;

    [[nodiscard]] bool IsOOBInline() const;

    [[nodiscard]] bool IsTCPNoDelay() const;

    SocketLinger socketLinger;

private:
    bool keepAlive_;

    bool OOBInline_;

    bool TCPNoDelay_;
};
} // namespace OHOS::NetStack

#endif /* COMMUNICATIONNETSTACK_TCP_EXTRA_OPTIONS_H */
