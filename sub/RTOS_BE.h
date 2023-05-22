#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// CPU fall into fault status
int Fault_BE();
// Interrupt
int PMCU_BE_Disable_Irq();
int PMCU_BE_Enable_Irq();
// Systick
void Setup_Ticker(unsigned int time_interval);
uint32_t PMCU_BE_Get_Systick();
// Task
void PMCU_BE_Task_Create(void* func, unsigned int params[], int param_count, unsigned int taskid, void (*tick_handler)(void));
void Start_Scheduler(unsigned int taskid, unsigned int interval);
void Pthread_Schedule(unsigned int new_task_id, unsigned int run_task_id);
void Systick_Signal_Handler(int sig);
void Tick_Handler_TaskSchedule();
void User_Task_Tick_Handler();
void idle_task();