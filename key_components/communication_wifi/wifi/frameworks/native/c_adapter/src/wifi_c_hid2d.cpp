/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "../../../interfaces/kits/c/wifi_hid2d.h"
#include "../../include/wifi_hid2d.h"
#include "define.h"
#include "wifi_logger.h"
#include "wifi_c_utils.h"
#include "wifi_common_util.h"
#include "../../../interfaces/kits/c/wifi_device.h"

DEFINE_WIFILOG_LABEL("WifiCHid2d");

std::unique_ptr<OHOS::Wifi::Hid2d> wifiHid2dPtr = OHOS::Wifi::Hid2d::GetInstance(WIFI_P2P_ABILITY_ID);

NO_SANITIZE("cfi") WifiErrorCode Hid2dRequestGcIp(const unsigned char gcMac[MAC_LEN],
    unsigned int ipAddr[IPV4_ARRAY_LEN])
{
    CHECK_PTR_RETURN(wifiHid2dPtr, ERROR_WIFI_NOT_AVAILABLE);

    std::string strMac = OHOS::Wifi::MacArrayToStr(gcMac);
    std::string strIpAddr;
    OHOS::Wifi::ErrCode ret = wifiHid2dPtr->Hid2dRequestGcIp(strMac, strIpAddr);
    if (ret != OHOS::Wifi::WIFI_OPT_SUCCESS) {
        WIFI_LOGE("Request gc ip failed!");
        return ERROR_WIFI_UNKNOWN;
    }
    return OHOS::Wifi::IpStrToArray(strIpAddr, ipAddr);
}

NO_SANITIZE("cfi") WifiErrorCode Hid2dSharedlinkIncrease(void)
{
    CHECK_PTR_RETURN(wifiHid2dPtr, ERROR_WIFI_NOT_AVAILABLE);
    return GetCErrorCode(wifiHid2dPtr->Hid2dSharedlinkIncrease());
}

NO_SANITIZE("cfi") WifiErrorCode Hid2dSharedlinkDecrease(void)
{
    CHECK_PTR_RETURN(wifiHid2dPtr, ERROR_WIFI_NOT_AVAILABLE);
    return GetCErrorCode(wifiHid2dPtr->Hid2dSharedlinkDecrease());
}

NO_SANITIZE("cfi") WifiErrorCode Hid2dCreateGroup(const int frequency, FreqType type)
{
    CHECK_PTR_RETURN(wifiHid2dPtr, ERROR_WIFI_NOT_AVAILABLE);
    return GetCErrorCode(wifiHid2dPtr->Hid2dCreateGroup(frequency, OHOS::Wifi::FreqType(static_cast<int>(type))));
}

NO_SANITIZE("cfi") WifiErrorCode Hid2dRemoveGcGroup(const char gcIfName[IF_NAME_LEN])
{
    CHECK_PTR_RETURN(wifiHid2dPtr, ERROR_WIFI_NOT_AVAILABLE);
    return GetCErrorCode(wifiHid2dPtr->Hid2dRemoveGcGroup(gcIfName));
}

NO_SANITIZE("cfi") WifiErrorCode Hid2dConnect(const Hid2dConnectConfig *config)
{
    CHECK_PTR_RETURN(wifiHid2dPtr, ERROR_WIFI_NOT_AVAILABLE);
    CHECK_PTR_RETURN(config, ERROR_WIFI_INVALID_ARGS);

    OHOS::Wifi::Hid2dConnectConfig cppConfig;
    cppConfig.SetSsid(config->ssid);
    cppConfig.SetBssid(OHOS::Wifi::MacArrayToStr(config->bssid));
    cppConfig.SetPreSharedKey(config->preSharedKey);
    cppConfig.SetFrequency(config->frequency);
    cppConfig.SetDhcpMode(OHOS::Wifi::DhcpMode(static_cast<int>(config->dhcpMode)));
    return GetCErrorCode(wifiHid2dPtr->Hid2dConnect(cppConfig));
}

NO_SANITIZE("cfi") WifiErrorCode Hid2dConfigIPAddr(const char ifName[IF_NAME_LEN], const IpAddrInfo* ipInfo)
{
    CHECK_PTR_RETURN(wifiHid2dPtr, ERROR_WIFI_NOT_AVAILABLE);
    CHECK_PTR_RETURN(ipInfo, ERROR_WIFI_INVALID_ARGS);

    OHOS::Wifi::IpAddrInfo ipAddrInfo;
    ipAddrInfo.ip = OHOS::Wifi::IpArrayToStr(ipInfo->ip);
    ipAddrInfo.gateway = OHOS::Wifi::IpArrayToStr(ipInfo->gateway);
    ipAddrInfo.netmask = OHOS::Wifi::IpArrayToStr(ipInfo->netmask);
    return GetCErrorCode(wifiHid2dPtr->Hid2dConfigIPAddr(ifName, ipAddrInfo));
}

NO_SANITIZE("cfi") WifiErrorCode Hid2dReleaseIPAddr(const char ifName[IF_NAME_LEN])
{
    CHECK_PTR_RETURN(wifiHid2dPtr, ERROR_WIFI_NOT_AVAILABLE);
    return GetCErrorCode(wifiHid2dPtr->Hid2dReleaseIPAddr(ifName));
}

static void ConvertRecommendChannelRequest(const RecommendChannelRequest *request,
    OHOS::Wifi::RecommendChannelRequest& req)
{
    CHECK_PTR_RETURN_VOID(request);
    req.remoteIfName = request->remoteIfName;
    req.remoteIfMode = request->remoteIfMode;
    req.localIfName = request->localIfName;
    req.localIfMode = request->localIfMode;
    req.prefBand = request->prefBand;
    req.prefBandwidth = OHOS::Wifi::PreferBandwidth(static_cast<int>(request->prefBandwidth));
}

static void ConvertRecommendChannelResponse(const OHOS::Wifi::RecommendChannelResponse& rsp,
    RecommendChannelResponse* response)
{
    CHECK_PTR_RETURN_VOID(response);
    response->status = RecommendStatus(static_cast<int>(rsp.status));
    response->index = rsp.index;
    response->centerFreq = rsp.centerFreq;
    response->centerFreq1 = rsp.centerFreq1;
    response->centerFreq2 = rsp.centerFreq2;
    response->bandwidth = rsp.bandwidth;
}

NO_SANITIZE("cfi") WifiErrorCode Hid2dGetRecommendChannel(const RecommendChannelRequest *request,
    RecommendChannelResponse *response)
{
    CHECK_PTR_RETURN(wifiHid2dPtr, ERROR_WIFI_NOT_AVAILABLE);
    CHECK_PTR_RETURN(request, ERROR_WIFI_INVALID_ARGS);
    CHECK_PTR_RETURN(response, ERROR_WIFI_INVALID_ARGS);
    OHOS::Wifi::RecommendChannelRequest req;
    OHOS::Wifi::RecommendChannelResponse rsp;
    ConvertRecommendChannelRequest(request, req);
    OHOS::Wifi::ErrCode retCode = wifiHid2dPtr->Hid2dGetRecommendChannel(req, rsp);
    if (retCode != OHOS::Wifi::WIFI_OPT_SUCCESS) {
        return GetCErrorCode(retCode);
    }
    ConvertRecommendChannelResponse(rsp, response);
    return WIFI_SUCCESS;
}

NO_SANITIZE("cfi") WifiErrorCode Hid2dGetChannelListFor5G(int *chanList, int len)
{
    CHECK_PTR_RETURN(wifiHid2dPtr, ERROR_WIFI_NOT_AVAILABLE);
    CHECK_PTR_RETURN(chanList, ERROR_WIFI_INVALID_ARGS);

    std::vector<int> vecChannelList;
    OHOS::Wifi::ErrCode ret = wifiHid2dPtr->Hid2dGetChannelListFor5G(vecChannelList);
    if (ret == OHOS::Wifi::WIFI_OPT_SUCCESS) {
        size_t idx = 0;
        for (; idx != vecChannelList.size() && (int)(idx + 1) < len; ++idx) {
            *chanList = vecChannelList[idx];
            ++chanList;
        }
        if (idx != vecChannelList.size()) {
            WIFI_LOGW("Get channel list for 5G, `chanList` is too small!");
        }
        *chanList = 0;
    }
    return GetCErrorCode(ret);
}

NO_SANITIZE("cfi") WifiErrorCode Hid2dGetSelfWifiCfgInfo(SelfCfgType cfgType,
    char cfgData[CFG_DATA_MAX_BYTES], int* getDatValidLen)
{
    CHECK_PTR_RETURN(wifiHid2dPtr, ERROR_WIFI_NOT_AVAILABLE);
    CHECK_PTR_RETURN(getDatValidLen, ERROR_WIFI_INVALID_ARGS);
    return GetCErrorCode(wifiHid2dPtr->Hid2dGetSelfWifiCfgInfo(
        OHOS::Wifi::SelfCfgType(static_cast<int>(cfgType)), cfgData, getDatValidLen));
}

NO_SANITIZE("cfi") WifiErrorCode Hid2dSetPeerWifiCfgInfo(PeerCfgType cfgType,
    char cfgData[CFG_DATA_MAX_BYTES], int setDataValidLen)
{
    CHECK_PTR_RETURN(wifiHid2dPtr, ERROR_WIFI_NOT_AVAILABLE);
    return GetCErrorCode(wifiHid2dPtr->Hid2dSetPeerWifiCfgInfo(
        OHOS::Wifi::PeerCfgType(static_cast<int>(cfgType)), cfgData, setDataValidLen));
}

int Hid2dIsWideBandwidthSupported(void)
{
    constexpr int NOT_SUPPORT = 0; // false
    return NOT_SUPPORT;
}

NO_SANITIZE("cfi") WifiErrorCode Hid2dSetUpperScene(const char ifName[IF_NAME_LEN], const Hid2dUpperScene *scene)
{
    CHECK_PTR_RETURN(wifiHid2dPtr, ERROR_WIFI_NOT_AVAILABLE);
    CHECK_PTR_RETURN(scene, ERROR_WIFI_INVALID_ARGS);
    OHOS::Wifi::Hid2dUpperScene upperScene;
    upperScene.mac = OHOS::Wifi::MacArrayToStr(scene->mac);
    upperScene.scene = scene->scene;
    upperScene.fps = scene->fps;
    upperScene.bw = scene->bw;
    return GetCErrorCode(wifiHid2dPtr->Hid2dSetUpperScene(ifName, upperScene));
}
