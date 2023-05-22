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

#include "lnn_auth_mock.h"
#include "lnn_net_ledger_mock.h"
#include "lnn_network_manager.h"
#include "lnn_network_manager.c"
#include "lnn_network_manager_mock.h"
#include "lnn_physical_subnet_manager.h"
#include "lnn_trans_mock.h"
#include "message_handler.h"
#include "softbus_adapter_mem.h"
#include "softbus_common.h"
#include "softbus_conn_interface.h"
#include "softbus_errcode.h"
#include "softbus_log.h"
#include "softbus_protocol_def.h"

namespace OHOS {
using namespace testing::ext;
using namespace testing;

LnnNetIfMgr g_netIfMgr1 = {
    .type = LNN_NETIF_TYPE_ETH,
    .ifName = "ETH",
};
LnnNetIfMgr g_netIfMgr2 = {
    .type = LNN_NETIF_TYPE_WLAN,
    .ifName = "WLAN",
};
LnnNetIfMgr g_netIfMgr3 = {
    .type = LNN_NETIF_TYPE_BR,
    .ifName = "BR",
};
LnnNetIfMgr g_netIfMgr4 = {
    .type = LNN_NETIF_TYPE_BLE,
    .ifName = "BLE",
};
class LNNNetworkManagerMockTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void LNNNetworkManagerMockTest::SetUpTestCase()
{
    ListTailInsert(&g_netIfNameList, &g_netIfMgr1.node);
    ListTailInsert(&g_netIfNameList, &g_netIfMgr2.node);
    ListTailInsert(&g_netIfNameList, &g_netIfMgr3.node);
    ListTailInsert(&g_netIfNameList, &g_netIfMgr4.node);
}

void LNNNetworkManagerMockTest::TearDownTestCase()
{
}

void LNNNetworkManagerMockTest::SetUp()
{
}

void LNNNetworkManagerMockTest::TearDown()
{
}

int32_t LnnInitBtProtocolOk(LnnProtocolManager *self)
{
    (void)self;
    return SOFTBUS_OK;
}

int32_t LnnInitBtProtocolErr(LnnProtocolManager *self)
{
    (void)self;
    return SOFTBUS_ERR;
}

int32_t LnnEnableBtProtocol(LnnProtocolManager *self, LnnNetIfMgr *netifMgr)
{
    (void)self;
    (void)netifMgr;
    return SOFTBUS_OK;
}

static ListenerModule LnnGetBtListenerModule(ListenerMode mode)
{
    (void)mode;
    return UNUSE_BUTT;
}

/*
* @tc.name: LNN_NETWORK_MANAGER_TEST_001
* @tc.desc: len is not CONNECTION_ADDR_MAX return SOFTBUS_INVALID_PARAM
* @tc.type: FUNC
* @tc.require: I5OMIK
*/
HWTEST_F(LNNNetworkManagerMockTest, LNN_NETWORK_MANAGER_TEST_001, TestSize.Level1)
{
    NiceMock<LnnNetworkManagerInterfaceMock> managerMock;
    NiceMock<LnnAuthtInterfaceMock> authMock;
    NiceMock<LnnNetLedgertInterfaceMock> ledgerMock;
    EXPECT_CALL(managerMock, SoftbusGetConfig).WillRepeatedly(Return(SOFTBUS_ERR));
    EXPECT_CALL(managerMock, RegistIPProtocolManager).WillOnce(Return(SOFTBUS_ERR)).
        WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(authMock, RegGroupChangeListener).WillOnce(Return(SOFTBUS_ERR)).
        WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(managerMock, LnnInitPhysicalSubnetManager).WillOnce(Return(SOFTBUS_ERR)).
        WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(ledgerMock, LnnSetLocalNum64Info).WillOnce(Return(SOFTBUS_ERR)).
        WillRepeatedly(Return(SOFTBUS_OK));
    int ret = LnnInitNetworkManager();
    EXPECT_TRUE(ret == SOFTBUS_ERR);
    ret = LnnInitNetworkManager();
    EXPECT_TRUE(ret == SOFTBUS_ERR);
    ret = LnnInitNetworkManager();
    EXPECT_TRUE(ret == SOFTBUS_ERR);
    ret = LnnInitNetworkManager();
    EXPECT_TRUE(ret == SOFTBUS_ERR);
    ret = LnnInitNetworkManager();
    EXPECT_TRUE(ret == SOFTBUS_OK);
}

/*
* @tc.name: LNN_NETWORK_MANAGER_TEST_002
* @tc.desc: len is not CONNECTION_ADDR_MAX return SOFTBUS_INVALID_PARAM
* @tc.type: FUNC
* @tc.require: I5OMIK
*/
HWTEST_F(LNNNetworkManagerMockTest, LNN_NETWORK_MANAGER_TEST_002, TestSize.Level1)
{
    NiceMock<LnnNetworkManagerInterfaceMock> managerMock;
    EXPECT_CALL(managerMock, SoftbusGetConfig).WillRepeatedly(Return(SOFTBUS_OK));
    int32_t ret = LnnGetAddrTypeByIfName(nullptr, nullptr);
    EXPECT_TRUE(ret == SOFTBUS_ERR);

    ConnectionAddrType type1 = CONNECTION_ADDR_ETH;
    ret = LnnGetAddrTypeByIfName("unknown", &type1);
    EXPECT_TRUE(ret == SOFTBUS_ERR);

    ret = LnnGetAddrTypeByIfName("ETH", &type1);
    EXPECT_TRUE(ret == SOFTBUS_OK);

    ConnectionAddrType type2 = CONNECTION_ADDR_WLAN;
    ret = LnnGetAddrTypeByIfName("WLAN", &type2);
    EXPECT_TRUE(ret == SOFTBUS_OK);

    ConnectionAddrType type3 = CONNECTION_ADDR_BR;
    ret = LnnGetAddrTypeByIfName("BR", &type3);
    EXPECT_TRUE(ret == SOFTBUS_OK);

    ConnectionAddrType type4 = CONNECTION_ADDR_BLE;
    ret = LnnGetAddrTypeByIfName("BLE", &type4);
    EXPECT_TRUE(ret == SOFTBUS_OK);

    ret =LnnGetNetIfTypeByName(nullptr, nullptr);
    EXPECT_TRUE(ret == SOFTBUS_ERR);
}

/*
* @tc.name: LNN_NETWORK_MANAGER_TEST_003
* @tc.desc: len is not CONNECTION_ADDR_MAX return SOFTBUS_INVALID_PARAM
* @tc.type: FUNC
* @tc.require: I5OMIK
*/
HWTEST_F(LNNNetworkManagerMockTest, LNN_NETWORK_MANAGER_TEST_003, TestSize.Level1)
{
    NiceMock<LnnNetworkManagerInterfaceMock> managerMock;
    EXPECT_CALL(managerMock, SoftbusGetConfig).WillRepeatedly(Return(SOFTBUS_OK));
    int ret = LnnRegistProtocol(nullptr);
    EXPECT_TRUE(ret == SOFTBUS_ERR);

    LnnProtocolManager protocolMgr;
    memset_s(&protocolMgr, sizeof(protocolMgr), 0, sizeof(protocolMgr));
    protocolMgr.init = LnnInitBtProtocolErr;
    protocolMgr.enable = LnnEnableBtProtocol;
    protocolMgr.getListenerModule = LnnGetBtListenerModule;

    ret = LnnRegistProtocol(&protocolMgr);
    EXPECT_TRUE(ret == SOFTBUS_ERR);

    protocolMgr.init = LnnInitBtProtocolOk;
    ret = LnnRegistProtocol(&protocolMgr);
    EXPECT_TRUE(ret == SOFTBUS_OK);

    ret = UnregistProtocol(nullptr);
    EXPECT_TRUE(ret == SOFTBUS_ERR);

    ret = UnregistProtocol(&protocolMgr);
    EXPECT_TRUE(ret == SOFTBUS_OK);

    ProtocolType protocol = LNN_PROTOCOL_BR;

    ListenerModule listenerModule =  LnnGetProtocolListenerModule(protocol, LNN_LISTENER_MODE_DIRECT);
    EXPECT_TRUE(listenerModule == UNUSE_BUTT);

    FindProtocolByTypeRequest data = {
        .protocol = LNN_PROTOCOL_BR,
    };
    LnnProtocolManager manager = {
        .id = LNN_PROTOCOL_BR,
    };
    VisitNextChoice visitNextChoice = FindProtocolByType(&manager, (void *)&data);
    EXPECT_TRUE(visitNextChoice == CHOICE_FINISH_VISITING);
    manager.id = LNN_PROTOCOL_BLE;
    visitNextChoice = FindProtocolByType(&manager, (void *)&data);
    EXPECT_TRUE(visitNextChoice == CHOICE_VISIT_NEXT);

    NiceMock<LnnNetLedgertInterfaceMock> ledgerMock;
    EXPECT_CALL(ledgerMock, LnnGetLocalStrInfo).WillOnce(Return(SOFTBUS_ERR)).WillRepeatedly(Return(SOFTBUS_OK));
    ret = LnnInitNetworkManagerDelay();
    EXPECT_TRUE(ret == SOFTBUS_ERR);
    ret = LnnInitNetworkManagerDelay();
    EXPECT_TRUE(ret == SOFTBUS_OK);
}

/*
* @tc.name: LNN_NETWORK_MANAGER_TEST_004
* @tc.desc: len is not CONNECTION_ADDR_MAX return SOFTBUS_INVALID_PARAM
* @tc.type: FUNC
* @tc.require: I5OMIK
*/
HWTEST_F(LNNNetworkManagerMockTest, LNN_NETWORK_MANAGER_TEST_004, TestSize.Level1)
{
    int len = 0;
    char buf[] = "nullptr";
    NiceMock<LnnNetworkManagerInterfaceMock> managerMock;
    EXPECT_CALL(managerMock, SoftbusGetConfig).WillRepeatedly(Return(SOFTBUS_ERR));
    bool ret = LnnIsAutoNetWorkingEnabled();
    EXPECT_TRUE(ret == true);
    EXPECT_CALL(managerMock, SoftbusGetConfig).WillRepeatedly(Return(SOFTBUS_OK));
    ret = LnnIsAutoNetWorkingEnabled();
    EXPECT_TRUE(ret == false);

    EXPECT_CALL(managerMock, SoftbusGetConfig).WillRepeatedly(Return(SOFTBUS_OK));
    int res = LnnInitManagerByConfig();
    EXPECT_TRUE(res == SOFTBUS_ERR);

    EXPECT_CALL(managerMock, SoftbusGetConfig).WillRepeatedly(Return(SOFTBUS_ERR));
    res = LnnInitManagerByConfig();
    EXPECT_TRUE(res == SOFTBUS_OK);

    res = ParseIfNameConfig(nullptr, len);
    EXPECT_TRUE(res == SOFTBUS_ERR);
    res = ParseIfNameConfig(buf, sizeof("nullptr"));
    EXPECT_TRUE(res == SOFTBUS_OK);

    res = SetIfNameDefaultVal();
    EXPECT_TRUE(res == SOFTBUS_OK);
}
}
