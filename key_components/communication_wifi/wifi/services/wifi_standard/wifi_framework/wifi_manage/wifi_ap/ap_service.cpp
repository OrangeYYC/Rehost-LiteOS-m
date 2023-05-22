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

#include "ap_service.h"
#include <unistd.h>
#include "ap_state_machine.h"
#include "wifi_logger.h"
#include "wifi_settings.h"
#include "wifi_ap_hal_interface.h"

DEFINE_WIFILOG_HOTSPOT_LABEL("WifiApService");
namespace OHOS {
namespace Wifi {
ApService::ApService(ApStateMachine &apStateMachine, int id)
    : m_ApStateMachine(apStateMachine), m_id(id)
{}

ApService::~ApService()
{}

ErrCode ApService::EnableHotspot() const
{
    WIFI_LOGI("Instance %{public}d %{public}s", m_id, __func__);
    m_ApStateMachine.SendMessage(static_cast<int>(ApStatemachineEvent::CMD_START_HOTSPOT));
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode ApService::DisableHotspot() const
{
    WIFI_LOGI("Instance %{public}d %{public}s", m_id, __func__);
    m_ApStateMachine.SendMessage(static_cast<int>(ApStatemachineEvent::CMD_STOP_HOTSPOT));
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode ApService::SetHotspotConfig(const HotspotConfig &cfg) const
{
    WIFI_LOGI("Instance %{public}d %{public}s", m_id, __func__);
    InternalMessage *msg = m_ApStateMachine.CreateMessage();
    if (msg == nullptr) {
        return ErrCode::WIFI_OPT_FAILED;
    }
    msg->SetMessageName(static_cast<int>(ApStatemachineEvent::CMD_SET_HOTSPOT_CONFIG));
    msg->AddStringMessageBody(cfg.GetSsid());
    msg->AddStringMessageBody(cfg.GetPreSharedKey());
    msg->AddIntMessageBody(static_cast<int>(cfg.GetSecurityType()));
    msg->AddIntMessageBody(static_cast<int>(cfg.GetBand()));
    msg->AddIntMessageBody(cfg.GetChannel());
    msg->AddIntMessageBody(cfg.GetMaxConn());
    m_ApStateMachine.SendMessage(msg);
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode ApService::SetHotspotIdleTimeout(int time) const
{
    WIFI_LOGI("SetHotspotIdleTimeout");
    InternalMessage *msg = m_ApStateMachine.CreateMessage();
    if (msg == nullptr) {
        return ErrCode::WIFI_OPT_FAILED;
    }
    msg->SetMessageName(static_cast<int>(ApStatemachineEvent::CMD_SET_IDLE_TIMEOUT));
    msg->AddIntMessageBody(time);
    m_ApStateMachine.SendMessage(msg);
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode ApService::AddBlockList(const StationInfo &stationInfo) const
{
    WIFI_LOGI("Instance %{public}d %{public}s", m_id, __func__);
    InternalMessage *msg = m_ApStateMachine.CreateMessage();
    if (msg == nullptr) {
        return ErrCode::WIFI_OPT_FAILED;
    }
    msg->SetMessageName(static_cast<int>(ApStatemachineEvent::CMD_ADD_BLOCK_LIST));
    msg->AddStringMessageBody(stationInfo.deviceName);
    msg->AddStringMessageBody(stationInfo.bssid);
    msg->AddStringMessageBody(stationInfo.ipAddr);
    m_ApStateMachine.SendMessage(msg);
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode ApService::DelBlockList(const StationInfo &stationInfo) const
{
    WIFI_LOGI("Instance %{public}d %{public}s", m_id, __func__);
    InternalMessage *msg = m_ApStateMachine.CreateMessage();
    if (msg == nullptr) {
        return ErrCode::WIFI_OPT_FAILED;
    }
    msg->SetMessageName(static_cast<int>(ApStatemachineEvent::CMD_DEL_BLOCK_LIST));
    msg->AddStringMessageBody(stationInfo.deviceName);
    msg->AddStringMessageBody(stationInfo.bssid);
    msg->AddStringMessageBody(stationInfo.ipAddr);
    m_ApStateMachine.SendMessage(msg);
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode ApService::DisconnetStation(const StationInfo &stationInfo) const
{
    WIFI_LOGI("Instance %{public}d %{public}s", m_id, __func__);
    InternalMessage *msg = m_ApStateMachine.CreateMessage();
    if (msg == nullptr) {
        return ErrCode::WIFI_OPT_FAILED;
    }
    msg->SetMessageName(static_cast<int>(ApStatemachineEvent::CMD_DISCONNECT_STATION));
    msg->AddStringMessageBody(stationInfo.deviceName);
    msg->AddStringMessageBody(stationInfo.bssid);
    msg->AddStringMessageBody(stationInfo.ipAddr);
    m_ApStateMachine.SendMessage(msg);
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode ApService::GetStationList(std::vector<StationInfo> &result) const
{
    WIFI_LOGI("Instance %{public}d %{public}s", m_id, __func__);
    WifiSettings::GetInstance().GetStationList(result);
    if (result.empty()) {
        WIFI_LOGW("GetStationList is empty.");
        return ErrCode::WIFI_OPT_SUCCESS;
    }
    // get dhcp lease info, return full connected station info
    std::map<std::string, StationInfo> tmp;
    if (!m_ApStateMachine.GetConnectedStationInfo(tmp)) {
        WIFI_LOGW("Get connected station info failed!");
        return ErrCode::WIFI_OPT_FAILED;
    }
    for (auto iter = result.begin(); iter != result.end(); ++iter) {
        auto itMap = tmp.find(iter->bssid);
        if (itMap == tmp.end()) {
            continue;
        }
        iter->deviceName = itMap->second.deviceName;
        iter->ipAddr = itMap->second.ipAddr;
    }
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode ApService::GetValidBands(std::vector<BandType> &bands)
{
    WIFI_LOGI("Instance %{public}d %{public}s", m_id, __func__);
    std::vector<int> allowed5GFreq;
    std::vector<int> allowed2GFreq;
    if (WifiSettings::GetInstance().GetValidBands(bands) != 0) {
        WIFI_LOGE("%{public}s, GetValidBands failed!", __func__);
        return ErrCode::WIFI_OPT_FAILED;
    }
    if (bands.size() > 0) {
        return ErrCode::WIFI_OPT_SUCCESS;
    }

    /* Get freqs from hal */
    if (WifiApHalInterface::GetInstance().GetFrequenciesByBand(static_cast<int>(BandType::BAND_2GHZ), allowed2GFreq)) {
        WIFI_LOGW("%{public}s, fail to get 2.4G channel", __func__);
    }
    if (WifiApHalInterface::GetInstance().GetFrequenciesByBand(static_cast<int>(BandType::BAND_5GHZ), allowed5GFreq)) {
        WIFI_LOGW("%{public}s, fail to get 5G channel", __func__);
    }
    if (allowed2GFreq.size() > 0) {
        bands.push_back(BandType::BAND_2GHZ);
    }
    if (allowed5GFreq.size() > 0) {
        bands.push_back(BandType::BAND_5GHZ);
    }
    if (bands.size() == 0) {
        WIFI_LOGW("%{public}s, GetValidBands failed!", __func__);
        return ErrCode::WIFI_OPT_FAILED;
    }
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode ApService::GetValidChannels(BandType band, std::vector<int32_t> &validChannel)
{
    WIFI_LOGI("Instance %{public}d %{public}s", m_id, __func__);
    ChannelsTable channelsInfo;
    if (WifiSettings::GetInstance().GetValidChannels(channelsInfo) != 0) {
        WIFI_LOGE("Failed to obtain channels from the WifiSettings.");
        return ErrCode::WIFI_OPT_FAILED;
    }

    if (channelsInfo.size() > 0) {
        auto it = channelsInfo.find(band);
        if (it != channelsInfo.end()) {
            validChannel = channelsInfo[band];
            WIFI_LOGI("%{public}s, get valid channel size:%{public}d, band:%{public}d from WifiSettings.",
                __func__, (int)validChannel.size(), band);
            return ErrCode::WIFI_OPT_SUCCESS;
        }
    }

    /* get freqs from hal service */
    std::vector<int32_t> allowed5GFreq, allowed2GFreq;
    std::vector<int32_t> allowed5GChan, allowed2GChan;
    if (WifiApHalInterface::GetInstance().GetFrequenciesByBand(static_cast<int>(BandType::BAND_2GHZ), allowed2GFreq)) {
        WIFI_LOGW("%{public}s, fail to get 2.4G channel", __func__);
    }
    if (WifiApHalInterface::GetInstance().GetFrequenciesByBand(static_cast<int>(BandType::BAND_5GHZ), allowed5GFreq)) {
        WIFI_LOGW("%{public}s, fail to get 5G channel", __func__);
    }
    if (allowed2GFreq.size() == 0) {
        WifiSettings::GetInstance().SetDefaultFrequenciesByCountryBand(BandType::BAND_2GHZ, allowed2GFreq);
    }
    TransformFrequencyIntoChannel(allowed2GFreq, allowed2GChan);
    TransformFrequencyIntoChannel(allowed5GFreq, allowed5GChan);

    ChannelsTable ChanTbs;
    ChanTbs[BandType::BAND_2GHZ] = allowed2GChan;
    ChanTbs[BandType::BAND_5GHZ] = allowed5GChan;
    if (WifiSettings::GetInstance().SetValidChannels(ChanTbs)) {
        WIFI_LOGE("%{public}s, fail to SetValidChannels", __func__);
    }
    if (band == BandType::BAND_2GHZ || band == BandType::BAND_5GHZ) {
        validChannel = ChanTbs[band];
    } else {
        WIFI_LOGE("%{public}s, invalid band: %{public}d", __func__, band);
        return ErrCode::WIFI_OPT_INVALID_PARAM;
    }
    WIFI_LOGI("%{public}s, get valid channel size:%{public}d, band:%{public}d from hal service.",
        __func__, (int)validChannel.size(), band);
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode ApService::RegisterApServiceCallbacks(const IApServiceCallbacks &callbacks)
{
    WIFI_LOGI("%{public}s, Instance %{public}d ", __func__, m_id);
    m_ApStateMachine.RegisterApServiceCallbacks(callbacks);
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode ApService::GetSupportedPowerModel(std::set<PowerModel>& setPowerModelList)
{
    WIFI_LOGI("Instance %{public}d %{public}s", m_id, __func__);
    /* All modes are currently supported */
    setPowerModelList.insert(PowerModel::SLEEPING);
    setPowerModelList.insert(PowerModel::GENERAL);
    setPowerModelList.insert(PowerModel::THROUGH_WALL);
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode ApService::GetPowerModel(PowerModel& model)
{
    WIFI_LOGI("Instance %{public}d %{public}s", m_id, __func__);
    WifiSettings::GetInstance().GetPowerModel(model, m_id);
    LOGI("ApService::GetPowerModel, model=[%{public}d]", static_cast<int>(model));
    return ErrCode::WIFI_OPT_SUCCESS;
}

ErrCode ApService::SetPowerModel(const PowerModel& model)
{
    WIFI_LOGI("Instance %{public}d %{public}s", m_id, __func__);
    LOGI("Enter ApService::SetPowerModel, model=[%d]", static_cast<int>(model));
    if (WifiApHalInterface::GetInstance().SetPowerModel(static_cast<int>(model), m_id) != WIFI_IDL_OPT_OK) {
        LOGE("SetPowerModel() failed!");
        return WIFI_OPT_FAILED;
    }
    LOGI("SetPowerModel() succeed!");
    WifiSettings::GetInstance().SetPowerModel(model);
    return ErrCode::WIFI_OPT_SUCCESS;
}
}  // namespace Wifi
}  // namespace OHOS