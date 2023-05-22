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

#ifndef STATIC_CONFIGURATION_H
#define STATIC_CONFIGURATION_H

#include <string>
#include <vector>

#include "inet_addr.h"
#include "parcel.h"

namespace OHOS {
namespace NetManagerStandard {
struct StaticConfiguration : public Parcelable {
    INetAddr ipAddr_;
    INetAddr route_;
    INetAddr gateway_;
    INetAddr netMask_;
    std::vector<INetAddr> dnsServers_;
    std::string domain_;

    virtual bool Marshalling(Parcel &parcel) const override;
    static sptr<StaticConfiguration> Unmarshalling(Parcel &parcel);
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif