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

#ifndef NETMANAGER_BASE_HTTP_PROXY_H
#define NETMANAGER_BASE_HTTP_PROXY_H

#include <string>
#include <set>

#include "parcel.h"

namespace OHOS {
namespace NetManagerStandard {
class HttpProxy : public Parcelable {
public:
    HttpProxy();
    HttpProxy(std::string host, uint16_t port, const std::set<std::string> &exclusionList);

    [[nodiscard]] std::string GetHost() const;
    [[nodiscard]] uint16_t GetPort() const;
    [[nodiscard]] std::set<std::string> GetExclusionList() const;
    [[nodiscard]] std::string ToString() const;
    void inline SetHost(std::string &&host)
    {
        host_ = host;
    }
    void inline SetPort(uint16_t port)
    {
        port_ = port;
    }
    void inline SetExclusionList(std::set<std::string> &list)
    {
        exclusionList_ = list;
    }

    bool Marshalling(Parcel &parcel) const override;
    static bool Unmarshalling(Parcel &parcel, HttpProxy &httpProxy);

private:
    std::string host_;
    uint16_t port_;
    std::set<std::string> exclusionList_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif /* NETMANAGER_BASE_HTTP_PROXY_H */
