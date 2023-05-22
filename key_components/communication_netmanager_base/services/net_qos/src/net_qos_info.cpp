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

#include "net_qos_info.h"

namespace OHOS {
namespace NetManagerStandard {
bool NetQosInfo::Marshalling(OHOS::Parcel &parcel) const
{
    // Write qosLevel_
    if (!parcel.WriteUint32(qosLevel_)) {
        return false;
    }
    // Write rttTx_
    if (!parcel.WriteUint32(rttTx_)) {
        return false;
    }
    // Write rttRx_
    if (!parcel.WriteUint32(rttRx_)) {
        return false;
    }
    // Write rateTx_
    if (!parcel.WriteUint32(rateTx_)) {
        return false;
    }
    // Write rateRx_
    if (!parcel.WriteUint32(rateRx_)) {
        return false;
    }
    // Write pkgLossRate_
    if (!parcel.WriteUint32(pkgLossRate_)) {
        return false;
    }
    return true;
}
sptr<NetQosInfo> NetQosInfo::Unmarshalling(Parcel &parcel)
{
    sptr<NetQosInfo> info = new (std::nothrow) NetQosInfo;
    if (info != nullptr) {
        if (!parcel.ReadUint32(info->qosLevel_) || !parcel.ReadUint32(rttTx_) || !parcel.ReadUint32(rttRx_) ||
            !parcel.ReadUint32(rateTx_) || !parcel.ReadUint32(rateRx_) || !parcel.ReadUint32(pkgLossRate_)) {
            return nullptr;
        }
    }
    return info;
}

bool NetQosInfo::operator==(const NetQosInfo &rhs) const
{
    if (this == &rhs) {
        return true;
    }
    return qosLevel_ == rhs.qosLevel_ && rttTx_ == rhs.rttTx_ && rttRx_ == rhs.rttRx_ && rateTx_ == rhs.rateTx_ &&
           rateRx_ == rhs.rateRx_ && pkgLossRate_ == rhs.pkgLossRate_;
}

bool NetQosInfo::operator!=(const NetQosInfo &rhs) const
{
    return !(rhs == *this);
}
} // namespace NetManagerStandard
} // namespace OHOS
