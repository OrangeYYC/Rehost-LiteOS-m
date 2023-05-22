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

#ifndef INCLUDE_FWMARK_CLIENT_H
#define INCLUDE_FWMARK_CLIENT_H

#include <cstdint>

#include "fwmark_command.h"

namespace OHOS {
namespace nmd {
class FwmarkClient {
public:
    FwmarkClient();
    ~FwmarkClient();

    /**
     * Binds {@code fd} to the network designated by {@code netId}.
     *
     * @param socket_fd
     * @param netId
     *
     * @return Return the return value of the netsys interface call
     */
    int32_t BindSocket(int32_t fd, uint32_t netId);

private:
    // Sends |data| to the fwmark network, along with |fd| as ancillary data using cmsg(3).
    int32_t Send(FwmarkCommand *data, int32_t fd);
    int32_t HandleError(int32_t ret, int32_t errorCode);
    int32_t socketFd_;
};
} // namespace nmd
} // namespace OHOS
#endif // INCLUDE_FWMARK_CLIENT_H
