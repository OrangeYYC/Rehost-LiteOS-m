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
#include <cstring>
#include <gtest/gtest.h>
#include <securec.h>

#include "softbus_common.h"
#include "client_bus_center_manager.c"

namespace OHOS {
using namespace testing::ext;


class ClientBusMangerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void ClientBusMangerTest::SetUpTestCase()
{
}

void ClientBusMangerTest::TearDownTestCase()
{
}

void ClientBusMangerTest::SetUp()
{
    BusCenterClientInit();
}

void ClientBusMangerTest::TearDown()
{
}

/*
* @tc.name: IS_SAME_CONNECTION_ADDR
* @tc.desc: PreLink lane test
* @tc.type: FUNC
* @tc.require: AR000FN5VC
*/
HWTEST_F(ClientBusMangerTest, IS_SAME_CONNECTION_ADDR_Test_001, TestSize.Level1)
{
    const ConnectionAddr addr1 = {
        .type = CONNECTION_ADDR_MAX,
        .info.session.sessionId = 1,
    };
    const ConnectionAddr addr2 = {
        .type = CONNECTION_ADDR_SESSION,
        .info.session.sessionId = 2,
    };
    int32_t ret = IsSameConnectionAddr(&addr1, &addr2);
    EXPECT_TRUE(ret == false);
    const ConnectionAddr addr3 = {
        .type = CONNECTION_ADDR_SESSION,
        .info.session.sessionId = 1,
    };
    ret = IsSameConnectionAddr(&addr3, &addr2);
    EXPECT_TRUE(ret == false);
}

/*

* @tc.name: JoinMetaNodeInner
* @tc.desc: JoinMetaNode test
* @tc.type: FUNC
* @tc.require: AR000FN5VC
*/
HWTEST_F(ClientBusMangerTest, JOIN_METANODE_Test_001, TestSize.Level1)
{
    g_busCenterClient.isInit = false;
    char pkgName[] = "test";
    ConnectionAddr connAddr;
    (void)memset_s(&connAddr, sizeof(ConnectionAddr), 0, sizeof(ConnectionAddr));
    CustomData customData;
    (void)memset_s(&customData, sizeof(CustomData), 0, sizeof(CustomData));
    OnJoinLNNResult cb = nullptr;
    int32_t ret = JoinMetaNodeInner(pkgName, &connAddr, &customData, cb);
    EXPECT_TRUE(ret == SOFTBUS_NO_INIT);
    g_busCenterClient.isInit = true;
    ret = JoinMetaNodeInner(pkgName, &connAddr, &customData, cb);
    EXPECT_TRUE(ret == SOFTBUS_IPC_ERR);
}

/*
* @tc.name: LEAVE_META_NODE_INNER
* @tc.desc: LEAVE_META_NODE_INNER test
* @tc.type: FUNC
* @tc.require: AR000FN5VC
*/
HWTEST_F(ClientBusMangerTest, LEAVE_META_NODE_INNER_Test_001, TestSize.Level1)
{
    char pkgName[] = "test"; 
    char networkId[] = "222";
    OnLeaveMetaNodeResult cb = nullptr;
    g_busCenterClient.isInit = false;
    int32_t ret = LeaveMetaNodeInner(pkgName, networkId, cb);
    EXPECT_TRUE(ret == SOFTBUS_NO_INIT);
    g_busCenterClient.isInit = true;
    ret = LeaveMetaNodeInner(pkgName, networkId, cb);
    EXPECT_TRUE(ret == SOFTBUS_IPC_ERR);
}

/*
* @tc.name: LEAVE_META_NODE_INNER
* @tc.desc: LEAVE_META_NODE_INNER test
* @tc.type: FUNC
* @tc.require: AR000FN5VC
*/
HWTEST_F(ClientBusMangerTest, META_NODE_ON_JOIN_Test_001, TestSize.Level1)
{
    g_busCenterClient.isInit = false;
    void *addr = nullptr;
    char networkId[] = "3333";
    int32_t retCod = 111;
    int32_t ret = MetaNodeOnJoinResult(addr, networkId, retCod);
    EXPECT_TRUE(ret == SOFTBUS_INVALID_PARAM);
    g_busCenterClient.isInit = true;
    ConnectionAddr connAddr;
    (void)memset_s(&connAddr, sizeof(ConnectionAddr), 0, sizeof(ConnectionAddr));
    addr = (void*)&connAddr;
    ret = MetaNodeOnJoinResult(addr, networkId, retCod);
    EXPECT_TRUE(ret == SOFTBUS_OK);
}

/*
* @tc.name: META_NODE_ON_LEAVE
* @tc.desc: LEAVE_META_NODE_INNER test
* @tc.type: FUNC
* @tc.require: AR000FN5VC
*/
HWTEST_F(ClientBusMangerTest, META_NODE_ON_LEAVE_Test_001, TestSize.Level1)
{
    g_busCenterClient.isInit = false;
    char networkId[] = "3333";
    int32_t retCod = 111;
    int32_t ret = MetaNodeOnLeaveResult(networkId, retCod);
    EXPECT_TRUE(ret == SOFTBUS_ERR);
    g_busCenterClient.isInit = true;
    ret = MetaNodeOnLeaveResult(networkId, retCod);
    EXPECT_TRUE(ret == SOFTBUS_OK);
}
}