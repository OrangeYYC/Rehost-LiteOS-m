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

#ifndef COMMUNICATIONNETSTACK_SOCKET_STATE_BASE_H
#define COMMUNICATIONNETSTACK_SOCKET_STATE_BASE_H

namespace OHOS::NetStack {
class SocketStateBase final {
public:
    SocketStateBase();

    ~SocketStateBase() = default;

    void SetIsBound(bool isBound);

    void SetIsClose(bool isClose);

    void SetIsConnected(bool isConnected);

    [[nodiscard]] bool IsBound() const;

    [[nodiscard]] bool IsClose() const;

    [[nodiscard]] bool IsConnected() const;

private:
    bool isBound_;

    bool isClose_;

    bool isConnected_;
};
} // namespace OHOS::NetStack

#endif /* COMMUNICATIONNETSTACK_SOCKET_STATE_BASE_H */
