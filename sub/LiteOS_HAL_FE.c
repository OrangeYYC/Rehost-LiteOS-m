#include "LiteOS_HAL_FE.h"

void LiteOS_HAL_FE_void(){}

int LiteOS_HAL_FE_return_success(){
    return HAL_BE_return_0();
}

int LiteOS_HAL_FE_Out(int len){
    return HAL_BE_Out(len);
}

int LiteOS_HAL_FE_In(void* buf, int len){
    // if(HAL_BE_In(buf, len) != len){
    //     exit(0);
    //     return -1;
    // }
    // return len;
    int temp_len = HAL_BE_In(buf, len);
    return temp_len;
}