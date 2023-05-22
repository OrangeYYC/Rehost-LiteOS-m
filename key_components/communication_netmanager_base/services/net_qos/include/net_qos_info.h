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

#ifndef COMMUNICATION_NET_QOS_INFO_H
#define COMMUNICATION_NET_QOS_INFO_H

#include "parcel.h"

namespace OHOS {
namespace NetManagerStandard {
struct NetQosInfo : public Parcelable {
    enum {
        QOS_LEVEL_1 = 1,
        QOS_LEVEL_2 = 2,
        QOS_LEVEL_3 = 3,
        QOS_LEVEL_4 = 4,
        QOS_LEVEL_5 = 5,
    };
    bool Marshalling(Parcel &parcel) const override;
    sptr<NetQosInfo> Unmarshalling(OHOS::Parcel &parcel);
    bool operator==(const NetQosInfo &rhs) const;
    bool operator!=(const NetQosInfo &rhs) const;
    uint32_t qosLevel_;
    uint32_t rttTx_;
    uint32_t rttRx_;
    uint32_t rateTx_;
    uint32_t rateRx_;
    uint32_t pkgLossRate_;
};
} // namespace NetManagerStandard
} // namespace OHOS

#endif // COMMUNICATION_NET_QOS_INFO_H
