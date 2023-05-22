#include "HAL_BE.h"

void LiteOS_HAL_FE_void();
int LiteOS_HAL_FE_return_success();
int LiteOS_HAL_FE_Out(int len);
int LiteOS_HAL_FE_In(void* buf, int len);
#define HalIrqInit(void)    LiteOS_HAL_FE_void() 