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

#include "wifi_hal_module_manage.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "securec.h"
#include "wifi_hal_define.h"
#include "wifi_log.h"
#include "wifi_wpa_hal.h"

#undef LOG_TAG
#define LOG_TAG "WifiHalModuleManage"

static ModuleInfo *g_halModuleList = NULL;

#define MAX_WPA_MAIN_ARGC_NUM 20
#define MAX_WPA_MAIN_ARGV_LEN 128

struct StWpaMainParam {
    int argc;
    char argv[MAX_WPA_MAIN_ARGC_NUM][MAX_WPA_MAIN_ARGV_LEN];
};

static void SplitCmdString(const char *startCmd, struct StWpaMainParam *pParam)
{
    if (pParam == NULL) {
        return;
    }
    if (startCmd == NULL) {
        pParam->argc = 0;
        return;
    }
    const char *p = startCmd;
    int i = 0;
    int j = 0;
    while (*p != '\0') {
        if (*p == ' ') {
            if (j <= MAX_WPA_MAIN_ARGV_LEN - 1) {
                pParam->argv[i][j] = '\0';
            } else {
                pParam->argv[i][MAX_WPA_MAIN_ARGV_LEN - 1] = '\0';
            }
            ++i;
            j = 0;
            if (i >= MAX_WPA_MAIN_ARGC_NUM) {
                break;
            }
        } else {
            if (j < MAX_WPA_MAIN_ARGV_LEN - 1) {
                pParam->argv[i][j] = *p;
                ++j;
            }
        }
        ++p;
    }
    if (i >= MAX_WPA_MAIN_ARGC_NUM) {
        pParam->argc = MAX_WPA_MAIN_ARGC_NUM;
    } else {
        pParam->argc = i + 1;
    }
    return;
}

static void *WpaThreadMain(void *p)
{
    if (p == NULL) {
        return NULL;
    }
    const char *startCmd = (const char *)p;
    struct StWpaMainParam param = {0};
    SplitCmdString(startCmd, &param);
#ifdef OHOS_ARCH_LITE
    void *handleLibWpa = dlopen("libwpa.so", RTLD_NOW | RTLD_LOCAL);
#else
#ifdef __aarch64__
    void *handleLibWpa = dlopen("/system/lib64/libwpa.z.so", RTLD_NOW | RTLD_LOCAL | RTLD_NODELETE);
#else
    void *handleLibWpa = dlopen("/system/lib/libwpa.z.so", RTLD_NOW | RTLD_LOCAL | RTLD_NODELETE);
#endif
#endif
    if (handleLibWpa == NULL) {
        LOGE("dlopen libwpa failed.");
        return NULL;
    }
    int (*func)(int, char **) = NULL;
    if (strcmp(param.argv[0], "wpa_supplicant") == 0) {
        func = (int (*)(int, char **))dlsym(handleLibWpa, "wpa_main");
    } else {
        func = (int (*)(int, char **))dlsym(handleLibWpa, "ap_main");
    }
    if (func == NULL) {
        dlclose(handleLibWpa);
        LOGE("dlsym wpa_main failed.");
        return NULL;
    }
    char *tmpArgv[MAX_WPA_MAIN_ARGC_NUM] = {0};
    for (int i = 0; i < param.argc; i++) {
        tmpArgv[i] = param.argv[i];
    }
    int ret = func(param.argc, tmpArgv);
    LOGD("run wpa_main ret:%{public}d.\n", ret);
    if (dlclose(handleLibWpa) != 0) {
        LOGE("dlclose libwpa failed.");
        return NULL;
    }
    return NULL;
}

int StartModuleInternal(const char *moduleName, const char *startCmd, pid_t *pProcessId)
{
    if (moduleName == NULL || startCmd == NULL || pProcessId == NULL) {
        return HAL_FAILURE;
    }
    pid_t pid = fork();
    if (pid < 0) {
        LOGE("Create wpa process failed!");
        return HAL_FAILURE;
    }
    if (pid == 0) { /* sub process */
        // The child process will receive the exit signal when the parent process exits.
        prctl(PR_SET_PDEATHSIG, SIGKILL);
        pthread_t tid;
        int ret = pthread_create(&tid, NULL, WpaThreadMain, (void *)startCmd);
        if (ret != 0) {
            LOGE("Create wpa thread failed!");
            return HAL_FAILURE;
        }
        pthread_join(tid, NULL);
        exit(0);
    } else {
        LOGE("Create wpa process id is [%{public}d]", pid);
        *pProcessId = pid;
    }
    return HAL_SUCCESS;
}

static int StopModuleInternalKillProcess(pid_t processId)
{
    LOGI("Stop module kill process: %{public}d", processId);
    if (kill(processId, SIGTERM) == -1) {
        if (ESRCH == errno) {
            LOGI("kill [%{public}d] success, pid no exist", processId);
            return HAL_SUCCESS;
        }
        LOGE("kill [%{public}d] failed", processId);
        return HAL_FAILURE;
    }
    return HAL_SUCCESS;
}

static int StopModuleInternalCheckProcess(const char *moduleName, pid_t processId)
{
    if (moduleName == NULL) {
        return HAL_SUCCESS;
    }
    LOGI("Stop module internal check wpa process: %{public}d", processId);
    const int STOP_MODULE_TRY_TIMES = 30;
    const int SLEEP_TIME_US = 1000 * 100; // 100ms
    int tryTimes = STOP_MODULE_TRY_TIMES;
    while (tryTimes-- >= 0) {
        int ret = waitpid(processId, NULL, WNOHANG);
        if (ret <= 0) {
            LOGI("Waitpid %{public}d ret %{public}d, tryTimes %{public}d and retry", processId, ret, tryTimes);
            usleep(SLEEP_TIME_US);
            continue;
        } else {
            LOGI("Stop wpa process [%{public}d] success, tryTimes %{public}d", processId, tryTimes);
            return HAL_SUCCESS;
        }
    }
    LOGE("Stop wpa process [%{public}d] failed for timeout, try to kill process", processId);
    StopModuleInternalKillProcess(processId);
    return HAL_SUCCESS;
}

static int StopModuleInternalSendTerminate(void)
{
    WifiWpaInterface *pWpaInterface = GetWifiWapGlobalInterface();
    if (pWpaInterface == NULL) {
        LOGE("Get wpa global interface failed!");
        return HAL_FAILURE;
    }
    int ret = pWpaInterface->wpaCliTerminate(pWpaInterface);
    return (ret == 0 ? HAL_SUCCESS : HAL_FAILURE);
}

int StopModuleInternal(const char *moduleName, pid_t processId, bool isHostapd)
{
    int ret;
    if (isHostapd) {
        ret = StopModuleInternalKillProcess(processId);
    } else {
        ret = StopModuleInternalSendTerminate();
    }
    if (ret != HAL_SUCCESS) {
        LOGE("Send stop module command failed!");
    }
    return StopModuleInternalCheckProcess(moduleName, processId);
}

ModuleInfo *FindModule(const char *moduleName)
{
    if (moduleName == NULL) {
        return NULL;
    }
    ModuleInfo *p = g_halModuleList;
    while (p != NULL) {
        if (strcmp(p->szModuleName, moduleName) == 0) {
            break;
        } else {
            p = p->next;
        }
    }
    return p;
}

ModuleManageRetCode StartModule(const char *moduleName, const char *startCmd)
{
    if (moduleName == NULL || startCmd == NULL) {
        return MM_FAILED;
    }
    ModuleInfo *p = FindModule(moduleName);
    if (p != NULL) {
        p->referenceCount += 1;
        LOGD("module %{public}s has been started, current reference is %{public}d", moduleName, p->referenceCount);
        return MM_SUCCESS;
    }
    p = (ModuleInfo *)calloc(1, sizeof(ModuleInfo));
    if (p == NULL) {
        return MM_FAILED;
    }
    if (strcpy_s(p->szModuleName, sizeof(p->szModuleName), moduleName) != EOK) {
        free(p);
        p = NULL;
        return MM_FAILED;
    }
    int ret = StartModuleInternal(moduleName, startCmd, &(p->processId));
    if (ret != 0) { /* start module failed! */
        free(p);
        p = NULL;
        return MM_FAILED;
    }
    p->referenceCount = 1;
    p->next = g_halModuleList;
    g_halModuleList = p;
    return MM_SUCCESS;
}

ModuleManageRetCode StopModule(const char *moduleName, bool isHostapd)
{
    if (moduleName == NULL) {
        return MM_FAILED;
    }
    ModuleInfo *p = FindModule(moduleName);
    if (p == NULL) {
        return MM_SUCCESS;
    }
    p->referenceCount -= 1;
    if (p->referenceCount > 0) {
        LOGD("module %{public}s reference left %{public}d, return ok", moduleName, p->referenceCount);
        return MM_REDUCE_REFERENCE;
    }
    int ret = StopModuleInternal(p->szModuleName, p->processId, isHostapd);
    if (ret != 0) { /* stop module failed! */
        p->referenceCount += 1;
        return MM_FAILED;
    }
    if (p == g_halModuleList) {
        g_halModuleList = p->next;
    } else {
        ModuleInfo *q = g_halModuleList;
        while (q != NULL && q->next != p) {
            q = q->next;
        }
        if (q != NULL) {
            q->next = p->next;
        }
    }
    free(p);
    p = NULL;
    return MM_SUCCESS;
}
