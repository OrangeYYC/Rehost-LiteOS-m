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

#include "wifi_scan_stub.h"
#include "wifi_logger.h"
#include "wifi_msg.h"
#include "define.h"
#include "wifi_scan_callback_proxy.h"
#include "wifi_internal_event_dispatcher.h"
#include "wifi_scan_death_recipient.h"

DEFINE_WIFILOG_SCAN_LABEL("WifiScanStub");

namespace OHOS {
namespace Wifi {
WifiScanStub::WifiScanStub() : callback_(nullptr), mSingleCallback(false)
{
    InitHandleMap();
}

WifiScanStub::~WifiScanStub()
{}

void WifiScanStub::InitHandleMap()
{
    handleFuncMap[WIFI_SVR_CMD_SET_SCAN_CONTROL_INFO] = &WifiScanStub::OnSetScanControlInfo;
    handleFuncMap[WIFI_SVR_CMD_FULL_SCAN] = &WifiScanStub::OnScan;
    handleFuncMap[WIFI_SVR_CMD_SPECIFIED_PARAMS_SCAN] = &WifiScanStub::OnScanByParams;
    handleFuncMap[WIFI_SVR_CMD_IS_SCAN_ALWAYS_ACTIVE] = &WifiScanStub::OnIsWifiClosedScan;
    handleFuncMap[WIFI_SVR_CMD_GET_SCAN_INFO_LIST] = &WifiScanStub::OnGetScanInfoList;
    handleFuncMap[WIFI_SVR_CMD_REGISTER_SCAN_CALLBACK] = &WifiScanStub::OnRegisterCallBack;
    handleFuncMap[WIFI_SVR_CMD_GET_SUPPORTED_FEATURES] = &WifiScanStub::OnGetSupportedFeatures;
}

int WifiScanStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("WifiScanStub::OnRemoteRequest,code:%{public}u", code);

    if (data.ReadInterfaceToken() != GetDescriptor()) {
        WIFI_LOGE("Scan stub token verification error: %{public}d", code);
        return WIFI_OPT_FAILED;
    }

    HandleFuncMap::iterator iter = handleFuncMap.find(code);
    if (iter == handleFuncMap.end()) {
        WIFI_LOGI("not find function to deal, code %{public}u", code);
        reply.WriteInt32(0);
        reply.WriteInt32(WIFI_OPT_NOT_SUPPORTED);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    } else {
        int exception = data.ReadInt32();
        if (exception) {
            return WIFI_OPT_FAILED;
        }
        return (this->*(iter->second))(code, data, reply, option);
    }
}

sptr<IWifiScanCallback> WifiScanStub::GetCallback() const
{
    return callback_;
}

int WifiScanStub::OnSetScanControlInfo(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run OnSetScanControlInfo code %{public}u, datasize %{public}zu", code, data.GetRawDataSize());
    constexpr int MAX_SIZE = 1024;
    ScanControlInfo info;
    int forbidListSize = data.ReadInt32();
    if (forbidListSize > MAX_SIZE) {
        reply.WriteInt32(0);
        reply.WriteInt32(WIFI_OPT_INVALID_PARAM);
        return WIFI_OPT_INVALID_PARAM;
    }
    for (int i = 0; i < forbidListSize; i++) {
        ScanForbidMode scanForbidMode;
        scanForbidMode.scanScene = data.ReadInt32();
        scanForbidMode.scanMode = static_cast<ScanMode>(data.ReadInt32());
        scanForbidMode.forbidTime = data.ReadInt32();
        scanForbidMode.forbidCount = data.ReadInt32();
        info.scanForbidList.push_back(scanForbidMode);
    }

    int intervalSize = data.ReadInt32();
    if (intervalSize > MAX_SIZE) {
        reply.WriteInt32(0);
        reply.WriteInt32(WIFI_OPT_INVALID_PARAM);
        return WIFI_OPT_INVALID_PARAM;
    }
    for (int i = 0; i < intervalSize; i++) {
        ScanIntervalMode scanIntervalMode;
        scanIntervalMode.scanScene = data.ReadInt32();
        scanIntervalMode.scanMode = static_cast<ScanMode>(data.ReadInt32());
        scanIntervalMode.isSingle = data.ReadBool();
        scanIntervalMode.intervalMode = static_cast<IntervalMode>(data.ReadInt32());
        scanIntervalMode.interval = data.ReadInt32();
        scanIntervalMode.count = data.ReadInt32();
        info.scanIntervalList.push_back(scanIntervalMode);
    }

    ErrCode ret = SetScanControlInfo(info);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);

    return ret;
}

int WifiScanStub::OnScan(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run OnScan code %{public}u, datasize %{public}zu", code, data.GetRawDataSize());
    ErrCode ret = Scan();
    reply.WriteInt32(0);
    reply.WriteInt32(ret);

    return ret;
}

int WifiScanStub::OnScanByParams(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run OnScanByParams code %{public}u, datasize %{public}zu", code, data.GetRawDataSize());
    const char *readStr = nullptr;
    constexpr int MAX_FREQS_SIZE = 512;
    WifiScanParams params;
    readStr = data.ReadCString();
    params.ssid = (readStr != nullptr) ? readStr : "";
    readStr = data.ReadCString();
    params.bssid = (readStr != nullptr) ? readStr : "";
    int size = data.ReadInt32();
    if (size > MAX_FREQS_SIZE) {
        reply.WriteInt32(0);
        reply.WriteInt32(WIFI_OPT_INVALID_PARAM);
        return WIFI_OPT_INVALID_PARAM;
    }
    for (int i = 0; i < size; i++) {
        int tmp = data.ReadInt32();
        params.freqs.push_back(tmp);
    }
    params.band = data.ReadInt32();

    ErrCode ret = AdvanceScan(params);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);

    return ret;
}

int WifiScanStub::OnIsWifiClosedScan(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run OnIsWifiClosedScan code %{public}u, datasize %{public}zu", code, data.GetRawDataSize());
    bool bOpen = false;
    ErrCode ret = IsWifiClosedScan(bOpen);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    if (ret == WIFI_OPT_SUCCESS) {
        reply.WriteBool(bOpen);
    }
    return ret;
}

int WifiScanStub::OnGetScanInfoList(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run OnGetScanInfoList code %{public}u, datasize %{public}zu", code, data.GetRawDataSize());
    std::vector<WifiScanInfo> result;
    ErrCode ret = GetScanInfoList(result);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    if (ret != WIFI_OPT_SUCCESS) {
        return ret;
    }

    unsigned int size = result.size();
    reply.WriteInt32(size);
    for (unsigned int i = 0; i < size; ++i) {
        reply.WriteString(result[i].bssid);
        reply.WriteString(result[i].ssid);
        reply.WriteString(result[i].capabilities);
        reply.WriteInt32(result[i].frequency);
        reply.WriteInt32(result[i].rssi);
        reply.WriteInt64(result[i].timestamp);
        reply.WriteInt32(result[i].band);
        reply.WriteInt32(static_cast<int>(result[i].securityType));
        reply.WriteInt32(static_cast<int>(result[i].channelWidth));
        reply.WriteInt32(result[i].centerFrequency0);
        reply.WriteInt32(result[i].centerFrequency1);
        reply.WriteInt64(result[i].features);
        reply.WriteInt32(result[i].infoElems.size());
        for (unsigned int m = 0; m < result[i].infoElems.size(); ++m) {
            reply.WriteInt32(result[i].infoElems[m].id);
            reply.WriteInt32(result[i].infoElems[m].content.size());
            for (unsigned int n = 0; n < result[i].infoElems[m].content.size(); ++n) {
                reply.WriteInt8(result[i].infoElems[m].content[n]);
            }
        }
    }
    return ret;
}

int WifiScanStub::OnRegisterCallBack(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    ErrCode ret = WIFI_OPT_FAILED;
    do {
        sptr<IRemoteObject> remote = data.ReadRemoteObject();
        if (remote == nullptr) {
            WIFI_LOGE("Failed to readRemoteObject!");
            break;
        }

        callback_ = iface_cast<IWifiScanCallback>(remote);
        if (callback_ == nullptr) {
            callback_ = new (std::nothrow) WifiScanCallbackProxy(remote);
            WIFI_LOGI("create new `WifiScanCallbackProxy`!");
        }

        int pid = data.ReadInt32();
        WIFI_LOGD("%{public}s, get pid: %{public}d", __func__, pid);
        if (mSingleCallback) {
            ret = RegisterCallBack(callback_);
        } else {
            if (deathRecipient_ == nullptr) {
                deathRecipient_ = new (std::nothrow) WifiScanDeathRecipient();
            }
            if ((remote->IsProxyObject()) && (!remote->AddDeathRecipient(deathRecipient_))) {
                WIFI_LOGD("AddDeathRecipient!");
            }
            if (callback_ != nullptr) {
                WifiInternalEventDispatcher::GetInstance().AddScanCallback(remote, callback_, pid);
            }
            ret = WIFI_OPT_SUCCESS;
        }
    } while (0);

    reply.WriteInt32(0);
    reply.WriteInt32(ret);
    return 0;
}

int WifiScanStub::OnGetSupportedFeatures(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    WIFI_LOGD("run %{public}s code %{public}u, datasize %{public}zu", __func__, code, data.GetRawDataSize());
    long features = 0;
    int ret = GetSupportedFeatures(features);
    reply.WriteInt32(0);
    reply.WriteInt32(ret);

    if (ret == WIFI_OPT_SUCCESS) {
        reply.WriteInt64(features);
    }

    return ret;
}

bool WifiScanStub::IsSingleCallback() const
{
    return mSingleCallback;
}

void WifiScanStub::SetSingleCallback(const bool isSingleCallback)
{
    mSingleCallback = true;
}
}  // namespace Wifi
}  // namespace OHOS