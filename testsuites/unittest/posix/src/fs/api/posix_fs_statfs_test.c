/*
 * Copyright (c) 2023-2023 Huawei Device Co., Ltd. All rights reserved.
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

#include "posix_fs_test.h"

/* *
 * @tc.number   SUB_KERNEL_FS_STATFS_OK
 * @tc.name     statfs
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, TestFsStatfsOK, Function | MediumTest | Level1)
{
    struct statfs buf = { 0 };

    int32_t ret = statfs(TEST_ROOT, &buf);
    ICUNIT_ASSERT_NOT_EQUAL(ret, POSIX_FS_IS_ERROR, ret);

    return POSIX_FS_NO_ERROR;
}

/* *
 * @tc.number   SUB_KERNEL_FS_STATFS_EINVAL
 * @tc.name     statfs
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, TestFsStatfsEINVAL, Function | MediumTest | Level1)
{
    struct statfs buf = { 0 };

    int32_t ret = statfs(NULL, &buf);
    ICUNIT_ASSERT_EQUAL(errno, EINVAL, POSIX_FS_IS_ERROR);
    ICUNIT_ASSERT_EQUAL(ret, POSIX_FS_IS_ERROR, ret);

    ret = statfs(TEST_ROOT, NULL);
    ICUNIT_ASSERT_EQUAL(errno, EINVAL, POSIX_FS_IS_ERROR);
    ICUNIT_ASSERT_EQUAL(ret, POSIX_FS_IS_ERROR, ret);

    return POSIX_FS_NO_ERROR;
}
/* *
 * @tc.number   SUB_KERNEL_FS_STATFS_ENOENT
 * @tc.name     statfs
 * @tc.desc     [C- SOFTWARE -0200]
 */
LITE_TEST_CASE(PosixFsFuncTestSuite, TestFsStatfsENOENT, Function | MediumTest | Level1)
{
    int32_t ret;
    struct statfs buf = { 0 };

    struct MountPoint *mountBak = g_mountPoints;
    g_mountPoints = NULL;
    ret = statfs(TEST_ROOT, &buf);
    g_mountPoints = mountBak;
    ICUNIT_ASSERT_EQUAL(errno, ENOENT, POSIX_FS_IS_ERROR);
    ICUNIT_ASSERT_EQUAL(ret, POSIX_FS_IS_ERROR, ret);

    return POSIX_FS_NO_ERROR;
}

void PosixFsStatfsTest(void)
{
    RUN_ONE_TESTCASE(TestFsStatfsOK);
    RUN_ONE_TESTCASE(TestFsStatfsEINVAL);
    RUN_ONE_TESTCASE(TestFsStatfsENOENT);
}
