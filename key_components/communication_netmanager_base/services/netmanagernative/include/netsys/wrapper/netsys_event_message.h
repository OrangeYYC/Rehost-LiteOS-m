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

#ifndef NETSYS_EVENT_MESSAGE_H
#define NETSYS_EVENT_MESSAGE_H

#include <map>
#include <string>

namespace OHOS {
namespace nmd {
class NetsysEventMessage {
public:
    NetsysEventMessage() = default;
    ~NetsysEventMessage() = default;

    enum class Action {
        UNKNOWN = 0,
        ADD = 1,
        REMOVE = 2,
        CHANGE = 3,
        LINKUP = 4,
        LINKDOWN = 5,
        ADDRESSUPDATE = 6,
        ADDRESSREMOVED = 7,
        RDNSS = 8,
        ROUTEUPDATED = 9,
        ROUTEREMOVED = 10,
    };

    enum class Type {
        ADDRESS = 0,
        ALERT_NAME,
        CSTAMP,
        FLAGS,
        GATEWAY,
        HEX,
        IFINDEX,
        INTERFACE,
        LIFETIME,
        PREFERRED,
        ROUTE,
        SCOPE,
        SERVERS,
        SERVICES,
        TSTAMP,
        UID,
        VALID,
    };

    enum class SubSys {
        UNKNOWN = 0,
        NET,
        QLOG,
        STRICT,
    };

    inline void SetAction(Action action)
    {
        action_ = action;
    }

    inline const Action &GetAction() const
    {
        return action_;
    }

    inline void SetSubSys(const SubSys subSys)
    {
        subSys_ = subSys;
    }

    inline const SubSys &GetSubSys() const
    {
        return subSys_;
    }

    inline void SetSeq(int32_t seq)
    {
        seqNum_ = seq;
    }

    inline int32_t GetSeq() const
    {
        return seqNum_;
    }

    void PushMessage(Type type, const std::string &value);

    const std::string GetMessage(Type type);

    void DumpMessage();

private:
    Action action_ = Action::UNKNOWN;
    SubSys subSys_ = SubSys::UNKNOWN;
    int32_t seqNum_ = 0;
    std::map<Type, std::string> messageMap_;
};
} // namespace nmd
} // namespace OHOS

#endif // NETSYS_EVENT_MESSAGE_H