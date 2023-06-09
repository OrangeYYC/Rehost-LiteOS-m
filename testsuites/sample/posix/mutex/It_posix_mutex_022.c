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

#include "It_posix_mutex.h"

static pthread_mutex_t g_mutex042;

static void *TaskF01(void *arg)
{
    int ret;
    ret = pthread_mutex_unlock(&g_mutex042);
    ICUNIT_TRACK_EQUAL(ret, EINVAL, ret);

    return NULL;
}

static UINT32 Testcase(VOID)
{
    int ret;
    pthread_mutexattr_t ma;
    pthread_t th;

    ret = pthread_mutexattr_init(&ma);
    ICUNIT_ASSERT_EQUAL(ret, 0, ret);

    ret = pthread_mutex_init(&g_mutex042, &ma);
    ICUNIT_ASSERT_EQUAL(ret, 0, ret);

    ret = pthread_mutex_lock(&g_mutex042);
    ICUNIT_ASSERT_EQUAL(ret, 0, ret);

    /* destroy the mutex attribute object */
    ret = pthread_mutexattr_destroy(&ma);
    ICUNIT_ASSERT_EQUAL(ret, 0, ret);

    ret = pthread_create(&th, NULL, TaskF01, NULL);
    ICUNIT_ASSERT_EQUAL(ret, 0, ret);

    /* Let the thread terminate */
    ret = pthread_join(th, NULL);
    ICUNIT_GOTO_EQUAL(ret, 0, ret, EXIT1);

    /* We can clean everything and exit */
    ret = pthread_mutex_unlock(&g_mutex042);
    ICUNIT_GOTO_EQUAL(ret, 0, ret, EXIT2);

    ret = pthread_mutex_destroy(&g_mutex042);
    ICUNIT_GOTO_EQUAL(ret, 0, ret, EXIT3);
    
    return LOS_OK;

EXIT1:
    pthread_mutex_unlock(&g_mutex042);

EXIT2:
    pthread_mutex_destroy(&g_mutex042);

EXIT3:
    return LOS_NOK;
}

/**
 * @tc.name: ItPosixMux022
 * @tc.desc: Test interface pthread_mutex_unlock
 * @tc.type: FUNC
 * @tc.require: issueI5WZI6
 */

VOID ItPosixMux022(void)
{
    TEST_ADD_CASE("ItPosixMux022", Testcase, TEST_POSIX, TEST_MUX, TEST_LEVEL2, TEST_FUNCTION);
}
