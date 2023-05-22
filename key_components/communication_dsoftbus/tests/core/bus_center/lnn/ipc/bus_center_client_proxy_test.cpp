/*
* Copyright (c) 2022 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <gtest/gtest.h>
#include <securec.h>
#include "softbus_errcode.h"
#include "bus_center_client_proxy.h"

namespace OHOS {
using namespace testing::ext;
constexpr uint32_t TEST_DATA_LEN = 10;

class BusCenterClientProxyTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void BusCenterClientProxyTest::SetUpTestCase()
{
}

void BusCenterClientProxyTest::TearDownTestCase()
{
}

void BusCenterClientProxyTest::SetUp()
{
}

void BusCenterClientProxyTest::TearDown()
{
}

/*
* @tc.name: CLIENT_ON_JOIN_META_NODE_RESULT_Test_001
* @tc.desc: client on join meta node result test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(BusCenterClientProxyTest, CLIENT_ON_JOIN_META_NODE_RESULT_Test_001, TestSize.Level1)
{
    const char *pkgName = "000";
    void *addr = nullptr;
    uint32_t addrTypeLen = TEST_DATA_LEN;
    const char *networkId = "1234";
    int32_t retCode = 0;
    int32_t ret;

    ret = ClientOnJoinMetaNodeResult(nullptr, addr, addrTypeLen, networkId, retCode);
    EXPECT_EQ(ret, SOFTBUS_ERR);
    ret = ClientOnJoinMetaNodeResult(pkgName, addr, addrTypeLen, networkId, retCode);
    EXPECT_EQ(ret, SOFTBUS_ERR);
}

/*
* @tc.name: CLIENT_ON_LEAVE_META_NODE_RESULT_Test_001
* @tc.desc: client on leave meta node result test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(BusCenterClientProxyTest, CLIENT_ON_LEAVE_META_NODE_RESULT_Test_001, TestSize.Level0)
{
    const char *pkgName = "000";
    const char *networkId = "1234";
    int32_t retCode = 0;
    int32_t ret;

    ret = ClientOnLeaveMetaNodeResult(nullptr, networkId, retCode);
    EXPECT_EQ(ret, SOFTBUS_ERR);
    ret = ClientOnLeaveMetaNodeResult(pkgName, networkId, retCode);
    EXPECT_EQ(ret, SOFTBUS_ERR);
}
} // namespace OHOS
