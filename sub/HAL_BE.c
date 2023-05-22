#include "HAL_BE.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#define FUZZER_BUF_SIZE (6*1024)

int input_loaded = 0;
// char fuzzer_buf[FUZZER_BUF_SIZE] = {0};
char *fuzzer_buf = NULL;
int fuzzer_buf_size = 0;
int fuzzer_buf_cursor = 0;

int HAL_BE_return_0(){
    return 0;
}

int HAL_BE_return_1(){
    return 1;
}

int HAL_BE_Out(int len){
    return len;
}

void afl_load_input(char* path){
    FILE* fp = NULL;
    if(!(fp=fopen(path, "r"))) {
        exit(-1);
    }
    if(fseek(fp, 0, SEEK_END)) {
        exit(-1);
    }
    fuzzer_buf_size = ftell(fp);
    if(fuzzer_buf_size == -1){
        exit(-1);
    }
    if(fuzzer_buf_size > FUZZER_BUF_SIZE){
        fuzzer_buf_size = FUZZER_BUF_SIZE;
    }
    rewind(fp);
    fuzzer_buf = malloc(FUZZER_BUF_SIZE);
    fread(fuzzer_buf, fuzzer_buf_size, 1, fp);
    fclose(fp);
}

int HAL_BE_In(void* buf, int len){
    return read(0, buf, len);
    // if(fuzzer_buf_cursor + len > fuzzer_buf_size){
    //     // struct timeval tv1;
    //     // gettimeofday(&tv1,NULL);
    //     // printf("%lu us\n", (tv1.tv_sec*1000000 + tv1.tv_usec));
    //     exit(0);
    // }else{
    //     memcpy(buf, fuzzer_buf + fuzzer_buf_cursor, len);
    //     fuzzer_buf_cursor += len;
    //     return len;
    // }
}