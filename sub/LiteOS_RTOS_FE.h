#include <string.h> // patch for c99
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

int Hard_Fault_FE();

unsigned int ArchIntLock();
unsigned int ArchIntUnlock();
void ArchIntRestore(unsigned int save);

void PMCU_FE_Task_Create(void* func, unsigned int params[], int param_count, unsigned int taskid);
void OsTaskSchedule();
void OsStartToRun(void *task);
void ArchExcInit(void);

/* Clock */
void HalClockInit(void);
void HalClockStart(void);
uint64_t HalClockGetCycles(void);
void HalDelayUs(uint32_t usecs);

/* Patch for c99 */
size_t strnlen(const char *s, size_t n);