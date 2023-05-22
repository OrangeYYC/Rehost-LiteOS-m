//
// Created by ubuntu on 23-4-10.
//
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "los_task.h"
#include <pthread.h>
#include <signal.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>

UINT32 taskId1;
TSK_INIT_PARAM_S LEDTask1;

UINT32 taskId2;
TSK_INIT_PARAM_S LEDTask2;

void sub_thread();
void sub_thread2();
void entry_main(){
    printf("Hello World\n");
    LEDTask1.pfnTaskEntry = (TSK_ENTRY_FUNC)sub_thread;
    LEDTask1.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    LEDTask1.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    LEDTask1.pcName = "Task2";
    LEDTask1.uwResved = 0x0100;
    LOS_TaskCreate(&taskId1, &LEDTask1);

    LEDTask2.pfnTaskEntry = (TSK_ENTRY_FUNC)sub_thread2;
    LEDTask2.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    LEDTask2.usTaskPrio = LOSCFG_BASE_CORE_TSK_DEFAULT_PRIO;
    LEDTask2.pcName = "Task3";
    LEDTask2.uwResved = 0x01000;
    LOS_TaskCreate(&taskId2, &LEDTask2);

    for(int i=0; i<500000; i++){
        printf("A");
    }
}

void sub_thread(){
    for(int i=0; i<500000; i++){
        printf("B");
    }
}

void sub_thread2(){
    for(int i=0; i<500000; i++){
        printf("C");
    }
}