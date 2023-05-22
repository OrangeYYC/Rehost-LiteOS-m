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

#include "lnn_local_net_ledger.h"
#include "lnn_net_builder.h"
#include "lnn_net_builder.c"
#include "lnn_net_builder_deps_mock.h"
#include "softbus_common.h"
#include "softbus_errcode.h"
#include "softbus_log.h"

namespace OHOS {
using namespace testing::ext;
constexpr int32_t LOCAL_WEIGHT = 10;
constexpr char NODE_UDID[] = "123456ABCDEF";
constexpr char NODE_NETWORK_ID[] = "235689BNHFCF";
constexpr char NODE1_NETWORK_ID[] = "345678BNHFCF";
constexpr uint8_t MSG[] = "123456BNHFCF";
constexpr int64_t AUTH_META_ID = 1;
constexpr char INVALID_UDID[] =
    "ASDFGHJKLPASDFGHJKLPASDFGHJKLPASDFGHJKLPASDFGHJKLPASDFGHJKLPASDFGHJKLPASDFGHJK\
    LPASDFGHJKLPASDFGHJKLPASDFGHJKLPASDFGHJKLPASDFGHJKLPASDFGHJKLPASDFGHJKLPASDFGHJ\
    KLPASDFGHJKLPASDFGHJKLPASDFGHJKLPASDFGHJKLP";
constexpr uint32_t ADDR_LEN1 = 15;
constexpr uint32_t ADDR_LEN2 = 12;
constexpr int64_t AUTH_ID = 10;
constexpr int64_t AUTH_ID_ADD = 11;
constexpr char NODE1_BR_MAC[] = "12345TTU";
constexpr char NODE2_BR_MAC[] = "56789TTU";
constexpr uint32_t REQUEST_ID = 1;
constexpr uint32_t REQUEST_ID_ADD = 2;
constexpr uint16_t FSM_ID = 1;
constexpr uint16_t FSM_ID_ADD = 2;
constexpr char NODE1_IP[] = "10.146.181.134";
constexpr char NODE2_IP[] = "10.147.182.135";
constexpr int32_t CONN_COUNT = 10;
constexpr int32_t CURRENT_COUNT = 11;
constexpr uint32_t CONN_FLAG1 = 128;
constexpr uint32_t CONN_FLAG2 = 255;
using namespace testing;
class LNNNetBuilderMockTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void LNNNetBuilderMockTest::SetUpTestCase() {}

void LNNNetBuilderMockTest::TearDownTestCase() {}

void LNNNetBuilderMockTest::SetUp() {}

void LNNNetBuilderMockTest::TearDown() {}

/*
* @tc.name: LNN_INIT_NET_BUILDER_TEST_001
* @tc.desc: lnn init netbuilder test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, LNN_INIT_NET_BUILDER_TEST_001, TestSize.Level1)
{
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnInitSyncInfoManager())
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnInitTopoManager()).WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, SoftbusGetConfig(_, _, _)).WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, RegAuthVerifyListener(_))
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnRegSyncInfoHandler(_, _))
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnGenLocalNetworkId(_, _))
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnGenLocalUuid(_, _)).WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnSetLocalStrInfo(_, _)).WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_TRUE(LnnInitNetBuilder() == SOFTBUS_ERR);
    EXPECT_TRUE(LnnInitNetBuilder() == SOFTBUS_ERR);
    EXPECT_TRUE(LnnInitNetBuilder() == SOFTBUS_ERR);
    EXPECT_TRUE(LnnInitNetBuilder() == SOFTBUS_ERR);
    EXPECT_TRUE(LnnInitNetBuilder() == SOFTBUS_ERR);
}

/*
* @tc.name: CONFIG_LOCAL_LEDGER_TEST_001
* @tc.desc: config local ledger test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, CONFIG_LOCAL_LEDGER_TEST_001, TestSize.Level1)
{
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnGenLocalNetworkId(_, _))
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnGenLocalUuid(_, _))
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnSetLocalStrInfo(_, _)).WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_TRUE(ConifgLocalLedger() == SOFTBUS_ERR);
    EXPECT_TRUE(ConifgLocalLedger() == SOFTBUS_ERR);
    EXPECT_TRUE(ConifgLocalLedger() == SOFTBUS_OK);
}

/*
* @tc.name: LNN_FILL_NODE_INFO_TEST_001
* @tc.desc: test FillNodeInfo
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, LNN_FILL_NODE_INFO_TEST_001, TestSize.Level1)
{
    NiceMock<NetBuilderDepsInterfaceMock> uuidMock;
    EXPECT_CALL(uuidMock, AuthGetDeviceUuid(_,_,_)).WillRepeatedly(Return(SOFTBUS_OK));
    MetaJoinRequestNode metaNode;
    NodeInfo info;
    info.uuid[0] = 'x';
    metaNode.addr.type = CONNECTION_ADDR_ETH;
    EXPECT_TRUE(FillNodeInfo(&metaNode, &info) == SOFTBUS_OK);
    EXPECT_CALL(uuidMock, AuthGetDeviceUuid(_,_,_)).WillRepeatedly(Return(SOFTBUS_OK));
    metaNode.addr.type = CONNECTION_ADDR_WLAN;
    EXPECT_TRUE(FillNodeInfo(&metaNode, &info) == SOFTBUS_OK);
    EXPECT_CALL(uuidMock, AuthGetDeviceUuid(_,_,_)).WillRepeatedly(Return(SOFTBUS_OK));
    metaNode.addr.type = CONNECTION_ADDR_BR;
    EXPECT_TRUE(FillNodeInfo(&metaNode, &info) == SOFTBUS_OK);
}

/*
* @tc.name: LNN_INIT_NET_BUILDER_DELAY_TEST_001
* @tc.desc: lnn init netbuilder delay test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, LNN_INIT_NET_BUILDER_DELAY_TEST_001, TestSize.Level1)
{
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnSetLocalStrInfo(_, _)).WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnGetLocalWeight()).WillRepeatedly(Return(LOCAL_WEIGHT));
    EXPECT_CALL(NetBuilderMock, LnnSetLocalNumInfo(_, _)).WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnInitFastOffline())
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_TRUE(LnnInitNetBuilderDelay() == SOFTBUS_ERR);
    EXPECT_TRUE(LnnInitNetBuilderDelay() == SOFTBUS_OK);
    EXPECT_TRUE(LnnInitNetBuilderDelay() == SOFTBUS_OK);
}

/*
* @tc.name: LNN_LEAVE_META_TO_LEDGER_TEST_001
* @tc.desc: leave meta info to ledger
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, LNN_LEAVE_META_TO_LEDGER_TEST_001, TestSize.Level1)
{
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    MetaJoinRequestNode metaInfo;
    NodeInfo nodeInfo;
    EXPECT_CALL(NetBuilderMock, LnnGetNodeInfoById(_, _))
        .WillOnce(Return(nullptr))
        .WillRepeatedly(Return(&nodeInfo));
    EXPECT_CALL(NetBuilderMock, LnnGetDeviceUdid(_)).WillRepeatedly(Return(NODE_UDID));
    EXPECT_CALL(NetBuilderMock, LnnDeleteMetaInfo(_, _))
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    LeaveMetaInfoToLedger(&metaInfo, nullptr);
    LeaveMetaInfoToLedger(&metaInfo, nullptr);
    LeaveMetaInfoToLedger(&metaInfo, nullptr);
}

/*
* @tc.name: POST_JOIN_REQUEST_TO_META_NODE_TEST_001
* @tc.desc: post join request to meta node test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, POST_JOIN_REQUEST_TO_META_NODE_TEST_001, TestSize.Level1)
{
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    MetaJoinRequestNode metaJoinNode;
    CustomData customData;
    EXPECT_CALL(NetBuilderMock, OnJoinMetaNode(_, _))
        .WillOnce(Return(SOFTBUS_OK))
        .WillRepeatedly(Return(SOFTBUS_ERR));
    EXPECT_CALL(NetBuilderMock, MetaNodeNotifyJoinResult(_, _, _)).WillRepeatedly(Return());
    EXPECT_TRUE(PostJoinRequestToMetaNode(&metaJoinNode, nullptr, nullptr, true) == SOFTBUS_OK);
    EXPECT_TRUE(PostJoinRequestToMetaNode(&metaJoinNode, nullptr, &customData, true) == SOFTBUS_ERR);
    EXPECT_TRUE(PostJoinRequestToMetaNode(&metaJoinNode, nullptr, &customData, false) == SOFTBUS_ERR);
}

/*
* @tc.name: LNN_NOTIFY_AUTH_HANDLE_LEAVE_LNN_TEST_001
* @tc.desc: lnn notify auth handle leave lnn test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, LNN_NOTIFY_AUTH_HANDLE_LEAVE_LNN_TEST_001, TestSize.Level1)
{
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, AuthHandleLeaveLNN(_)).WillRepeatedly(Return());
    g_netBuilder.isInit = true;
    int64_t authId = AUTH_ID;
    EXPECT_TRUE(LnnNotifyAuthHandleLeaveLNN(authId) == SOFTBUS_OK);
}

/*
* @tc.name: LNN_UPDATE_NODE_ADDR_TEST_001
* @tc.desc: lnn update node addr test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, LNN_UPDATE_NODE_ADDR_TEST_001, TestSize.Level1)
{
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnGetLocalStrInfo(_, _, _))
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnSetLocalStrInfo(_, _))
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnGetAllOnlineNodeInfo(_, _))
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnNotifyNodeAddressChanged(_)).WillRepeatedly(Return());
    EXPECT_TRUE(LnnUpdateNodeAddr(nullptr) == SOFTBUS_INVALID_PARAM);
    EXPECT_TRUE(LnnUpdateNodeAddr(NODE_NETWORK_ID) == SOFTBUS_ERR);
    EXPECT_TRUE(LnnUpdateNodeAddr(NODE_NETWORK_ID) == SOFTBUS_OK);
    EXPECT_TRUE(LnnUpdateNodeAddr(NODE_NETWORK_ID) == SOFTBUS_OK);
    EXPECT_TRUE(LnnUpdateNodeAddr(NODE_NETWORK_ID) == SOFTBUS_OK);
}

/*
* @tc.name: NODE_INFO_SYNC_TEST_001
* @tc.desc: node info sync test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, NODE_INFO_SYNC_TEST_001, TestSize.Level1)
{
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnInitP2p())
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnInitNetworkInfo())
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnInitDevicename())
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnInitOffline())
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_TRUE(NodeInfoSync() == SOFTBUS_ERR);
    EXPECT_TRUE(NodeInfoSync() == SOFTBUS_ERR);
    EXPECT_TRUE(NodeInfoSync() == SOFTBUS_ERR);
    EXPECT_TRUE(NodeInfoSync() == SOFTBUS_ERR);
    EXPECT_TRUE(NodeInfoSync() == SOFTBUS_OK);
}

/*
* @tc.name: ON_RECEIVER_NODE_ADDR_CHANGED_MSG_TEST_001
* @tc.desc: on receive node addr changed msg test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, ON_RECEIVER_NODE_ADDR_CHANGED_MSG_TEST_001, TestSize.Level1)
{
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    OnReceiveNodeAddrChangedMsg(LNN_INFO_TYPE_NODE_ADDR, nullptr, MSG, ADDR_LEN1);
    OnReceiveNodeAddrChangedMsg(LNN_INFO_TYPE_NODE_ADDR, nullptr, MSG, ADDR_LEN2);
    OnReceiveNodeAddrChangedMsg(LNN_INFO_TYPE_NODE_ADDR, nullptr, MSG, ADDR_LEN2);
}

/*
* @tc.name: ON_DEVICE_NOT_TRUSTED_TEST_001
* @tc.desc: on device not trusted test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, ON_DEVICE_NOT_TRUSTED_TEST_001, TestSize.Level1)
{
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    SoftBusLooper loop;
    EXPECT_CALL(NetBuilderMock, LnnGetOnlineStateById(_, _))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(NetBuilderMock, LnnGetAllAuthSeq(_, _, _))
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnSendNotTrustedInfo(_, _))
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, GetLooper(_)).WillRepeatedly(Return(&loop));
    EXPECT_CALL(NetBuilderMock, LnnAsyncCallbackDelayHelper(_, _, _, _))
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    OnDeviceNotTrusted(nullptr);
    OnDeviceNotTrusted(INVALID_UDID);
    OnDeviceNotTrusted(NODE_UDID);
    OnDeviceNotTrusted(NODE_UDID);
    OnDeviceNotTrusted(NODE_UDID);
    OnDeviceNotTrusted(NODE_UDID);
    OnDeviceNotTrusted(NODE_UDID);
}

/*
* @tc.name: ON_DEVICE_VERIFY_PASS_TEST_001
* @tc.desc: on device verify pass test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, ON_DEVICE_VERIFY_PASS_TEST_001, TestSize.Level1)
{
    NodeInfo info;
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, AuthGetConnInfo(_, _))
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnGetLocalStrInfo(_, _, _)).WillRepeatedly(Return(SOFTBUS_ERR));
    EXPECT_CALL(NetBuilderMock, LnnConvertAuthConnInfoToAddr(_, _, _))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    OnDeviceVerifyPass(AUTH_META_ID, &info);
    OnDeviceVerifyPass(AUTH_META_ID, &info);
}

/*
* @tc.name: GET_CURRENT_CONNECT_TYPE_TEST_001
* @tc.desc: get current connect type test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, GET_CURRENT_CONNECT_TYPE_TEST_001, TestSize.Level1)
{
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnGetLocalStrInfo(_, _, _))
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnGetAddrTypeByIfName(_, _))
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_TRUE(GetCurrentConnectType() == CONNECTION_ADDR_MAX);
    EXPECT_TRUE(GetCurrentConnectType() == CONNECTION_ADDR_MAX);
    EXPECT_TRUE(GetCurrentConnectType() == CONNECTION_ADDR_MAX);
}

/*
* @tc.name: NET_BUILDER_MESSAGE_HANDLER_TEST_001
* @tc.desc: net builder message handler test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, NET_BUILDER_MESSAGE_HANDLER_TEST_001, TestSize.Level1)
{
    SoftBusMessage msg;
    msg.what = MSG_TYPE_MAX;
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, SoftbusGetConfig(_, _, _)).WillRepeatedly(Return(SOFTBUS_OK));
    NetBuilderMessageHandler(nullptr);
    NetBuilderMessageHandler(&msg);
}

/*
* @tc.name: PROCESS_ON_AUTH_META_VERIFY_TEST_001
* @tc.desc: process on auth meta verify failed and passed test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_ON_AUTH_META_VERIFY_FAILED_TEST_001, TestSize.Level1)
{
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, MetaNodeNotifyJoinResult(_, _, _)).WillRepeatedly(Return());
    EXPECT_TRUE(ProcessOnAuthMetaVerifyFailed(nullptr) == SOFTBUS_ERR);
    EXPECT_TRUE(ProcessOnAuthMetaVerifyPassed(nullptr) == SOFTBUS_ERR);
}

/*
* @tc.name: PROCESS_LEAVE_SPECIFIC_TEST_001
* @tc.desc: process leave specific test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_LEAVE_SPECIFIC_TEST_001, TestSize.Level1)
{
    void *para = reinterpret_cast<void *>(SoftBusMalloc(sizeof(SpecificLeaveMsgPara)));
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnSendLeaveRequestToConnFsm(_)).WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_TRUE(ProcessLeaveSpecific(nullptr) == SOFTBUS_INVALID_PARAM);
    EXPECT_TRUE(ProcessLeaveSpecific(para) == SOFTBUS_OK);
}

/*
* @tc.name: PROCESS_LEAVE_BY_ADDR_TYPE_TEST_001
* @tc.desc: process leave by addr type test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_LEAVE_BY_ADDR_TYPE_TEST_001, TestSize.Level1)
{
    void *para = reinterpret_cast<void *>(SoftBusMalloc(sizeof(bool)));
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnSendLeaveRequestToConnFsm(_)).WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnNotifyAllTypeOffline(_)).WillRepeatedly(Return());
    EXPECT_TRUE(ProcessLeaveByAddrType(nullptr) == SOFTBUS_INVALID_PARAM);
    EXPECT_TRUE(ProcessLeaveByAddrType(para) == SOFTBUS_OK);
}

/*
* @tc.name: PROCESS_ELETE_TEST_001
* @tc.desc: process elect test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_ELETE_TEST_001, TestSize.Level1)
{
    void *para = reinterpret_cast<void *>(SoftBusMalloc(sizeof(ElectMsgPara)));
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, SoftbusGetConfig(_, _, _)).WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_TRUE(ProcessMasterElect(nullptr) == SOFTBUS_INVALID_PARAM);
    EXPECT_TRUE(ProcessMasterElect(para) == SOFTBUS_ERR);
}

/*
* @tc.name: PROCESS_NODE_STATE_CHANGED_TEST_001
* @tc.desc: process node state changed test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_NODE_STATE_CHANGED_TEST_001, TestSize.Level1)
{
    void *para = reinterpret_cast<void *>(SoftBusMalloc(sizeof(ConnectionAddr)));
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, SoftbusGetConfig(_, _, _)).WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_TRUE(ProcessNodeStateChanged(nullptr) == SOFTBUS_INVALID_PARAM);
    EXPECT_TRUE(ProcessNodeStateChanged(para) == SOFTBUS_ERR);
}

/*
* @tc.name: TRY_ELECT_NODE_OFFLINE_TEST_001
* @tc.desc: try elect node offline test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, TRY_ELECT_NODE_OFFLINE_TEST_001, TestSize.Level1)
{
    LnnConnectionFsm connFsm;
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnGetLocalStrInfo(_, _, _))
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_TRUE(TryElectMasterNodeOffline(&connFsm) == SOFTBUS_ERR);
    EXPECT_TRUE(TryElectMasterNodeOffline(&connFsm) == SOFTBUS_OK);
}

/*
* @tc.name: TRY_ELECT_NODE_ONLINE_TEST_001
* @tc.desc: try elect node online test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, TRY_ELECT_NODE_ONLINE_TEST_001, TestSize.Level1)
{
    LnnConnectionFsm connFsm;
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnGetLocalStrInfo(_, _, _))
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnGetLocalNumInfo(_, _))
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnGetRemoteStrInfo(_, _, _, _))
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnGetRemoteNumInfo(_, _, _))
        .WillOnce(Return(SOFTBUS_ERR))
        .WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnCompareNodeWeight(_, _, _, _)).WillRepeatedly(Return(0));
    EXPECT_TRUE(TryElectMasterNodeOnline(&connFsm) == SOFTBUS_ERR);
    EXPECT_TRUE(TryElectMasterNodeOnline(&connFsm) == SOFTBUS_ERR);
    EXPECT_TRUE(TryElectMasterNodeOnline(&connFsm) == SOFTBUS_ERR);
    EXPECT_TRUE(TryElectMasterNodeOnline(&connFsm) == SOFTBUS_ERR);
    EXPECT_TRUE(TryElectMasterNodeOnline(&connFsm) == SOFTBUS_OK);
}

/*
* @tc.name: PROCESS_LEAVE_INVALID_CONN_TEST_001
* @tc.desc: process leave invalid conn test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_LEAVE_INVALID_CONN_TEST_001, TestSize.Level1)
{
    void *para = reinterpret_cast<void *>(SoftBusMalloc(sizeof(LeaveInvalidConnMsgPara)));
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, SoftbusGetConfig(_, _, _)).WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_TRUE(ProcessLeaveInvalidConn(nullptr) == SOFTBUS_INVALID_PARAM);
    EXPECT_TRUE(ProcessLeaveInvalidConn(para) == SOFTBUS_OK);
}

/*
* @tc.name: IS_INVALID_CONNECTION_FSM_TEST_001
* @tc.desc: is invalid connection fsm test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, IS_INVALID_CONNECTION_FSM_TEST_001, TestSize.Level1)
{
    LnnConnectionFsm connFsm;
    (void)memset_s(&connFsm, sizeof(LnnConnectionFsm), 0, sizeof(LnnConnectionFsm));
    (void)strncpy_s(connFsm.connInfo.peerNetworkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID, strlen(NODE_NETWORK_ID));
    LeaveInvalidConnMsgPara msgPara;
    (void)memset_s(&msgPara, sizeof(LeaveInvalidConnMsgPara), 0, sizeof(LeaveInvalidConnMsgPara));
    (void)strncpy_s(msgPara.oldNetworkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID, strlen(NODE1_NETWORK_ID));
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, SoftbusGetConfig(_, _, _)).WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_TRUE(IsInvalidConnectionFsm(&connFsm, &msgPara) == false);
    (void)memset_s(&msgPara, sizeof(LeaveInvalidConnMsgPara), 0, sizeof(LeaveInvalidConnMsgPara));
    (void)strncpy_s(msgPara.oldNetworkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID, strlen(NODE_NETWORK_ID));
    connFsm.isDead = true;
    EXPECT_TRUE(IsInvalidConnectionFsm(&connFsm, &msgPara) == false);
    connFsm.isDead = false;
    msgPara.addrType = CONNECTION_ADDR_WLAN;
    connFsm.connInfo.addr.type = CONNECTION_ADDR_BR;
    EXPECT_TRUE(IsInvalidConnectionFsm(&connFsm, &msgPara) == false);
    msgPara.addrType = CONNECTION_ADDR_MAX;
    connFsm.connInfo.flag = 0;
    EXPECT_TRUE(IsInvalidConnectionFsm(&connFsm, &msgPara) == false);
    connFsm.connInfo.flag = 1;
    EXPECT_TRUE(IsInvalidConnectionFsm(&connFsm, &msgPara) == false);
}

/*
* @tc.name: PROCESS_SYNC_OFFLINE_FINISH_TEST_001
* @tc.desc: process sync offline finish test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_SYNC_OFFLINE_FINISH_TEST_001, TestSize.Level1)
{
    void *para = reinterpret_cast<void *>(SoftBusMalloc(sizeof(char) * NETWORK_ID_BUF_LEN));
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, SoftbusGetConfig(_, _, _)).WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_TRUE(ProcessSyncOfflineFinish(nullptr) == SOFTBUS_INVALID_PARAM);
    EXPECT_TRUE(ProcessSyncOfflineFinish(para) == SOFTBUS_OK);
}

/*
* @tc.name: PROCESS_LEAVE_META_NODE_REQUEST_TEST_001
* @tc.desc: process leave meta node request test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_LEAVE_META_NODE_REQUEST_TEST_001, TestSize.Level1)
{
    void *para = reinterpret_cast<void *>(SoftBusMalloc(sizeof(char) * NETWORK_ID_BUF_LEN));
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, MetaNodeNotifyLeaveResult(_, _)).WillRepeatedly(Return());
    EXPECT_TRUE(ProcessLeaveMetaNodeRequest(nullptr) == SOFTBUS_INVALID_PARAM);
    EXPECT_TRUE(ProcessLeaveMetaNodeRequest(para) == SOFTBUS_ERR);
}

/*
* @tc.name: PROCESS_LEAVE_LNN_REQUEST_TEST_001
* @tc.desc: process leave lnn request test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_LEAVE_LNN_REQUEST_TEST_001, TestSize.Level1)
{
    void *para = reinterpret_cast<void *>(SoftBusMalloc(sizeof(char) * NETWORK_ID_BUF_LEN));
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnSendLeaveRequestToConnFsm(_)).WillRepeatedly(Return(SOFTBUS_ERR));
    EXPECT_CALL(NetBuilderMock, LnnNotifyLeaveResult(_, _)).WillRepeatedly(Return());
    EXPECT_TRUE(ProcessLeaveLNNRequest(nullptr) == SOFTBUS_INVALID_PARAM);
    EXPECT_TRUE(ProcessLeaveLNNRequest(para) == SOFTBUS_ERR);
}

/*
* @tc.name: PROCESS_DEVICE_NOT_TRUSTED_TEST_001
* @tc.desc: process device not trusted test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_DEVICE_NOT_TRUSTED_TEST_001, TestSize.Level1)
{
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnGetNetworkIdByUdid(_, _, _)).WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_TRUE(ProcessDeviceNotTrusted(nullptr) == SOFTBUS_INVALID_PARAM);
}

/*
* @tc.name: PROCESS_DEVICE_DISCONNECT_TEST_001
* @tc.desc: process device disconnect test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_DEVICE_DISCONNECT_TEST_001, TestSize.Level1)
{
    void *para = reinterpret_cast<void *>(SoftBusMalloc(sizeof(int64_t)));
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, SoftbusGetConfig(_, _, _)).WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_TRUE(ProcessDeviceDisconnect(nullptr) == SOFTBUS_INVALID_PARAM);
    EXPECT_TRUE(ProcessDeviceDisconnect(para) == SOFTBUS_ERR);
}

/*
* @tc.name: PROCESS_DEVICE_VERIFY_PASS_TEST_001
* @tc.desc: process device verify pass test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_DEVICE_VERIFY_PASS_TEST_001, TestSize.Level1)
{
    DeviceVerifyPassMsgPara *msgPara =
        reinterpret_cast<DeviceVerifyPassMsgPara *>(SoftBusMalloc(sizeof(DeviceVerifyPassMsgPara)));
    msgPara->nodeInfo = NULL;
    void *para = reinterpret_cast<void *>(msgPara);
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, SoftbusGetConfig(_, _, _)).WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_TRUE(ProcessDeviceVerifyPass(nullptr) == SOFTBUS_INVALID_PARAM);
    EXPECT_TRUE(ProcessDeviceVerifyPass(para) == SOFTBUS_INVALID_PARAM);
}

/*
* @tc.name: PROCESS_VERIFY_RESULT_TEST_001
* @tc.desc: process verify result test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_VERIFY_RESULT_TEST_001, TestSize.Level1)
{
    VerifyResultMsgPara *msgPara1 = reinterpret_cast<VerifyResultMsgPara *>(SoftBusMalloc(sizeof(VerifyResultMsgPara)));
    msgPara1->nodeInfo = NULL;
    void *para1 = reinterpret_cast<void *>(msgPara1);
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, SoftbusGetConfig(_, _, _)).WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_TRUE(ProcessVerifyResult(nullptr) == SOFTBUS_INVALID_PARAM);
    EXPECT_TRUE(ProcessVerifyResult(para1) == SOFTBUS_INVALID_PARAM);
    VerifyResultMsgPara *msgPara2 = reinterpret_cast<VerifyResultMsgPara *>(SoftBusMalloc(sizeof(VerifyResultMsgPara)));
    msgPara2->nodeInfo = reinterpret_cast<NodeInfo *>(SoftBusMalloc(sizeof(NodeInfo)));
    void *para2 = reinterpret_cast<void *>(msgPara2);
    EXPECT_TRUE(ProcessVerifyResult(para2) == SOFTBUS_ERR);
}

/*
* @tc.name: PROCESS_CLEAN_CONNECTION_FSM_TEST_001
* @tc.desc: process clean connection fsm test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_CLEAN_CONNECTION_FSM_TEST_001, TestSize.Level1)
{
    void *para = reinterpret_cast<void *>(SoftBusMalloc(sizeof(uint16_t)));
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, SoftbusGetConfig(_, _, _)).WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_TRUE(ProcessCleanConnectionFsm(nullptr) == SOFTBUS_INVALID_PARAM);
    EXPECT_TRUE(ProcessCleanConnectionFsm(para) == SOFTBUS_ERR);
}

/*
* @tc.name: IS_NODE_ONLINE_TEST_001
* @tc.desc: is node online test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, IS_NODE_ONLINE_TEST_001, TestSize.Level1)
{
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    NodeInfo Info;
    EXPECT_CALL(NetBuilderMock, LnnGetNodeInfoById(_, _)).WillOnce(Return(nullptr));
    bool ret = IsNodeOnline(NODE_NETWORK_ID);
    EXPECT_TRUE(ret == false);
    EXPECT_CALL(NetBuilderMock, LnnGetNodeInfoById(_, _)).WillOnce(Return(&Info));
    EXPECT_CALL(NetBuilderMock, LnnIsNodeOnline(_)).WillOnce(Return(false));
    ret = IsNodeOnline(NODE_NETWORK_ID);
    EXPECT_TRUE(ret == false);
    EXPECT_CALL(NetBuilderMock, LnnGetNodeInfoById(_, _)).WillOnce(Return(&Info));
    EXPECT_CALL(NetBuilderMock, LnnIsNodeOnline(_)).WillOnce(Return(true));
    ret = IsNodeOnline(NODE_NETWORK_ID);
    EXPECT_TRUE(ret == true);
}

/*
* @tc.name: UPDATE_LOCAL_NODE_TEST_001
* @tc.desc: update local node test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, UPDATE_LOCAL_NODE_TEST_001, TestSize.Level1)
{
    bool isCurrentNode = false;

    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnSetLocalStrInfo(_, _)).WillOnce(Return(SOFTBUS_ERR));
    UpdateLocalMasterNode(isCurrentNode, NODE_UDID, LOCAL_WEIGHT);

    EXPECT_CALL(NetBuilderMock, LnnSetLocalStrInfo(_, _)).WillOnce(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnSetLocalNumInfo(_, _)).WillOnce(Return(SOFTBUS_ERR));
    EXPECT_CALL(NetBuilderMock, LnnNotifyMasterNodeChanged(_, _, _)).WillOnce(Return());
    UpdateLocalMasterNode(isCurrentNode, NODE_UDID, LOCAL_WEIGHT);

    EXPECT_CALL(NetBuilderMock, LnnSetLocalStrInfo(_, _)).WillOnce(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnSetLocalNumInfo(_, _)).WillOnce(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnNotifyMasterNodeChanged(_, _, _)).WillOnce(Return());
    UpdateLocalMasterNode(isCurrentNode, NODE_UDID, LOCAL_WEIGHT);
}

/*
* @tc.name: DUP_NODE_INFO_TEST_001
* @tc.desc: dup node info test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, DUP_NODE_INFO_TEST_001, TestSize.Level1)
{
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, SoftbusGetConfig(_, _, _)).WillRepeatedly(Return(SOFTBUS_ERR));
    NetBuilderConfigInit();
    NodeInfo info;
    NodeInfo *ret = nullptr;
    ret = DupNodeInfo(&info);
    EXPECT_TRUE(ret != nullptr);
    if (ret != nullptr) {
        SoftBusFree(ret);
    }
    CleanConnectionFsm(nullptr);
    EXPECT_TRUE(CreateNetworkIdMsgPara(nullptr) == nullptr);
    EXPECT_TRUE(CreateConnectionAddrMsgParaKey(nullptr) == nullptr);
    EXPECT_TRUE(CreateConnectionAddrMsgPara(nullptr) == nullptr);
}

/*
* @tc.name: FIND_CONNECTION_FSM_TEST_001
* @tc.desc: net builder config init test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, FIND_CONNECTION_FSM_TEST_001, TestSize.Level1)
{
    LnnConnectionFsm *connFsm = reinterpret_cast<LnnConnectionFsm *>(SoftBusMalloc(sizeof(LnnConnectionFsm)));
    ListInit(&connFsm->node);
    (void)strcpy_s(connFsm->connInfo.addr.info.br.brMac, BT_MAC_LEN, NODE1_BR_MAC);
    (void)strcpy_s(connFsm->connInfo.peerNetworkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);
    connFsm->connInfo.addr.type = CONNECTION_ADDR_BR;
    connFsm->connInfo.requestId = REQUEST_ID;
    connFsm->connInfo.authId = AUTH_ID;
    connFsm->id = FSM_ID;
    ListAdd(&g_netBuilder.fsmList, &connFsm->node);
    MetaJoinRequestNode *requestNode =
        reinterpret_cast<MetaJoinRequestNode *>(SoftBusMalloc(sizeof(MetaJoinRequestNode)));
    ListInit(&requestNode->node);
    (void)strcpy_s(requestNode->addr.info.br.brMac, BT_MAC_LEN, NODE1_BR_MAC);
    requestNode->requestId = REQUEST_ID;
    ListAdd(&g_netBuilder.metaNodeList, &requestNode->node);

    ConnectionAddr addr;
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    (void)memset_s(&addr, sizeof(ConnectionAddr), 0, sizeof(ConnectionAddr));
    (void)strcpy_s(addr.info.br.brMac, BT_MAC_LEN, NODE1_BR_MAC);
    addr.type = CONNECTION_ADDR_BR;
    EXPECT_CALL(NetBuilderMock, LnnIsSameConnectionAddr(_, _)).WillOnce(Return(true));
    EXPECT_TRUE(FindConnectionFsmByAddr(&addr) != nullptr);
    EXPECT_CALL(NetBuilderMock, LnnIsSameConnectionAddr(_, _)).WillOnce(Return(true));
    EXPECT_TRUE(FindMetaNodeByAddr(&addr) != nullptr);
    addr.type = CONNECTION_ADDR_BLE;
    EXPECT_CALL(NetBuilderMock, LnnIsSameConnectionAddr(_, _)).WillOnce(Return(false));
    EXPECT_TRUE(FindConnectionFsmByAddr(&addr) == nullptr);
    EXPECT_CALL(NetBuilderMock, LnnIsSameConnectionAddr(_, _)).WillOnce(Return(false));
    EXPECT_TRUE(FindMetaNodeByAddr(&addr) == nullptr);

    EXPECT_TRUE(FindMetaNodeByRequestId(REQUEST_ID) != nullptr);
    EXPECT_TRUE(FindConnectionFsmByRequestId(REQUEST_ID) != nullptr);
    EXPECT_TRUE(FindMetaNodeByRequestId(REQUEST_ID_ADD) == nullptr);

    EXPECT_TRUE(FindConnectionFsmByRequestId(REQUEST_ID_ADD) == nullptr);
    EXPECT_TRUE(FindConnectionFsmByAuthId(AUTH_ID) != nullptr);
    EXPECT_TRUE(FindConnectionFsmByAuthId(AUTH_ID_ADD) == nullptr);
    EXPECT_TRUE(FindConnectionFsmByNetworkId(NODE_NETWORK_ID) != nullptr);
    EXPECT_TRUE(FindConnectionFsmByNetworkId(NODE1_NETWORK_ID) == nullptr);
    EXPECT_TRUE(FindConnectionFsmByConnFsmId(FSM_ID) != nullptr);
    EXPECT_TRUE(FindConnectionFsmByConnFsmId(FSM_ID_ADD) == nullptr);

    ListDelete(&connFsm->node);
    ListDelete(&requestNode->node);
    SoftBusFree(connFsm);
    SoftBusFree(requestNode);
}

/*
* @tc.name: SYNC_ELECT_MESSAGE_TEST_001
* @tc.desc: sync elect meaasge test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, SYNC_ELECT_MESSAGE_TEST_001, TestSize.Level1)
{
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnGetLocalStrInfo(_, _, _)).WillOnce(Return(SOFTBUS_ERR));
    int32_t ret = SyncElectMessage(NODE_NETWORK_ID);
    EXPECT_TRUE(ret == SOFTBUS_INVALID_PARAM);

    EXPECT_CALL(NetBuilderMock, LnnGetLocalStrInfo(_, _, _)).WillOnce(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnGetLocalNumInfo(_, _)).WillOnce(Return(SOFTBUS_ERR));
    ret = SyncElectMessage(NODE_NETWORK_ID);
    EXPECT_TRUE(ret == SOFTBUS_INVALID_PARAM);

    EXPECT_CALL(NetBuilderMock, LnnGetLocalStrInfo(_, _, _)).WillOnce(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnGetLocalNumInfo(_, _)).WillOnce(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, AddStringToJsonObject(_, _, _)).WillOnce(Return(false));
    ret = SyncElectMessage(NODE_NETWORK_ID);
    EXPECT_TRUE(ret == SOFTBUS_ERR);

    EXPECT_CALL(NetBuilderMock, LnnGetLocalStrInfo(_, _, _)).WillOnce(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnGetLocalNumInfo(_, _)).WillOnce(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, AddStringToJsonObject(_, _, _)).WillOnce(Return(true));
    EXPECT_CALL(NetBuilderMock, AddNumberToJsonObject(_, _, _)).WillOnce(Return(false));
    ret = SyncElectMessage(NODE_NETWORK_ID);
    EXPECT_TRUE(ret == SOFTBUS_ERR);

    EXPECT_CALL(NetBuilderMock, LnnGetLocalStrInfo(_, _, _)).WillOnce(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnGetLocalNumInfo(_, _)).WillOnce(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, AddStringToJsonObject(_, _, _)).WillOnce(Return(true));
    EXPECT_CALL(NetBuilderMock, AddNumberToJsonObject(_, _, _)).WillOnce(Return(true));
    EXPECT_CALL(NetBuilderMock, LnnSendSyncInfoMsg(_, _, _, _, _)).WillOnce(Return(SOFTBUS_ERR));
    ret = SyncElectMessage(NODE_NETWORK_ID);
    EXPECT_TRUE(ret == SOFTBUS_ERR);

    EXPECT_CALL(NetBuilderMock, LnnGetLocalStrInfo(_, _, _)).WillOnce(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnGetLocalNumInfo(_, _)).WillOnce(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, AddStringToJsonObject(_, _, _)).WillOnce(Return(true));
    EXPECT_CALL(NetBuilderMock, AddNumberToJsonObject(_, _, _)).WillOnce(Return(true));
    EXPECT_CALL(NetBuilderMock, LnnSendSyncInfoMsg(_, _, _, _, _)).WillOnce(Return(SOFTBUS_OK));
    ret = SyncElectMessage(NODE_NETWORK_ID);
    EXPECT_TRUE(ret == SOFTBUS_OK);
}

/*
* @tc.name: SEND_ELECT_MESSAGE_TO_ALL_TEST_001
* @tc.desc: send elect message to all test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, SEND_ELECT_MESSAGE_TO_ALL_TEST_001, TestSize.Level1)
{
    LnnConnectionFsm *connFsm = reinterpret_cast<LnnConnectionFsm *>(SoftBusMalloc(sizeof(LnnConnectionFsm)));
    ListInit(&connFsm->node);
    (void)strcpy_s(connFsm->connInfo.peerNetworkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);
    connFsm->isDead = false;
    connFsm->connInfo.flag = CONN_FLAG1;
    ListAdd(&g_netBuilder.fsmList, &connFsm->node);
    g_netBuilder.maxConcurrentCount = 1;
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnGetNodeInfoById(_, _)).WillOnce(Return(nullptr));
    SendElectMessageToAll(NODE1_NETWORK_ID);
    NodeInfo nodeInfo;
    EXPECT_CALL(NetBuilderMock, LnnGetNodeInfoById(_, _)).WillOnce(Return(&nodeInfo));
    EXPECT_CALL(NetBuilderMock, LnnIsNodeOnline(_)).WillOnce(Return(true));
    EXPECT_CALL(NetBuilderMock, LnnGetLocalStrInfo(_, _, _)).WillOnce(Return(SOFTBUS_ERR));
    SendElectMessageToAll(NODE1_NETWORK_ID);
    EXPECT_TRUE(NeedPendingJoinRequest() == false);
    ListDelete(&connFsm->node);
    SoftBusFree(connFsm);
}

/*
* @tc.name: INITIATE_NEW_NETWORK_ONLINE_TEST_001
* @tc.desc: initiate new network online test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, INITIATE_NEW_NETWORK_ONLINE_TEST_001, TestSize.Level1)
{
    LnnConnectionFsm *connFsm = reinterpret_cast<LnnConnectionFsm *>(SoftBusMalloc(sizeof(LnnConnectionFsm)));
    ListInit(&connFsm->node);
    (void)strcpy_s(connFsm->connInfo.peerNetworkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);
    connFsm->connInfo.addr.type = CONNECTION_ADDR_BR;
    connFsm->isDead = false;
    connFsm->connInfo.flag = CONN_FLAG2;
    ListAdd(&g_netBuilder.fsmList, &connFsm->node);

    InitiateNewNetworkOnline(CONNECTION_ADDR_MAX, NODE1_NETWORK_ID);
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnSendNewNetworkOnlineToConnFsm(_)).WillOnce(Return(SOFTBUS_OK));
    InitiateNewNetworkOnline(CONNECTION_ADDR_MAX, NODE_NETWORK_ID);
    
    EXPECT_CALL(NetBuilderMock, LnnSendNewNetworkOnlineToConnFsm(_)).WillOnce(Return(SOFTBUS_OK));
    InitiateNewNetworkOnline(CONNECTION_ADDR_BR, NODE_NETWORK_ID);
    InitiateNewNetworkOnline(CONNECTION_ADDR_BLE, NODE_NETWORK_ID);
    ListDelete(&connFsm->node);
    SoftBusFree(connFsm);
}


/*
* @tc.name: TYR_INITIATE_NEW_NETWORK_ONLINE_TEST_001
* @tc.desc: try initiate new network online test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, TYR_INITIATE_NEW_NETWORK_ONLINE_TEST_001, TestSize.Level1)
{
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, SoftbusGetConfig(_, _, _)).WillRepeatedly(Return(SOFTBUS_OK));
    LnnConnectionFsm *connFsm = reinterpret_cast<LnnConnectionFsm *>(SoftBusMalloc(sizeof(LnnConnectionFsm)));
    ListInit(&connFsm->node);
    (void)strcpy_s(connFsm->connInfo.peerNetworkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);
    connFsm->connInfo.flag = CONN_FLAG2;
    ListAdd(&g_netBuilder.fsmList, &connFsm->node);

    LnnConnectionFsm fsmTest;
    (void)memset_s(&fsmTest, sizeof(LnnConnectionFsm), 0, sizeof(LnnConnectionFsm));
    connFsm->connInfo.flag = CONN_FLAG1;
    (void)strcpy_s(fsmTest.connInfo.peerNetworkId, NETWORK_ID_BUF_LEN, NODE1_NETWORK_ID);
    TryInitiateNewNetworkOnline(&fsmTest);

    (void)strcpy_s(fsmTest.connInfo.peerNetworkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);
    TryInitiateNewNetworkOnline(&fsmTest);
    ListDelete(&connFsm->node);
    SoftBusFree(connFsm);
}

/*
* @tc.name: TRY_DISCONNECT_ALL_CONNECTION_TEST_001
* @tc.desc: tyr disconnect all connection test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, TRY_DISCONNECT_ALL_CONNECTION_TEST_001, TestSize.Level1)
{
    LnnConnectionFsm *connFsm = reinterpret_cast<LnnConnectionFsm *>(SoftBusMalloc(sizeof(LnnConnectionFsm)));
    ListInit(&connFsm->node);
    (void)strcpy_s(connFsm->connInfo.addr.info.br.brMac, BT_MAC_LEN, NODE1_BR_MAC);
    connFsm->connInfo.addr.type = CONNECTION_ADDR_BR;
    ListAdd(&g_netBuilder.fsmList, &connFsm->node);

    LnnConnectionFsm fsmTest;
    (void)memset_s(&fsmTest, sizeof(LnnConnectionFsm), 0, sizeof(LnnConnectionFsm));
    fsmTest.connInfo.flag = 1;
    TryDisconnectAllConnection(&fsmTest);

    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnConvertAddrToOption(_, _)).WillOnce(Return(false));
    fsmTest.connInfo.flag = CONN_FLAG1;
    fsmTest.connInfo.addr.type = CONNECTION_ADDR_BLE;
    TryDisconnectAllConnection(&fsmTest);

    EXPECT_CALL(NetBuilderMock, LnnConvertAddrToOption(_, _)).WillOnce(Return(true));
    EXPECT_CALL(NetBuilderMock, ConnDisconnectDeviceAllConn(_)).WillOnce(Return(SOFTBUS_OK));
    TryDisconnectAllConnection(&fsmTest);

    fsmTest.connInfo.addr.type = CONNECTION_ADDR_BR;
    (void)strcpy_s(fsmTest.connInfo.addr.info.br.brMac, BT_MAC_LEN, NODE1_BR_MAC);
    TryDisconnectAllConnection(&fsmTest);

    (void)strcpy_s(fsmTest.connInfo.addr.info.br.brMac, BT_MAC_LEN, NODE2_BR_MAC);
    TryDisconnectAllConnection(&fsmTest);
    ListDelete(&connFsm->node);
    SoftBusFree(connFsm);
}

/*
* @tc.name: TRY_DISCONNECT_ALL_CONNECTION_TEST_002
* @tc.desc: tyr disconnect all connection test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, TRY_DISCONNECT_ALL_CONNECTION_TEST_002, TestSize.Level1)
{
    LnnConnectionFsm *connFsm = reinterpret_cast<LnnConnectionFsm *>(SoftBusMalloc(sizeof(LnnConnectionFsm)));
    ListInit(&connFsm->node);
    (void)strcpy_s(connFsm->connInfo.addr.info.br.brMac, BT_MAC_LEN, NODE1_BR_MAC);
    connFsm->connInfo.addr.type = CONNECTION_ADDR_BLE;
    ListAdd(&g_netBuilder.fsmList, &connFsm->node);

    LnnConnectionFsm fsmTest;
    (void)memset_s(&fsmTest, sizeof(LnnConnectionFsm), 0, sizeof(LnnConnectionFsm));
    fsmTest.connInfo.flag = CONN_FLAG1;
    fsmTest.connInfo.addr.type = CONNECTION_ADDR_BLE;
    (void)strcpy_s(fsmTest.connInfo.addr.info.br.brMac, BT_MAC_LEN, NODE1_BR_MAC);
    TryDisconnectAllConnection(&fsmTest);
    ListDelete(&connFsm->node);
    SoftBusFree(connFsm);
}

/*
* @tc.name: TRY_DISCONNECT_ALL_CONNECTION_TEST_003
* @tc.desc: tyr disconnect all connection test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, TRY_DISCONNECT_ALL_CONNECTION_TEST_003, TestSize.Level1)
{
    LnnConnectionFsm *connFsm = reinterpret_cast<LnnConnectionFsm *>(SoftBusMalloc(sizeof(LnnConnectionFsm)));
    ListInit(&connFsm->node);
    (void)strcpy_s(connFsm->connInfo.addr.info.ip.ip, IP_STR_MAX_LEN, NODE1_IP);
    connFsm->connInfo.addr.type = CONNECTION_ADDR_WLAN;
    ListAdd(&g_netBuilder.fsmList, &connFsm->node);

    LnnConnectionFsm fsmTest;
    (void)memset_s(&fsmTest, sizeof(LnnConnectionFsm), 0, sizeof(LnnConnectionFsm));
    fsmTest.connInfo.flag = CONN_FLAG1;
    fsmTest.connInfo.addr.type = CONNECTION_ADDR_WLAN;
    (void)strcpy_s(fsmTest.connInfo.addr.info.ip.ip, IP_STR_MAX_LEN, NODE2_IP);

    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnConvertAddrToOption(_, _)).WillOnce(Return(false));
    TryDisconnectAllConnection(&fsmTest);
    TryNotifyAllTypeOffline(&fsmTest);

    (void)strcpy_s(fsmTest.connInfo.addr.info.ip.ip, IP_STR_MAX_LEN, NODE1_IP);
    TryDisconnectAllConnection(&fsmTest);
    TryNotifyAllTypeOffline(&fsmTest);

    ListDelete(&connFsm->node);
    SoftBusFree(connFsm);
}

/*
* @tc.name: TRY_DISCONNECT_ALL_CONNECTION_TEST_004
* @tc.desc: tyr disconnect all connection test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, TRY_DISCONNECT_ALL_CONNECTION_TEST_004, TestSize.Level1)
{
    LnnConnectionFsm *connFsm = reinterpret_cast<LnnConnectionFsm *>(SoftBusMalloc(sizeof(LnnConnectionFsm)));
    ListInit(&connFsm->node);
    (void)strcpy_s(connFsm->connInfo.addr.info.ip.ip, IP_STR_MAX_LEN, NODE1_IP);
    connFsm->connInfo.addr.type = CONNECTION_ADDR_ETH;
    ListAdd(&g_netBuilder.fsmList, &connFsm->node);

    LnnConnectionFsm fsmTest;
    (void)memset_s(&fsmTest, sizeof(LnnConnectionFsm), 0, sizeof(LnnConnectionFsm));
    fsmTest.connInfo.flag = CONN_FLAG1;
    fsmTest.connInfo.addr.type = CONNECTION_ADDR_ETH;
    (void)strcpy_s(fsmTest.connInfo.addr.info.ip.ip, IP_STR_MAX_LEN, NODE1_IP);
    TryDisconnectAllConnection(&fsmTest);
    ListDelete(&connFsm->node);
    SoftBusFree(connFsm);
}

/*
* @tc.name: PROCESS_VERIFY_RESULT_TEST_002
* @tc.desc: process verify result test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_VERIFY_RESULT_TEST_002, TestSize.Level1)
{
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, SoftbusGetConfig(_, _, _)).WillRepeatedly(Return(SOFTBUS_OK));
    VerifyResultMsgPara *msgPara = reinterpret_cast<VerifyResultMsgPara *>(SoftBusMalloc(sizeof(VerifyResultMsgPara)));
    msgPara->nodeInfo = reinterpret_cast<NodeInfo *>(SoftBusMalloc(sizeof(NodeInfo)));
    msgPara->requestId = REQUEST_ID;
    void *para = reinterpret_cast<void *>(msgPara);

    LnnConnectionFsm *connFsm = reinterpret_cast<LnnConnectionFsm *>(SoftBusMalloc(sizeof(LnnConnectionFsm)));
    ListInit(&connFsm->node);
    connFsm->connInfo.requestId = REQUEST_ID;
    connFsm->isDead = true;
    ListAdd(&g_netBuilder.fsmList, &connFsm->node);
    EXPECT_TRUE(ProcessVerifyResult(para) == SOFTBUS_ERR);
    ListDelete(&connFsm->node);
    SoftBusFree(connFsm);
}

/*
* @tc.name: PROCESS_VERIFY_RESULT_TEST_003
* @tc.desc: process verify result test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_VERIFY_RESULT_TEST_003, TestSize.Level1)
{
    VerifyResultMsgPara *msgPara = reinterpret_cast<VerifyResultMsgPara *>(SoftBusMalloc(sizeof(VerifyResultMsgPara)));
    msgPara->nodeInfo = reinterpret_cast<NodeInfo *>(SoftBusMalloc(sizeof(NodeInfo)));
    msgPara->requestId = REQUEST_ID;
    msgPara->retCode = SOFTBUS_ERR;
    void *para = reinterpret_cast<void *>(msgPara);

    LnnConnectionFsm *connFsm = reinterpret_cast<LnnConnectionFsm *>(SoftBusMalloc(sizeof(LnnConnectionFsm)));
    ListInit(&connFsm->node);
    connFsm->connInfo.requestId = REQUEST_ID;
    connFsm->isDead = false;
    ListAdd(&g_netBuilder.fsmList, &connFsm->node);

    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnSendAuthResultMsgToConnFsm(_, _)).WillOnce(Return(SOFTBUS_ERR));
    EXPECT_TRUE(ProcessVerifyResult(para) == SOFTBUS_ERR);
    ListDelete(&connFsm->node);
    SoftBusFree(connFsm);
}

/*
* @tc.name: PROCESS_VERIFY_RESULT_TEST_004
* @tc.desc: process verify result test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_VERIFY_RESULT_TEST_004, TestSize.Level1)
{
    VerifyResultMsgPara *msgPara = reinterpret_cast<VerifyResultMsgPara *>(SoftBusMalloc(sizeof(VerifyResultMsgPara)));
    msgPara->nodeInfo = reinterpret_cast<NodeInfo *>(SoftBusMalloc(sizeof(NodeInfo)));
    msgPara->requestId = REQUEST_ID;
    msgPara->retCode = SOFTBUS_ERR;
    void *para = reinterpret_cast<void *>(msgPara);
    LnnConnectionFsm *connFsm = reinterpret_cast<LnnConnectionFsm *>(SoftBusMalloc(sizeof(LnnConnectionFsm)));
    ListInit(&connFsm->node);
    connFsm->connInfo.requestId = REQUEST_ID;
    connFsm->isDead = false;
    ListAdd(&g_netBuilder.fsmList, &connFsm->node);
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnSendAuthResultMsgToConnFsm(_, _)).WillOnce(Return(SOFTBUS_OK));
    EXPECT_TRUE(ProcessVerifyResult(para) == SOFTBUS_OK);
    ListDelete(&connFsm->node);
    SoftBusFree(connFsm);
}

/*
* @tc.name: PROCESS_DEVICE_VERIFY_PASS_TEST_002
* @tc.desc: process device verify pass test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_DEVICE_VERIFY_PASS_TEST_002, TestSize.Level1)
{
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, SoftbusGetConfig(_, _, _)).WillRepeatedly(Return(SOFTBUS_OK));
    DeviceVerifyPassMsgPara *msgPara =
        reinterpret_cast<DeviceVerifyPassMsgPara *>(SoftBusMalloc(sizeof(DeviceVerifyPassMsgPara)));
    msgPara->nodeInfo = reinterpret_cast<NodeInfo *>(SoftBusMalloc(sizeof(NodeInfo)));
    (void)strcpy_s(msgPara->nodeInfo->networkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);
    msgPara->authId = AUTH_ID;

    LnnConnectionFsm *connFsm = reinterpret_cast<LnnConnectionFsm *>(SoftBusMalloc(sizeof(LnnConnectionFsm)));
    ListInit(&connFsm->node);
    (void)strcpy_s(connFsm->connInfo.peerNetworkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);
    connFsm->connInfo.authId = AUTH_ID_ADD;
    ListAdd(&g_netBuilder.fsmList, &connFsm->node);
    g_netBuilder.connCount = CURRENT_COUNT;
    g_netBuilder.maxConnCount = CONN_COUNT;
    void *para = reinterpret_cast<void *>(msgPara);
    EXPECT_TRUE(ProcessDeviceVerifyPass(para) == SOFTBUS_ERR);
    ListDelete(&connFsm->node);
    SoftBusFree(connFsm);
}

/*
* @tc.name: PROCESS_DEVICE_VERIFY_PASS_TEST_003
* @tc.desc: process device verify pass test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_DEVICE_VERIFY_PASS_TEST_003, TestSize.Level1)
{
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, SoftbusGetConfig(_, _, _)).WillRepeatedly(Return(SOFTBUS_OK));
    DeviceVerifyPassMsgPara *msgPara =
        reinterpret_cast<DeviceVerifyPassMsgPara *>(SoftBusMalloc(sizeof(DeviceVerifyPassMsgPara)));
    msgPara->nodeInfo = reinterpret_cast<NodeInfo *>(SoftBusMalloc(sizeof(NodeInfo)));
    (void)strcpy_s(msgPara->nodeInfo->networkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);
    msgPara->authId = AUTH_ID;

    LnnConnectionFsm *connFsm = reinterpret_cast<LnnConnectionFsm *>(SoftBusMalloc(sizeof(LnnConnectionFsm)));
    ListInit(&connFsm->node);
    (void)strcpy_s(connFsm->connInfo.peerNetworkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);
    connFsm->connInfo.authId = AUTH_ID;
    connFsm->isDead = true;
    ListAdd(&g_netBuilder.fsmList, &connFsm->node);
    g_netBuilder.connCount = CURRENT_COUNT;
    g_netBuilder.maxConnCount = CONN_COUNT;
    void *para = reinterpret_cast<void *>(msgPara);
    EXPECT_TRUE(ProcessDeviceVerifyPass(para) == SOFTBUS_ERR);
    ListDelete(&connFsm->node);
    SoftBusFree(connFsm);
}

/*
* @tc.name: PROCESS_DEVICE_VERIFY_PASS_TEST_004
* @tc.desc: process device verify pass test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_DEVICE_VERIFY_PASS_TEST_004, TestSize.Level1)
{
    DeviceVerifyPassMsgPara *msgPara =
        reinterpret_cast<DeviceVerifyPassMsgPara *>(SoftBusMalloc(sizeof(DeviceVerifyPassMsgPara)));
    msgPara->nodeInfo = reinterpret_cast<NodeInfo *>(SoftBusMalloc(sizeof(NodeInfo)));
    (void)strcpy_s(msgPara->nodeInfo->networkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);
    msgPara->authId = AUTH_ID;

    LnnConnectionFsm *connFsm = reinterpret_cast<LnnConnectionFsm *>(SoftBusMalloc(sizeof(LnnConnectionFsm)));
    ListInit(&connFsm->node);
    (void)strcpy_s(connFsm->connInfo.peerNetworkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);
    connFsm->connInfo.authId = AUTH_ID;
    connFsm->isDead = false;
    ListAdd(&g_netBuilder.fsmList, &connFsm->node);

    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnConvAddrTypeToDiscType(_)).WillOnce(Return(DISCOVERY_TYPE_WIFI));
    EXPECT_CALL(NetBuilderMock, LnnUpdateNodeInfo(_)).WillOnce(Return(SOFTBUS_ERR));
    void *para = reinterpret_cast<void *>(msgPara);
    EXPECT_TRUE(ProcessDeviceVerifyPass(para) == SOFTBUS_ERR);
    ListDelete(&connFsm->node);
    SoftBusFree(connFsm);
}

/*
* @tc.name: PROCESS_DEVICE_NOT_TRUSTED_TEST_002
* @tc.desc: process device not trusted test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_DEVICE_NOT_TRUSTED_TEST_002, TestSize.Level1)
{
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    void *msgPara = reinterpret_cast<void *>(SoftBusMalloc(sizeof(char) * UDID_BUF_LEN));
    EXPECT_CALL(NetBuilderMock, LnnGetNetworkIdByUdid(_, _, _)).WillRepeatedly(Return(SOFTBUS_ERR));
    EXPECT_CALL(NetBuilderMock, LnnGetDeviceUdid(_)).WillRepeatedly(Return(nullptr));

    LnnConnectionFsm *connFsm = reinterpret_cast<LnnConnectionFsm *>(SoftBusMalloc(sizeof(LnnConnectionFsm)));
    ListInit(&connFsm->node);
    (void)strcpy_s(connFsm->connInfo.peerNetworkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);
    ListAdd(&g_netBuilder.fsmList, &connFsm->node);

    void *para = reinterpret_cast<void *>(msgPara);
    EXPECT_TRUE(ProcessDeviceNotTrusted(para) == SOFTBUS_OK);
    ListDelete(&connFsm->node);
    SoftBusFree(connFsm);
}

/*
* @tc.name: PROCESS_LEAVE_LNN_REQUEST_TEST_002
* @tc.desc: process leave lnn request test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_LEAVE_LNN_REQUEST_TEST_002, TestSize.Level1)
{
    char *msgPara = reinterpret_cast<char *>(SoftBusMalloc(sizeof(char) * NETWORK_ID_BUF_LEN));
    (void)strcpy_s(msgPara, NETWORK_ID_BUF_LEN, NODE1_NETWORK_ID);

    LnnConnectionFsm *connFsm = reinterpret_cast<LnnConnectionFsm *>(SoftBusMalloc(sizeof(LnnConnectionFsm)));
    ListInit(&connFsm->node);
    (void)strcpy_s(connFsm->connInfo.peerNetworkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);
    ListAdd(&g_netBuilder.fsmList, &connFsm->node);

    void *para = reinterpret_cast<void *>(msgPara);
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnNotifyLeaveResult(_, _)).WillRepeatedly(Return());
    EXPECT_TRUE(ProcessLeaveLNNRequest(para) == SOFTBUS_ERR);
    ListDelete(&connFsm->node);
    SoftBusFree(connFsm);
}

/*
* @tc.name: PROCESS_LEAVE_LNN_REQUEST_TEST_003
* @tc.desc: process leave lnn request test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_LEAVE_LNN_REQUEST_TEST_003, TestSize.Level1)
{
    char *msgPara = reinterpret_cast<char *>(SoftBusMalloc(sizeof(char) * NETWORK_ID_BUF_LEN));
    (void)strcpy_s(msgPara, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);

    LnnConnectionFsm *connFsm = reinterpret_cast<LnnConnectionFsm *>(SoftBusMalloc(sizeof(LnnConnectionFsm)));
    ListInit(&connFsm->node);
    (void)strcpy_s(connFsm->connInfo.peerNetworkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);
    connFsm->isDead = true;
    ListAdd(&g_netBuilder.fsmList, &connFsm->node);

    void *para = reinterpret_cast<void *>(msgPara);
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnNotifyLeaveResult(_, _)).WillRepeatedly(Return());
    EXPECT_TRUE(ProcessLeaveLNNRequest(para) == SOFTBUS_ERR);
    ListDelete(&connFsm->node);
    SoftBusFree(connFsm);
}

/*
* @tc.name: PROCESS_LEAVE_LNN_REQUEST_TEST_004
* @tc.desc: process leave lnn request test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_LEAVE_LNN_REQUEST_TEST_004, TestSize.Level1)
{
    char *msgPara = reinterpret_cast<char *>(SoftBusMalloc(sizeof(char) * NETWORK_ID_BUF_LEN));
    (void)strcpy_s(msgPara, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);

    LnnConnectionFsm *connFsm = reinterpret_cast<LnnConnectionFsm *>(SoftBusMalloc(sizeof(LnnConnectionFsm)));
    ListInit(&connFsm->node);
    (void)strcpy_s(connFsm->connInfo.peerNetworkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);
    connFsm->isDead = false;
    ListAdd(&g_netBuilder.fsmList, &connFsm->node);

    void *para = reinterpret_cast<void *>(msgPara);
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnNotifyLeaveResult(_, _)).WillRepeatedly(Return());
    EXPECT_CALL(NetBuilderMock, LnnSendLeaveRequestToConnFsm(_)).WillRepeatedly(Return(SOFTBUS_OK));
    EXPECT_TRUE(ProcessLeaveLNNRequest(para) == SOFTBUS_OK);
    ListDelete(&connFsm->node);
    SoftBusFree(connFsm);
}

/*
* @tc.name: PROCESS_SYNC_OFFLINE_FINISH_TEST_002
* @tc.desc: process sync offline finish test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_SYNC_OFFLINE_FINISH_TEST_002, TestSize.Level1)
{
    char *msgPara = reinterpret_cast<char *>(SoftBusMalloc(sizeof(char) * NETWORK_ID_BUF_LEN));
    (void)strcpy_s(msgPara, NETWORK_ID_BUF_LEN, NODE1_NETWORK_ID);

    LnnConnectionFsm *connFsm = reinterpret_cast<LnnConnectionFsm *>(SoftBusMalloc(sizeof(LnnConnectionFsm)));
    ListInit(&connFsm->node);
    (void)strcpy_s(connFsm->connInfo.peerNetworkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);
    ListAdd(&g_netBuilder.fsmList, &connFsm->node);

    void *para = reinterpret_cast<void *>(msgPara);
    EXPECT_TRUE(ProcessSyncOfflineFinish(para) == SOFTBUS_OK);
    ListDelete(&connFsm->node);
    SoftBusFree(connFsm);
}

/*
* @tc.name: PROCESS_SYNC_OFFLINE_FINISH_TEST_003
* @tc.desc: process sync offline finish test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_SYNC_OFFLINE_FINISH_TEST_003, TestSize.Level1)
{
    char *msgPara = reinterpret_cast<char *>(SoftBusMalloc(sizeof(char) * NETWORK_ID_BUF_LEN));
    (void)strcpy_s(msgPara, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);

    LnnConnectionFsm *connFsm = reinterpret_cast<LnnConnectionFsm *>(SoftBusMalloc(sizeof(LnnConnectionFsm)));
    ListInit(&connFsm->node);
    connFsm->isDead = true;
    (void)strcpy_s(connFsm->connInfo.peerNetworkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);
    ListAdd(&g_netBuilder.fsmList, &connFsm->node);

    void *para = reinterpret_cast<void *>(msgPara);
    EXPECT_TRUE(ProcessSyncOfflineFinish(para) == SOFTBUS_OK);
    ListDelete(&connFsm->node);
    SoftBusFree(connFsm);
}

/*
* @tc.name: PROCESS_LEAVE_SPECIFIC_TEST_002
* @tc.desc: process leave specific test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_LEAVE_SPECIFIC_TEST_002, TestSize.Level1)
{
    SpecificLeaveMsgPara *msgPara = reinterpret_cast<SpecificLeaveMsgPara *>(SoftBusMalloc(sizeof(SpecificLeaveMsgPara)));
    (void)strcpy_s(msgPara->networkId, NETWORK_ID_BUF_LEN, NODE1_NETWORK_ID);
    msgPara->addrType = CONNECTION_ADDR_BLE;

    LnnConnectionFsm *connFsm = reinterpret_cast<LnnConnectionFsm *>(SoftBusMalloc(sizeof(LnnConnectionFsm)));
    ListInit(&connFsm->node);
    (void)strcpy_s(connFsm->connInfo.peerNetworkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);
    connFsm->connInfo.addr.type = CONNECTION_ADDR_BR;
    ListAdd(&g_netBuilder.fsmList, &connFsm->node);

    void *para = reinterpret_cast<void *>(msgPara);
    EXPECT_TRUE(ProcessLeaveSpecific(para) == SOFTBUS_OK);
    ListDelete(&connFsm->node);
    SoftBusFree(connFsm);
}

/*
* @tc.name: PROCESS_LEAVE_SPECIFIC_TEST_003
* @tc.desc: process leave specific test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_LEAVE_SPECIFIC_TEST_003, TestSize.Level1)
{
    SpecificLeaveMsgPara *msgPara = reinterpret_cast<SpecificLeaveMsgPara *>(SoftBusMalloc(sizeof(SpecificLeaveMsgPara)));
    (void)strcpy_s(msgPara->networkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);
    LnnConnectionFsm *connFsm = reinterpret_cast<LnnConnectionFsm *>(SoftBusMalloc(sizeof(LnnConnectionFsm)));
    ListInit(&connFsm->node);
    (void)strcpy_s(connFsm->connInfo.peerNetworkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);
    ListAdd(&g_netBuilder.fsmList, &connFsm->node);

    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, LnnSendLeaveRequestToConnFsm(_)).WillRepeatedly(Return(SOFTBUS_OK));
    void *para = reinterpret_cast<void *>(msgPara);
    EXPECT_TRUE(ProcessLeaveSpecific(para) == SOFTBUS_OK);
    ListDelete(&connFsm->node);
    SoftBusFree(connFsm);
}

/*
* @tc.name: ON_LNN_PROCESS_NOT_TRUSTED_MSG_DELAY_TEST_001
* @tc.desc: on lnn prodecc not trusted msg delay test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, ON_LNN_PROCESS_NOT_TRUSTED_MSG_DELAY_TEST_001, TestSize.Level1)
{
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, SoftbusGetConfig(_, _, _)).WillRepeatedly(Return(SOFTBUS_OK));
    OnLnnProcessNotTrustedMsgDelay(nullptr);
    void *para1 = reinterpret_cast<void *>(SoftBusMalloc(sizeof(NotTrustedDelayInfo)));
    EXPECT_CALL(NetBuilderMock, LnnGetAllAuthSeq(_, _, _)).WillOnce(Return(SOFTBUS_ERR));
    OnLnnProcessNotTrustedMsgDelay(para1);

    void *para2 = reinterpret_cast<void *>(SoftBusMalloc(sizeof(NotTrustedDelayInfo)));
    EXPECT_CALL(NetBuilderMock, LnnGetAllAuthSeq(_, _, _)).WillOnce(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnConvertDlId(_, _, _, _, _)).WillOnce(Return(SOFTBUS_ERR));
    OnLnnProcessNotTrustedMsgDelay(para2);
}

/*
* @tc.name: ON_AUTH_META_VERIFY_PASSED_TEST_001
* @tc.desc: on auth meta verify passed test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, ON_AUTH_META_VERIFY_PASSED_TEST_001, TestSize.Level1)
{
    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    EXPECT_CALL(NetBuilderMock, SoftbusGetConfig(_, _, _)).WillRepeatedly(Return(SOFTBUS_OK));
    OnAuthMetaVerifyPassed(REQUEST_ID, AUTH_META_ID, nullptr);
    MetaJoinRequestNode *requestNode =
        reinterpret_cast<MetaJoinRequestNode *>(SoftBusMalloc(sizeof(MetaJoinRequestNode)));
    ListInit(&requestNode->node);
    requestNode->requestId = REQUEST_ID;
    ListAdd(&g_netBuilder.metaNodeList, &requestNode->node);
    OnAuthMetaVerifyPassed(REQUEST_ID_ADD, AUTH_META_ID, nullptr);
    ListDelete(&requestNode->node);
    SoftBusFree(requestNode);
}

/*
* @tc.name: PROCESS_ELETE_TEST_002
* @tc.desc: process elect test
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(LNNNetBuilderMockTest, PROCESS_ELETE_TEST_002, TestSize.Level1)
{
    ElectMsgPara *msgPara = reinterpret_cast<ElectMsgPara *>(SoftBusMalloc(sizeof(ElectMsgPara)));
    (void)strcpy_s(msgPara->networkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);

    LnnConnectionFsm *connFsm = reinterpret_cast<LnnConnectionFsm *>(SoftBusMalloc(sizeof(LnnConnectionFsm)));
    ListInit(&connFsm->node);
    connFsm->connInfo.version = SOFTBUS_NEW_V1;
    connFsm->isDead = false;
    (void)strcpy_s(connFsm->connInfo.peerNetworkId, NETWORK_ID_BUF_LEN, NODE_NETWORK_ID);
    ListAdd(&g_netBuilder.fsmList, &connFsm->node);

    NiceMock<NetBuilderDepsInterfaceMock> NetBuilderMock;
    NodeInfo nodeInfo;
    EXPECT_CALL(NetBuilderMock, LnnGetNodeInfoById(_, _)).WillOnce(Return(&nodeInfo));
    EXPECT_CALL(NetBuilderMock, LnnIsNodeOnline(_)).WillOnce(Return(true));
    EXPECT_CALL(NetBuilderMock, LnnGetLocalStrInfo(_, _, _)).WillOnce(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnGetLocalNumInfo(_, _)).WillOnce(Return(SOFTBUS_OK));
    EXPECT_CALL(NetBuilderMock, LnnCompareNodeWeight(_, _, _, _)).WillOnce(Return(0));

    void *para = reinterpret_cast<void *>(msgPara);
    EXPECT_TRUE(ProcessMasterElect(para) == SOFTBUS_OK);
    ListDelete(&connFsm->node);
    SoftBusFree(connFsm);
}
} // namespace OHOS
