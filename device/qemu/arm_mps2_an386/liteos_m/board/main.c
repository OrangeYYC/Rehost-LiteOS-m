/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "los_config.h"
#include "uart.h"
#include "los_debug.h"
#include "lan9118_eth_drv.h"
#include "los_task.h"

UINT32 taskId;
TSK_INIT_PARAM_S MainTask;

extern void entry_main();

extern void OsRecycleFinishedTask();
LITE_OS_SEC_TEXT_INIT UINT32 LOS_KernelInit(VOID);
LITE_OS_SEC_TEXT_INIT UINT32 LOS_Start(VOID);
unsigned int LosAppInit(VOID);
extern unsigned int LosShellInit(void);
/*****************************************************************************
 Function    : main
 Description : Main function entry
 Input       : None
 Output      : None
 Return      : None
 *****************************************************************************/
LITE_OS_SEC_TEXT_INIT int main(void) // 主函数：从这里开始
{
    unsigned int ret = LOS_OK;

    // AFL-fuzz
    //UartInit();

    ret = LOS_KernelInit(); // 内核初始化函数
    if (ret != LOS_OK) {
        printf("LiteOS kernel init failed! ERROR: 0x%x\n", ret);
        return 0;
    }
#if (LOSCFG_SUPPORT_LITTLEFS == 1)
    LfsLowLevelInit();
#endif

    // Uart0RxIrqRegister();

    // NetInit();

#if (LOSCFG_USE_SHELL == 1)
    // ret = LosShellInit();
    if (ret != LOS_OK) {
        printf("LosAppInit failed! ERROR: 0x%x\n", ret);
    }
#endif

    // ret = LosAppInit();
    if (ret != LOS_OK) {
        printf("LosAppInit failed! ERROR: 0x%x\n", ret);
    }

    MainTask.pfnTaskEntry = (TSK_ENTRY_FUNC)entry_main;
    MainTask.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    MainTask.pcName = "Main Entry";
    MainTask.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    MainTask.uwResved = 0x01000;
    LOS_TaskCreate(&taskId, &MainTask);

    // AFl-fuzz
    //while (1) {
//        __asm volatile("wfi");
    //}

    LOS_Start(); // LiteOS 核心的开始

    printf("GoogBye!\n");

    ret = LOS_TaskDelay(taskId);
    ret = LOS_TaskDelete(g_idleTaskID);
    OsRecycleFinishedTask();

    return 0;
}

