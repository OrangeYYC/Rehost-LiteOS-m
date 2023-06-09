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

#include "wifi_hostapd_hal.h"
#include <malloc.h>
#include <poll.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "securec.h"
#include "common/wpa_ctrl.h"
#include "wifi_common_def.h"
#include "wifi_hal_callback.h"
#include "wifi_log.h"

#undef LOG_TAG
#define LOG_TAG "WifiHostapdHal"

/**
 * Blocklist configuration file name. This parameter is used by hostapd in an earlier version.
 */
#define CONFIG_DENY_MAC_FILE_NAME "deny_mac.conf"
#define SLEEP_TIME_100_MS (100 * 1000)
#define CONFIG_PATH_DIR CONFIG_ROOR_DIR"/wpa_supplicant"
#if (AP_NUM > 1)
#define WIFI_5G_CFG "hostapd_0.conf"
#define WIFI_2G_CFG "hostapd_1.conf"
#define HOSTAPD_5G_CFG CONFIG_ROOR_DIR"/wpa_supplicant/"WIFI_5G_CFG
#define HOSTAPD_2G_CFG CONFIG_ROOR_DIR"/wpa_supplicant/"WIFI_2G_CFG
#define HOSTAPD_5G_UDPPORT "127.0.0.1:9866"
#define HOSTAPD_2G_UDPPORT "127.0.0.1:9877"

WifiHostapdHalDeviceInfo g_hostapdHalDevInfo[] = {
    {AP_5G_MAIN_INSTANCE, NULL, WIFI_5G_CFG, HOSTAPD_5G_CFG, HOSTAPD_5G_UDPPORT},
    {AP_2G_MAIN_INSTANCE, NULL, WIFI_2G_CFG, HOSTAPD_2G_CFG, HOSTAPD_2G_UDPPORT},
};
#else
#define WIFI_DEFAULT_CFG "hostapd.conf"
#define HOSTAPD_DEFAULT_CFG CONFIG_ROOR_DIR"/wpa_supplicant/"WIFI_DEFAULT_CFG
#define HOSTAPD_DEFAULT_UDPPORT "127.0.0.1:9877"
WifiHostapdHalDeviceInfo g_hostapdHalDevInfo[] = {
    {AP_2G_MAIN_INSTANCE, NULL, WIFI_DEFAULT_CFG, HOSTAPD_DEFAULT_CFG, HOSTAPD_DEFAULT_UDPPORT}
};
#endif

WifiHostapdHalDeviceInfo *GetWifiCfg(int *len)
{
    *len = sizeof(g_hostapdHalDevInfo) / sizeof(WifiHostapdHalDeviceInfo);
    return g_hostapdHalDevInfo;
}
/*
 * The wpa_ctrl_pending interface provided by the WPA does not wait for the response.
 * As a result, the CPU usage is high. Reconstructed
 */
static int MyWpaCtrlPending(struct wpa_ctrl *ctrl)
{
    if (ctrl == NULL) {
        return -1;
    }
    struct pollfd pfd;
    if (memset_s(&pfd, sizeof(pfd), 0, sizeof(pfd)) != EOK) {
        return -1;
    }
    pfd.fd = wpa_ctrl_get_fd(ctrl);
    pfd.events = POLLIN;
    int ret = poll(&pfd, 1, 100); /* 100 ms */
    if (ret < 0) {
        LOGE("poll failed!");
        return -1;
    } else if (ret == 0) {
        return 0;
    } else {
        return 1;
    }
}

static void DelCallbackMessage(const char *msg, int id)
{
    if (msg == NULL) {
        return;
    }
    if (strncmp(msg, AP_STA_CONNECTED, strlen(AP_STA_CONNECTED)) == 0 ||
        strncmp(msg, AP_STA_DISCONNECTED, strlen(AP_STA_DISCONNECTED)) == 0) {
        WifiHalCbStaJoin(msg, id);
    } else if (strncmp(msg, AP_EVENT_ENABLED, strlen(AP_EVENT_ENABLED)) == 0 ||
               strncmp(msg, AP_EVENT_DISABLED, strlen(AP_EVENT_DISABLED)) == 0 ||
               strncmp(msg, WPA_EVENT_TERMINATING, strlen(WPA_EVENT_TERMINATING)) == 0) {
        if (strncmp(msg, AP_EVENT_DISABLED, strlen(AP_EVENT_DISABLED)) == 0 &&
            g_hostapdHalDevInfo[id].hostapdHalDev->execDisable == 1) {
            g_hostapdHalDevInfo[id].hostapdHalDev->execDisable = 0;
            return;
        }
        WifiHalCbApState(msg, id);
        if (strncmp(msg, WPA_EVENT_TERMINATING, strlen(WPA_EVENT_TERMINATING)) == 0) {
            g_hostapdHalDevInfo[id].hostapdHalDev->threadRunFlag = 0;
        }
    }
}

static void *HostapdReceiveCallback(void *arg)
{
    if (arg == NULL) {
        LOGE("%{public}s arg is null", __func__);
        return NULL;
    }
    WifiHostapdHalDeviceInfo *halDeviceInfo = arg;
    int id = halDeviceInfo->id;
    if (halDeviceInfo->hostapdHalDev == NULL ||
        halDeviceInfo->hostapdHalDev->ctrlRecv == NULL) {
        LOGE("%{public}s invalid HalDev or ctrlRecv", __func__);
        return NULL;
    }
    struct wpa_ctrl *ctrl = halDeviceInfo->hostapdHalDev->ctrlRecv;
    char *buf = (char *)calloc(BUFSIZE_RECV, sizeof(char));
    if (buf == NULL) {
        LOGE("In hostapd deal receive message thread, failed to calloc buff!");
        return NULL;
    }
    while (g_hostapdHalDevInfo[id].hostapdHalDev->threadRunFlag) {
        int ret = MyWpaCtrlPending(ctrl);
        if (ret < 0) {
            LOGE("hostapd thread get event message failed!");
            break;
        } else if (ret == 0) {
            continue;
        }
        if (memset_s(buf, BUFSIZE_RECV, 0, BUFSIZE_RECV) != EOK) {
            LOGE("thread clear buffer cache failed!");
            break;
        }
        size_t len = BUFSIZE_RECV - 1;
        ret = wpa_ctrl_recv(ctrl, buf, &len);
        if (ret < 0) {
            LOGE("hostapd thread read event message failed!");
            break;
        }
        if (len <= 0) {
            continue;
        }
        char *p = buf;
        if (*p == '<') {
            p = strchr(p, '>');
            (p == NULL) ? (p = buf) : (p++);
        }
        DelCallbackMessage(p, id);
    }
    free(buf);
    buf = NULL;
    LOGI("=====================hostapd thread exist=======================");
    return NULL;
}

void ReleaseHostapdCtrl(int id)
{
    if (g_hostapdHalDevInfo[id].hostapdHalDev == NULL) {
        return;
    }
    if (g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn != NULL) {
        wpa_ctrl_close(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn);
        g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn = NULL;
    }
    if (g_hostapdHalDevInfo[id].hostapdHalDev->ctrlRecv != NULL) {
        wpa_ctrl_close(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlRecv);
        g_hostapdHalDevInfo[id].hostapdHalDev->ctrlRecv = NULL;
    }
    return;
}

int InitHostapdCtrl(const char *ifname, int id)
{
    if (g_hostapdHalDevInfo[id].hostapdHalDev == NULL || ifname == NULL) {
        return -1;
    }
    int flag = 0;
    do {
        g_hostapdHalDevInfo[id].hostapdHalDev->ctrlRecv = wpa_ctrl_open(ifname);
        if (g_hostapdHalDevInfo[id].hostapdHalDev->ctrlRecv == NULL) {
            LOGE("open hostapd control interface ctrlRecv failed!");
            break;
        }
        if (wpa_ctrl_attach(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlRecv) != 0) {
            LOGE("attach hostapd monitor interface failed!");
            break;
        }
        g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn = wpa_ctrl_open(ifname);
        if (g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn == NULL) {
            LOGE("open hostapd control interface ctrlConn failed!");
            break;
        }
        flag += 1;
    } while (0);
    if (!flag) {
        ReleaseHostapdCtrl(id);
        return -1;
    }
    return 0;
}

void GetDestPort(char *destPort, size_t len, int id)
{
    strcpy_s(destPort, len, g_hostapdHalDevInfo[id].udpPort);
}

static int HostapdCliConnect(int id)
{
    if (g_hostapdHalDevInfo[id].hostapdHalDev == NULL) {
        LOGE("hostapdHalDev is NULL!");
        return -1;
    }
    if (g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn != NULL) {
        LOGE("Hostapd already initialized!");
        return 0;
    }
    int retryCount = 20;
    char ifname[BUFFER_SIZE_64] = {0};
    GetDestPort(ifname, sizeof(ifname), id);
    while (retryCount-- > 0) {
        int ret = InitHostapdCtrl(ifname, id);
        if (ret == 0) {
            LOGI("Global hostapd interface connect successfully!");
            break;
        } else {
            LOGD("Init hostapd ctrl failed: %{public}d", ret);
        }
        usleep(SLEEP_TIME_100_MS);
    }
    if (retryCount <= 0) {
        LOGD("Retry init hostapd ctrl failed, retryCount: %{public}d", retryCount);
        return -1;
    }
    g_hostapdHalDevInfo[id].hostapdHalDev->threadRunFlag = 1;
    if (pthread_create(&g_hostapdHalDevInfo[id].hostapdHalDev->tid, NULL,
        HostapdReceiveCallback, &g_hostapdHalDevInfo[id]) != 0) {
        g_hostapdHalDevInfo[id].hostapdHalDev->threadRunFlag = 0;
        ReleaseHostapdCtrl(id);
        LOGE("Create hostapd monitor thread failed!");
        return -1;
    }
    return 0;
}

static int HostapdCliClose(int id)
{
    if (g_hostapdHalDevInfo[id].hostapdHalDev == NULL) {
        return 0;
    }
    if (g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn != NULL) {
        g_hostapdHalDevInfo[id].hostapdHalDev->threadRunFlag = 0;
        pthread_join(g_hostapdHalDevInfo[id].hostapdHalDev->tid, NULL);
        g_hostapdHalDevInfo[id].hostapdHalDev->tid = 0;
        wpa_ctrl_close(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlRecv);
        g_hostapdHalDevInfo[id].hostapdHalDev->ctrlRecv = NULL;
        wpa_ctrl_close(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn);
        g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn = NULL;
    }
    return 0;
}

static int WpaCtrlCommand(struct wpa_ctrl *ctrl, const char *cmd, char *buf, size_t bufSize)
{
    if (ctrl == NULL || cmd == NULL || buf == NULL || bufSize <= 0) {
        LOGE("Request parameters not correct");
        return -1;
    }
    size_t len = bufSize - 1;
    int ret = wpa_ctrl_request(ctrl, cmd, strlen(cmd), buf, &len, NULL);
    if (ret == REQUEST_FAILED) {
        LOGE("Command timed out.");
        return ret;
    } else if (ret < 0) {
        LOGE("Command failed.");
        return -1;
    }

    buf[len] = '\0';
    if (memcmp(buf, "FAIL", FAIL_LENGTH) == 0) {
        LOGD("Command result not ok, return %{public}s", buf);
        return -1;
    }
    return 0;
}

static int EnableAp(int id)
{
    char buf[BUFSIZE_REQUEST_SMALL] = {0};
    return WpaCtrlCommand(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn, "ENABLE", buf, sizeof(buf));
}

static int SetApName(const char *name, int id)
{
    if (name == NULL) {
        return -1;
    }
    char cmd[BUFSIZE_CMD] = {0};
    char buf[BUFSIZE_REQUEST_SMALL] = {0};

    if (sprintf_s(cmd, sizeof(cmd), "SET ssid %s", name) < 0) {
        return -1;
    }
    return WpaCtrlCommand(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn, cmd, buf, sizeof(buf));
}

static int SetApRsnPairwise(const char *type, int id)
{
    if (type == NULL) {
        return -1;
    }

    char cmd[BUFSIZE_CMD] = {0};
    char buf[BUFSIZE_REQUEST_SMALL] = {0};

    if (sprintf_s(cmd, sizeof(cmd), "SET rsn_pairwise %s", type) < 0) {
        return -1;
    }
    return WpaCtrlCommand(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn, cmd, buf, sizeof(buf));
}

static int SetApWpaPairwise(const char *type, int id)
{
    if (type == NULL) {
        return -1;
    }
    char cmd[BUFSIZE_CMD] = {0};
    char buf[BUFSIZE_REQUEST_SMALL] = {0};

    if (sprintf_s(cmd, sizeof(cmd), "SET wpa_pairwise %s", type) < 0) {
        return -1;
    }
    return WpaCtrlCommand(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn, cmd, buf, sizeof(buf));
}

static int SetApWpaKeyMgmt(const char *type, int id)
{
    if (type == NULL) {
        return -1;
    }
    char cmd[BUFSIZE_CMD] = {0};
    char buf[BUFSIZE_REQUEST_SMALL] = {0};

    if (sprintf_s(cmd, sizeof(cmd), "SET wpa_key_mgmt %s", type) < 0) {
        return -1;
    }
    return WpaCtrlCommand(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn, cmd, buf, sizeof(buf));
}

static int SetApWpaValue(int securityType, int id)
{
    int retval = -1;
    char cmd[BUFSIZE_CMD] = {0};
    char buf[BUFSIZE_REQUEST_SMALL] = {0};

    switch (securityType) {
        case NONE:
            retval = sprintf_s(cmd, sizeof(cmd), "SET wpa 0"); /* The authentication mode is NONE. */
            break;
        case WPA_PSK:
            retval = sprintf_s(cmd, sizeof(cmd), "SET wpa 1"); /* The authentication mode is WPA-PSK. */
            break;
        case WPA2_PSK:
            retval = sprintf_s(cmd, sizeof(cmd), "SET wpa 2"); /* The authentication mode is WPA2-PSK. */
            break;
        default:
            LOGE("Unknown encryption type!");
            return retval;
    }
    if (retval < 0) {
        return -1;
    }

    retval = WpaCtrlCommand(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn, cmd, buf, sizeof(buf));
    if (retval == 0 && securityType != NONE) {
        /*
         * If the value of wpa is switched between 0, 1, and 2, the wpa_key_mgmt,
         * wpa_pairwise, and rsn_pairwise attributes must be set. Otherwise, the
         * enable or STA cannot be connected.
         */
        retval = SetApWpaKeyMgmt("WPA-PSK", id);
    }
    if (retval == 0 && securityType == WPA_PSK) {
        retval = SetApWpaPairwise("CCMP", id);
    }
    if (retval == 0 && securityType == WPA2_PSK) {
        retval = SetApRsnPairwise("CCMP", id);
    }
    return retval;
}

static int SetApPasswd(const char *pass, int id)
{
    if (pass == NULL) {
        return -1;
    }
    char cmd[BUFSIZE_CMD] = {0};
    char buf[BUFSIZE_REQUEST_SMALL] = {0};

    if (sprintf_s(cmd, sizeof(cmd), "SET wpa_passphrase %s", pass) < 0) {
        return -1;
    }
    return WpaCtrlCommand(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn, cmd, buf, sizeof(buf));
}

static int SetApChannel(int channel, int id)
{
    char cmd[BUFSIZE_CMD] = {0};
    char buf[BUFSIZE_REQUEST_SMALL] = {0};

    if (sprintf_s(cmd, sizeof(cmd), "SET channel %d", channel) < 0) {
        return -1;
    }
    return WpaCtrlCommand(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn, cmd, buf, sizeof(buf));
}

static int SetApBand(int band, int id)
{
    char cmd[BUFSIZE_CMD] = {0};
    char buf[BUFSIZE_REQUEST_SMALL] = {0};
    const char *hwMode = NULL;

    switch (band) {
        case AP_NONE_BAND:
            hwMode = "any"; /* Unknown frequency band */
            break;
        case AP_2GHZ_BAND:
            hwMode = "g"; /* BAND_2_4_GHZ */
            break;
        case AP_5GHZ_BAND:
            hwMode = "a"; /* BAND_5_GHZ */
            break;
        default:
            LOGE("Invalid band!");
            return -1;
    }

    if (sprintf_s(cmd, sizeof(cmd), "SET hw_mode %s", hwMode) < 0) {
        return -1;
    }
    return WpaCtrlCommand(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn, cmd, buf, sizeof(buf));
}

static int SetApMaxConn(int maxConn, int id)
{
    char cmd[BUFSIZE_CMD] = {0};
    char buf[BUFSIZE_REQUEST_SMALL] = {0};

    if (sprintf_s(cmd, sizeof(cmd), "SET max_num_sta %d", maxConn) < 0) {
        return -1;
    }
    return WpaCtrlCommand(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn, cmd, buf, sizeof(buf));
}

static int SetApInfo(HostapdConfig *info, int id)
{
    if (info == NULL) {
        return -1;
    }
    int retval = -1;
    if (info->securityType != NONE) {
        int passwdLen = strlen(info->preSharedKey);
        if (passwdLen < PASSWD_MIN_LEN || passwdLen != info->preSharedKeyLen) {
            LOGE("password is invalid!");
            return retval;
        }
        if ((retval = SetApPasswd((char *)info->preSharedKey, id)) != 0) {
            LOGE("SetApPasswd failed. retval %{public}d", retval);
            return retval;
        }
    }
    if ((retval = SetApName((char *)info->ssid, id)) != 0) {
        LOGE("SetApName failed. retval %{public}d", retval);
        return retval;
    }
    if ((retval = SetApWpaValue(info->securityType, id)) != 0) {
        LOGE("SetApWpaValue failed. retval %{public}d", retval);
        return retval;
    }
    if ((retval = SetApBand(info->band, id)) != 0) {
        LOGE("SetApBand failed. retval %{public}d", retval);
        return retval;
    }
    if ((retval = SetApChannel(info->channel, id)) != 0) {
        LOGE("SetApChannel failed. retval %{public}d", retval);
        return retval;
    }
    if (info->maxConn >= 0 && (retval = SetApMaxConn(info->maxConn, id)) != 0) {
        LOGE("SetApMaxConn failed. retval %{public}d", retval);
        return retval;
    }
    return 0;
}

static int DisableAp(int id)
{
    char buf[BUFSIZE_REQUEST_SMALL] = {0};
    g_hostapdHalDevInfo[id].hostapdHalDev->execDisable = 1;
    return WpaCtrlCommand(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn, "DISABLE", buf, sizeof(buf));
}

static int ModBlockList(const char *mac, int id)
{
    if (mac == NULL) {
        return -1;
    }
    char buf[BUFSIZE_REQUEST_SMALL] = {0};
    char cmd[BUFSIZE_CMD] = {0};
    char file[FILE_NAME_SIZE] = {0};
    if (snprintf_s(file, sizeof(file), sizeof(file) - 1, "%s/%s", CONFIG_PATH_DIR, CONFIG_DENY_MAC_FILE_NAME) < 0) {
        return -1;
    }
    FILE *fp = fopen(file, "w");
    if (fp == NULL) {
        return -1;
    }
    if (fprintf(fp, "%s\n", mac) < 0) {
        fclose(fp);
        return -1;
    }
    fclose(fp);
    if (sprintf_s(cmd, sizeof(cmd), "SET deny_mac_file %s/%s", CONFIG_PATH_DIR, CONFIG_DENY_MAC_FILE_NAME) < 0) {
        return -1;
    }
    return WpaCtrlCommand(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn, cmd, buf, sizeof(buf));
}

static int AddBlocklist(const char *mac, int id)
{
    if (mac == NULL) {
        return -1;
    }
    char cmd[BUFSIZE_CMD] = {0};
    char buf[BUFSIZE_REQUEST_SMALL] = {0};

    if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "DENY_ACL ADD_MAC %s", mac) < 0) {
        return -1;
    }
    if (WpaCtrlCommand(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn, cmd, buf, sizeof(buf)) != 0) {
        LOGE("AddBlocklist Failed");
        return -1;
    }
    if (strncasecmp(buf, "UNKNOWN COMMAND", UNKNOWN_COMMAND_LENGTH) == 0) {
        LOGD("AddBlocklist DENY_ACL command return %{public}s, use SET command", buf);
        /**
         * The hostapd of an earlier version does not support the DENY_ACL command and uses the configuration file.
         */
        return ModBlockList(mac, id);
    }
    return 0;
}

static int DelBlocklist(const char *mac, int id)
{
    if (mac == NULL) {
        return -1;
    }
    char cmd[BUFSIZE_CMD] = {0};
    char buf[BUFSIZE_REQUEST_SMALL] = {0};

    if (sprintf_s(cmd, sizeof(cmd), "DENY_ACL DEL_MAC %s", mac) < 0) {
        return -1;
    }
    if (WpaCtrlCommand(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn, cmd, buf, sizeof(buf)) != 0) {
        LOGE("DelBlocklist Failed");
        return -1;
    }
    if (strncasecmp(buf, "UNKNOWN COMMAND", UNKNOWN_COMMAND_LENGTH) == 0) {
        LOGD("DelBlocklist DENY_ACL command return %{public}s, use SET command", buf);
        if (sprintf_s(cmd, sizeof(cmd), "-%s", mac) < 0) {
            return -1;
        }
        return ModBlockList(cmd, id);
    }
    return 0;
}

static int GetApStatus(StatusInfo *info, int id)
{
    if (info == NULL) {
        return -1;
    }
    char *buf = (char *)calloc(BUFSIZE_RECV, sizeof(char));
    if (buf == NULL) {
        return -1;
    }

    if (WpaCtrlCommand(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn, "STATUS", buf, BUFSIZE_RECV) != 0) {
        LOGE("Status WpaCtrlCommand failed");
        free(buf);
        buf = NULL;
        return -1;
    }

    char *p = strstr(buf, "state=");
    if (p == NULL) {
        LOGD("Status not find state result!");
        free(buf);
        buf = NULL;
        return 0;
    }
    p += strlen("state=");  // skip state=
    unsigned pos = 0;
    while (pos < sizeof(info->state) - 1 && *p != '\0' && *p != '\n') {
        info->state[pos++] = *p;
        ++p;
    }
    info->state[pos] = 0;
    free(buf);
    buf = NULL;
    return 0;
}

static int ShowConnectedDevList(char *buf, int size, int id)
{
    if (buf == NULL) {
        return -1;
    }
    char cmd[BUFSIZE_CMD] = {0};
    char *reqBuf = (char *)calloc(BUFSIZE_REQUEST, sizeof(char));
    if (reqBuf == NULL) {
        return -1;
    }
    if (WpaCtrlCommand(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn, "STA-FIRST",
        reqBuf, BUFSIZE_REQUEST) != 0) {
        LOGE("HostapdCliCmdListSta Failed");
        free(reqBuf);
        reqBuf = NULL;
        return -1;
    }
    do {
        char *pos = reqBuf;
        while (*pos != '\0' && *pos != '\n') { /* return station info, first line is mac address */
            pos++;
        }
        *pos = '\0';
        if (strcmp(reqBuf, "") != 0) {
            int bufLen = strlen(buf);
            int staLen = strlen(reqBuf);
            if (bufLen + staLen + 1 >= size) {
                free(reqBuf);
                reqBuf = NULL;
                return 0;
            }
            buf[bufLen++] = ',';
            for (int i = 0; i < staLen; ++i) {
                buf[bufLen + i] = reqBuf[i];
            }
            buf[bufLen + staLen] = '\0';
        }
        if (snprintf_s(cmd, sizeof(cmd), sizeof(cmd) - 1, "STA-NEXT %s", reqBuf) < 0) {
            break;
        }
    } while (WpaCtrlCommand(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn, cmd, reqBuf, BUFSIZE_REQUEST) == 0);
    free(reqBuf);
    reqBuf = NULL;
    return 0;
}

static int ReloadApConfigInfo(int id)
{
    char buf[BUFSIZE_REQUEST_SMALL] = {0};
    return WpaCtrlCommand(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn, "RELOAD", buf, sizeof(buf));
}

static int DisConnectedDev(const char *mac, int id)
{
    if (mac == NULL) {
        return -1;
    }
    char cmd[BUFSIZE_CMD] = {0};
    char buf[BUFSIZE_REQUEST_SMALL] = {0};

    if (sprintf_s(cmd, sizeof(cmd), "DISASSOCIATE %s", mac) < 0) {
        return -1;
    }
    return WpaCtrlCommand(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn, cmd, buf, sizeof(buf));
}

static int SetCountryCode(const char *code, int id)
{
    if (code == NULL) {
        return -1;
    }
    char cmd[BUFSIZE_CMD] = {0};
    char buf[BUFSIZE_REQUEST_SMALL] = {0};

    if (sprintf_s(cmd, sizeof(cmd), "SET country_code %s", code) < 0) {
        return -1;
    }
    return WpaCtrlCommand(g_hostapdHalDevInfo[id].hostapdHalDev->ctrlConn, cmd, buf, sizeof(buf));
}

static int InitHostapdHal(int id)
{
    if (g_hostapdHalDevInfo[id].hostapdHalDev == NULL) {
        return -1;
    }
    g_hostapdHalDevInfo[id].hostapdHalDev->threadRunFlag = 1;
    if (HostapdCliConnect(id) != 0) {
        return -1;
    }
    return 0;
}

WifiHostapdHalDevice *GetWifiHostapdDev(int id)
{
    if (id < 0 || id >= AP_MAX_INSTANCE) {
        LOGE("Invalid id: %{public}d!", id);
        return NULL;
    }

    if (g_hostapdHalDevInfo[id].hostapdHalDev != NULL) {
        return g_hostapdHalDevInfo[id].hostapdHalDev;
    }

    g_hostapdHalDevInfo[id].hostapdHalDev = (WifiHostapdHalDevice *)calloc(1, sizeof(WifiHostapdHalDevice));
    if (g_hostapdHalDevInfo[id].hostapdHalDev == NULL) {
        LOGE("hostapdHalDev is NULL");
        return NULL;
    }

    /* ************ Register hostapd_cli Interface ************************* */
    g_hostapdHalDevInfo[id].hostapdHalDev->enableAp = EnableAp;
    g_hostapdHalDevInfo[id].hostapdHalDev->disableAp = DisableAp;
    g_hostapdHalDevInfo[id].hostapdHalDev->setApInfo = SetApInfo;
    g_hostapdHalDevInfo[id].hostapdHalDev->addBlocklist = AddBlocklist;
    g_hostapdHalDevInfo[id].hostapdHalDev->delBlocklist = DelBlocklist;
    g_hostapdHalDevInfo[id].hostapdHalDev->status = GetApStatus;
    g_hostapdHalDevInfo[id].hostapdHalDev->showConnectedDevList = ShowConnectedDevList;
    g_hostapdHalDevInfo[id].hostapdHalDev->reloadApConfigInfo = ReloadApConfigInfo;
    g_hostapdHalDevInfo[id].hostapdHalDev->disConnectedDev = DisConnectedDev;
    g_hostapdHalDevInfo[id].hostapdHalDev->setCountryCode = SetCountryCode;

    if (InitHostapdHal(id) != 0) {
        LOGE("InitHostapdHal return failed!!");
        free(g_hostapdHalDevInfo[id].hostapdHalDev);
        g_hostapdHalDevInfo[id].hostapdHalDev = NULL;
        return NULL;
    }
    return g_hostapdHalDevInfo[id].hostapdHalDev;
}

void ReleaseHostapdDev(int id)
{
    if (g_hostapdHalDevInfo[id].hostapdHalDev != NULL) {
        HostapdCliClose(id);
        free(g_hostapdHalDevInfo[id].hostapdHalDev);
        g_hostapdHalDevInfo[id].hostapdHalDev = NULL;
    }
}
