/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "data_flow_statistics.h"

#include "netsys_controller.h"

namespace OHOS {
namespace NetManagerStandard {
int64_t DataFlowStatistics::GetCellularRxBytes()
{
    return NetsysController::GetInstance().GetCellularRxBytes();
}

int64_t DataFlowStatistics::GetCellularTxBytes()
{
    return NetsysController::GetInstance().GetCellularTxBytes();
}

int64_t DataFlowStatistics::GetAllRxBytes()
{
    return NetsysController::GetInstance().GetAllRxBytes();
}

int64_t DataFlowStatistics::GetAllTxBytes()
{
    return NetsysController::GetInstance().GetAllTxBytes();
}

int64_t DataFlowStatistics::GetUidRxBytes(uint32_t uid)
{
    return NetsysController::GetInstance().GetUidRxBytes(uid);
}

int64_t DataFlowStatistics::GetUidTxBytes(uint32_t uid)
{
    return NetsysController::GetInstance().GetUidTxBytes(uid);
}

int64_t DataFlowStatistics::GetIfaceRxBytes(const std::string &interfaceName)
{
    return NetsysController::GetInstance().GetIfaceRxBytes(interfaceName);
}

int64_t DataFlowStatistics::GetIfaceTxBytes(const std::string &interfaceName)
{
    return NetsysController::GetInstance().GetIfaceTxBytes(interfaceName);
}

int64_t DataFlowStatistics::GetIfaceRxPackets(const std::string &interfaceName)
{
    return NetsysController::GetInstance().GetIfaceRxPackets(interfaceName);
}

int64_t DataFlowStatistics::GetIfaceTxPackets(const std::string &interfaceName)
{
    return NetsysController::GetInstance().GetIfaceTxPackets(interfaceName);
}
} // namespace NetManagerStandard
} // namespace OHOS
