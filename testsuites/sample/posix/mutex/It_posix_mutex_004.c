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

/* pthread_mutexattr_destroy 2-1.c
 * Test pthread_mutexattr_destroy()
 * A destroyed 'attr' attributes object can be reinitialized using
 * pthread_mutexattr_init(); the results of otherwise referencing the
 * object after it has been destroyed are undefined.
 *
 * Steps:
 * 1.  Initialize a pthread_mutexattr_t object using pthread_mutexattr_init()
 * 2.  Destroy that initialized attribute using pthread_mutexattr_destroy()
 * 3.  Initialize the pthread_mutexattr_t object again. This should not result
 * in any uwErr.
 *
 */

static UINT32 Testcase(VOID)
{
    pthread_mutexattr_t mta;
    int rc;

    /* Initialize a mutex attributes object */
    rc = pthread_mutexattr_init(&mta);
    ICUNIT_ASSERT_EQUAL(rc, 0, rc);

    /* Destroy the mutex attributes object */
    rc = pthread_mutexattr_destroy(&mta);
    ICUNIT_GOTO_EQUAL(rc, 0, rc, EXIT);

    /* Initialize the mutex attributes object again.  This shouldn't result in an uwErr. */
    rc = pthread_mutexattr_init(&mta);
    ICUNIT_ASSERT_EQUAL(rc, 0, rc);

    rc = pthread_mutexattr_destroy(&mta);
    ICUNIT_GOTO_EQUAL(rc, 0, rc, EXIT);

    return LOS_OK;

EXIT:
    pthread_mutexattr_destroy(&mta);
    return LOS_OK;
}

/**
 * @tc.name: ItPosixMux004
 * @tc.desc: Test interface pthread_mutexattr_destroy
 * @tc.type: FUNC
 * @tc.require: issueI5WZI6
 */

VOID ItPosixMux004(void)
{
    TEST_ADD_CASE("ItPosixMux004", Testcase, TEST_POSIX, TEST_MUX, TEST_LEVEL2, TEST_FUNCTION);
}
