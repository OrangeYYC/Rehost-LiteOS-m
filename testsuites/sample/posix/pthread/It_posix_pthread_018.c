/*
 * Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
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

#include "It_posix_pthread.h"

static VOID *PthreadF01(VOID *num)
{
    int *arg = (int *)num;

    for (int i = 0; i < PTHREAD_THREADS_NUM; i++) {
        dprintf("Passed argument %d for thread\n", arg[i]);
        ICUNIT_TRACK_EQUAL(arg[i], i + 1, arg[i]);
    }

    return NULL;
}

static UINT32 Testcase(VOID)
{
    pthread_t newTh;
    INT32 arg[PTHREAD_THREADS_NUM], ret;

    for (int i = 0; i < PTHREAD_THREADS_NUM; i++) {
        arg[i] = i + 1;
    }

    ret = pthread_create(&newTh, NULL, PthreadF01, (void *)&arg);
    ICUNIT_GOTO_EQUAL(ret, 0, ret, EXIT);

    ret = pthread_join(newTh, NULL);
    ICUNIT_GOTO_EQUAL(ret, 0, ret, EXIT);

    return LOS_OK;
EXIT:
    pthread_detach(newTh);
    return LOS_OK;
}

/**
 * @tc.name: ItPosixPthread018
 * @tc.desc: Test interface pthread_join
 * @tc.type: FUNC
 * @tc.require: issueI5TIRQ
 */

VOID ItPosixPthread018(VOID)
{
    TEST_ADD_CASE("ItPosixPthread018", Testcase, TEST_POSIX, TEST_PTHREAD, TEST_LEVEL2, TEST_FUNCTION);
}
