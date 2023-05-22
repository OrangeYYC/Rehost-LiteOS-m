/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <string>
#include <gtest/gtest.h>

#include "softbus_error_code.h"
#include "softbus_hisysevt_transreporter.h"
#include "softbus_hidumper_trans.c"

using namespace std;
using namespace testing::ext;

static const TransDumpLaneLinkType g_errLinkType = (TransDumpLaneLinkType)(-1);
static const int32_t TEST_TRANS_UID = 1;
static const int32_t TEST_TRANS_PID = 1;
static const int32_t ERR_TRANS_UID = 1;
static const int32_t ERR_TRANS_PID = 1;
static const int32_t TEST_TRANS_ARGV_MAX_NUM = 10;
static const int32_t TRANS_DUMP_PROCESS_TEST_NUM = 4;
static const int32_t ERR_FD = -1;
static const int32_t TEST_FD = 0;
static const int32_t ERR_ARGC = -1;
static const int32_t TEST_ARGC_ONE = 1;
static const int32_t TEST_ARGC_TWO = 2;
static const char *g_testSessionName = "testSessionName";
static const char *g_testPkgName = "testPkg";

namespace OHOS {
class TransDfxTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void TransDfxTest::SetUpTestCase(void) {}

void TransDfxTest::TearDownTestCase(void) {}

void TransDfxTest::SetUp(void) {}

void TransDfxTest::TearDown(void) {}

typedef struct {
    int32_t fd;
    int32_t argc;
    const char* argv[TEST_TRANS_ARGV_MAX_NUM];
} TransArgvNode;

static AppInfo g_testAppInfo = {
    .groupId = "1234d575ad685798989adfe358aec45",
    .sessionKey = "867f996ae4567df",
    .reqId = "123456",
    .peerNetWorkId = "1456df44568e758b545aad45875e",
    .routeType = WIFI_STA,
    .businessType = BUSINESS_TYPE_MESSAGE,
    .streamType = COMMON_VIDEO_STREAM,
    .udpConnType = UDP_CONN_TYPE_P2P,
    .udpChannelOptType = TYPE_UDP_CHANNEL_CLOSE,
    .fd = 2,
    .appType = APP_TYPE_NORMAL,
    .protocol = 1,
    .encrypt = 1,
    .algorithm = 2,
    .crc = 101,
    .fileProtocol = 1
};

static void TransRegisterSessionTest1(int fd)
{
    SoftBusTransDumpRegisterSession(fd, g_testPkgName, g_testSessionName, TEST_TRANS_UID, TEST_TRANS_PID);
}
static void TransRunningSessionInfo1(int fd)
{
    SoftBusTransDumpRunningSession(fd, DUMPER_LANE_BR, &g_testAppInfo);
}

static void TransRegisterSessionTest2(int fd)
{
    SoftBusTransDumpRegisterSession(fd, nullptr, g_testSessionName, TEST_TRANS_UID, TEST_TRANS_PID);
}


static void TransRunningSessionInfo2(int fd)
{
    SoftBusTransDumpRunningSession(fd, DUMPER_LANE_BR, nullptr);
}

TransArgvNode g_validTransCmdArray[TRANS_DUMP_PROCESS_TEST_NUM] = {
    {TEST_FD, TEST_ARGC_ONE, {"-h"}},
    {TEST_FD, TEST_ARGC_ONE, {"-l"}},
    {TEST_FD, TEST_ARGC_TWO, {"-l", "registed_sessionlist"}},
    {TEST_FD, TEST_ARGC_TWO, {"-l", "concurrent_sessionlist"}},
};
/**
 * @tc.name: CreateTransFaultEvtTest001
 * @tc.desc: Verify SoftbusReportTransErrorEvt function, use the normal parameter.
 * @tc.type: FUNC
 * @tc.require: I5NJEO
 */
HWTEST_F(TransDfxTest, CreateTransFaultEvtTest001, TestSize.Level0)
{
    int32_t errorCode = SOFTBUS_ACCESS_TOKEN_DENIED;
    SoftbusReportTransErrorEvt(errorCode);
}

/**
 * @tc.name: SoftBusTransDumpHandler_001
 * @tc.desc: Verify SoftBusTransDumpHandler function, use the normal parameter.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransDfxTest, SoftBusTransDumpHandler_001, TestSize.Level1)
{
    int fd = 1;
    int argc = 1;
    const char* argv = "aaa";
    int ret = SoftBusTransDumpHandler(fd, argc, &argv);
    EXPECT_EQ(SOFTBUS_OK, ret);
}

/**
 * @tc.name: SoftBusTransDumpHandler002
 * @tc.desc: Verify SoftBusTransDumpHandler function, use valid param, return SOFTBUS_OK.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransDfxTest, SoftBusTransDumpHandler002, TestSize.Level0)
{
    SetShowRegisterSessionInfosFunc(TransRegisterSessionTest1);
    SetShowRunningSessionInfosFunc(TransRunningSessionInfo1);

    int32_t ret;
    for (int32_t i = 0; i < TRANS_DUMP_PROCESS_TEST_NUM; i++) {
        ret = SoftBusTransDumpHandler(g_validTransCmdArray[i].fd, g_validTransCmdArray[i].argc,
            g_validTransCmdArray[i].argv);
        EXPECT_EQ(SOFTBUS_OK, ret);
    }
}

/**
 * @tc.name: SoftBusTransDumpHandler003
 * @tc.desc: Verify SoftBusTransDumpHandler function, use hidumperHandler use valid param, return SOFTBUS_OK,
 *     but the TransRegisterSessionTest2() and TransRunningSessionInfo2 use invalid param, test error switch
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransDfxTest, SoftBusTransDumpHandler003, TestSize.Level0)
{
    SetShowRegisterSessionInfosFunc(TransRegisterSessionTest2);
    SetShowRunningSessionInfosFunc(TransRunningSessionInfo2);
    int32_t ret;
    for (int32_t i = 0; i < TRANS_DUMP_PROCESS_TEST_NUM; i++) {
        ret = SoftBusTransDumpHandler(g_validTransCmdArray[i].fd, g_validTransCmdArray[i].argc,
            g_validTransCmdArray[i].argv);
        EXPECT_EQ(SOFTBUS_OK, ret);
    }
}

/**
 * @tc.name: SoftBusTransDumpHandler004
 * @tc.desc: Verify SoftBusTransDumpHandler function, use hidumperHandler use valid param, return SOFTBUS_OK,
 *     but the TransRegisterSessionTest2() and TransRunningSessionInfo2 use invalid param, test error switch
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransDfxTest, SoftBusTransDumpHandler004, TestSize.Level0)
{
    TransArgvNode testInValidTransCmdArray[TRANS_DUMP_PROCESS_TEST_NUM] = {
        {ERR_FD, TEST_ARGC_ONE, {"-h"}},
        {TEST_FD, ERR_ARGC, {"-l"}},
        {ERR_FD, TEST_ARGC_ONE, {"-l", "registed_sessionlist"}},
        {ERR_FD, TEST_ARGC_TWO, {"-l", "concurrent_sessionlist"}},
    };
    int32_t ret;
    for (int32_t i = 0; i < TRANS_DUMP_PROCESS_TEST_NUM; i++) {
        ret = SoftBusTransDumpHandler(testInValidTransCmdArray[i].fd, testInValidTransCmdArray[i].argc,
            testInValidTransCmdArray[i].argv);
        EXPECT_EQ(SOFTBUS_ERR, ret);
    }
    ret = SoftBusTransDumpHandler(TEST_FD, TEST_ARGC_ONE, nullptr);
    EXPECT_EQ(SOFTBUS_ERR, ret);
}

/**
 * @tc.name: SetShowRegisterSessionInfosFunc001
 * @tc.desc: Verify SetShowRegisterSessionInfosFunc function, use hidumperHandler use valid param, return SOFTBUS_ERR.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransDfxTest, SetShowRegisterSessionInfosFunc001, TestSize.Level0)
{
    SetShowRegisterSessionInfosFunc(nullptr);
    SetShowRunningSessionInfosFunc(nullptr);

    int32_t ret = SoftBusTransDumpHandler(TEST_FD, TEST_ARGC_ONE, nullptr);
    EXPECT_EQ(SOFTBUS_ERR, ret);
}

/**
 * @tc.name: SoftBusTransDumpRegisterSession001
 * @tc.desc: Verify SoftBusTransDumpRegisterSession function, use hidumperHandler use valid param, return SOFTBUS_ERR.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F(TransDfxTest, SoftBusTransDumpRegisterSession001, TestSize.Level0)
{
    SoftBusTransDumpRegisterSession(ERR_FD, g_testPkgName, g_testSessionName, TEST_TRANS_UID, TEST_TRANS_PID);
    SoftBusTransDumpRunningSession(ERR_FD, DUMPER_LANE_BR, &g_testAppInfo);

    int32_t ret = SoftBusTransDumpHandler(TEST_FD, TEST_ARGC_ONE, nullptr);
    EXPECT_EQ(SOFTBUS_ERR, ret);

    SoftBusTransDumpRegisterSession(TEST_FD, nullptr, g_testSessionName, TEST_TRANS_UID, TEST_TRANS_PID);
    SoftBusTransDumpRunningSession(TEST_FD, g_errLinkType, &g_testAppInfo);

    ret = SoftBusTransDumpHandler(TEST_FD, TEST_ARGC_ONE, nullptr);
    EXPECT_EQ(SOFTBUS_ERR, ret);

    SoftBusTransDumpRegisterSession(TEST_FD, g_testPkgName, nullptr, TEST_TRANS_UID, TEST_TRANS_PID);
    SoftBusTransDumpRunningSession(TEST_FD, DUMPER_LANE_LINK_TYPE_BUTT, &g_testAppInfo);

    ret = SoftBusTransDumpHandler(TEST_FD, TEST_ARGC_ONE, nullptr);
    EXPECT_EQ(SOFTBUS_ERR, ret);

    SoftBusTransDumpRegisterSession(TEST_FD, g_testPkgName, g_testSessionName, ERR_TRANS_UID, TEST_TRANS_PID);
    SoftBusTransDumpRunningSession(TEST_FD, DUMPER_LANE_WLAN, nullptr);

    ret = SoftBusTransDumpHandler(TEST_FD, TEST_ARGC_ONE, nullptr);
    EXPECT_EQ(SOFTBUS_ERR, ret);

    SoftBusTransDumpRegisterSession(TEST_FD, g_testPkgName, g_testSessionName, TEST_TRANS_UID, ERR_TRANS_PID);
    SoftBusTransDumpRunningSession(TEST_FD, DUMPER_LANE_WLAN, &g_testAppInfo);

    ret = SoftBusTransDumpHandler(TEST_FD, TEST_ARGC_ONE, nullptr);
    EXPECT_EQ(SOFTBUS_ERR, ret);

    SoftBusTransDumpRegisterSession(TEST_FD, g_testPkgName, g_testSessionName, TEST_TRANS_UID, TEST_TRANS_PID);
    SoftBusTransDumpRunningSession(TEST_FD, DUMPER_LANE_WLAN, &g_testAppInfo);

    const char* tmpArgv = "aaa";
    ret = SoftBusTransDumpHandler(TEST_FD, TEST_ARGC_ONE, &tmpArgv);
    EXPECT_EQ(SOFTBUS_OK, ret);
}
} // namespace OHOS