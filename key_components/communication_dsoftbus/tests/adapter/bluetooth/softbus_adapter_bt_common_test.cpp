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

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include "softbus_adapter_bt_common.h"
#include "softbus_adapter_mem.h"
#include "softbus_errcode.h"

#include "assert_helper.h"
#include "bluetooth_mock.h"

using namespace testing::ext;
using ::testing::_;
using ::testing::AtLeast;
using ::testing::AtMost;
using ::testing::Return;

namespace OHOS {
static SoftBusBtStateListener *GetMockBtStateListener();

static StRecordCtx g_btStateChangedCtx("OnBtStateChanged");
static BtAddrRecordCtx g_btAclStateChangedCtx("OnBtAclStateChanged");

/**
 * @tc.name: AdapterBtCommonTest_ConvertStatus
 * @tc.desc: test enable bt
 * @tc.type: FUNC
 * @tc.require: NONE
 */
HWTEST(AdapterBtCommonTest, SoftBusEnableBt, TestSize.Level3)
{
    MockBluetooth mocker;
    EXPECT_CALL(mocker, EnableBle()).Times(2).WillOnce(Return(true)).WillOnce(Return(false));
    EXPECT_EQ(SoftBusEnableBt(), SOFTBUS_OK);
    EXPECT_EQ(SoftBusEnableBt(), SOFTBUS_ERR);
}

/**
 * @tc.name: AdapterBtCommonTest_SoftBusDisableBt
 * @tc.desc: test disable bt
 * @tc.type: FUNC
 * @tc.require: NONE
 */
HWTEST(AdapterBtCommonTest, SoftBusDisableBt, TestSize.Level3)
{
    MockBluetooth mocker;
    EXPECT_CALL(mocker, DisableBle()).Times(2).WillOnce(Return(true)).WillOnce(Return(false));
    EXPECT_EQ(SoftBusDisableBt(), SOFTBUS_OK);
    EXPECT_EQ(SoftBusDisableBt(), SOFTBUS_ERR);
}

/**
 * @tc.name: AdapterBtCommonTest_SoftBusGetBtState
 * @tc.desc: test get bt status
 * @tc.type: FUNC
 * @tc.require: NONE
 */
HWTEST(AdapterBtCommonTest, SoftBusGetBtState, TestSize.Level3)
{
    MockBluetooth mocker;
    EXPECT_CALL(mocker, IsBleEnabled()).Times(2).WillOnce(Return(true)).WillOnce(Return(false));
    EXPECT_EQ(SoftBusGetBtState(), BLE_ENABLE);
    EXPECT_EQ(SoftBusGetBtState(), BLE_DISABLE);
}

/**
 * @tc.name: AdapterBtCommonTest_SoftBusGetBtMacAddr
 * @tc.desc: test get bt mac address
 * @tc.type: FUNC
 * @tc.require: NONE
 */
HWTEST(AdapterBtCommonTest, SoftBusGetBtMacAddr, TestSize.Level3)
{
    EXPECT_EQ(SoftBusGetBtMacAddr(NULL), SOFTBUS_ERR);
    MockBluetooth mocker;
    SoftBusBtAddr mac = {0};
    EXPECT_CALL(mocker, GetLocalAddr(mac.addr, BT_ADDR_LEN)).Times(2).WillOnce(Return(true)).WillOnce(Return(false));
    EXPECT_EQ(SoftBusGetBtMacAddr(&mac), SOFTBUS_OK);
    EXPECT_EQ(SoftBusGetBtMacAddr(&mac), SOFTBUS_ERR);
}

/**
 * @tc.name: AdapterBtCommonTest_SoftBusSetBtName
 * @tc.desc: test set bt name
 * @tc.type: FUNC
 * @tc.require: NONE
 */
HWTEST(AdapterBtCommonTest, SoftBusSetBtName, TestSize.Level3)
{
    MockBluetooth mocker;
    const char *name = "awesome";
    EXPECT_CALL(mocker, SetLocalName((unsigned char *)name, (unsigned char)strlen(name)))
        .Times(2)
        .WillOnce(Return(true))
        .WillOnce(Return(false));
    EXPECT_EQ(SoftBusSetBtName(name), SOFTBUS_OK);
    EXPECT_EQ(SoftBusSetBtName(name), SOFTBUS_ERR);
}

static testing::AssertionResult PrepareBtStateListener(MockBluetooth &mocker, int *outlistenerId)
{
    EXPECT_CALL(mocker, BleStopScan).WillRepeatedly(Return(OHOS_BT_STATUS_SUCCESS));
    auto listenerId = SoftBusAddBtStateListener(GetMockBtStateListener());
    if (listenerId == SOFTBUS_ERR) {
        return testing::AssertionFailure() << "SoftBusAddBtStateListener failed";
    }
    if (MockBluetooth::btGapCallback == nullptr) {
        return testing::AssertionFailure() << "GapRegisterCallback is not invoke";
    }
    *outlistenerId = listenerId;
    return testing::AssertionSuccess();
}

/**
 * @tc.name: AdapterBtCommonTest_StateChangeCallback
 * @tc.desc: test bt state listener callback stateChangeCallback
 * @tc.type: FUNC
 * @tc.require: NONE
 */
HWTEST(AdapterBtCommonTest, StateChangeCallback, TestSize.Level3)
{
    int listenerId = -1;
    MockBluetooth mocker;
    auto prepareResult = PrepareBtStateListener(mocker, &listenerId);
    ASSERT_TRUE(prepareResult);

    MockBluetooth::btGapCallback->stateChangeCallback(OHOS_BT_TRANSPORT_BR_EDR, OHOS_GAP_STATE_TURNING_ON);
    auto btStateResult = g_btStateChangedCtx.Expect(listenerId, SOFTBUS_BR_STATE_TURNING_ON);
    EXPECT_TRUE(btStateResult);
    MockBluetooth::btGapCallback->stateChangeCallback(OHOS_BT_TRANSPORT_LE, OHOS_GAP_STATE_TURNING_ON);
    btStateResult = g_btStateChangedCtx.Expect(listenerId, SOFTBUS_BT_STATE_TURNING_ON);
    EXPECT_TRUE(btStateResult);

    MockBluetooth::btGapCallback->stateChangeCallback(OHOS_BT_TRANSPORT_BR_EDR, OHOS_GAP_STATE_TURN_ON);
    btStateResult = g_btStateChangedCtx.Expect(listenerId, SOFTBUS_BR_STATE_TURN_ON);
    EXPECT_TRUE(btStateResult);
    MockBluetooth::btGapCallback->stateChangeCallback(OHOS_BT_TRANSPORT_LE, OHOS_GAP_STATE_TURN_ON);
    btStateResult = g_btStateChangedCtx.Expect(listenerId, SOFTBUS_BT_STATE_TURN_ON);
    EXPECT_TRUE(btStateResult);

    MockBluetooth::btGapCallback->stateChangeCallback(OHOS_BT_TRANSPORT_BR_EDR, OHOS_GAP_STATE_TURNING_OFF);
    btStateResult = g_btStateChangedCtx.Expect(listenerId, SOFTBUS_BR_STATE_TURNING_OFF);
    EXPECT_TRUE(btStateResult);
    MockBluetooth::btGapCallback->stateChangeCallback(OHOS_BT_TRANSPORT_LE, OHOS_GAP_STATE_TURNING_OFF);
    btStateResult = g_btStateChangedCtx.Expect(listenerId, SOFTBUS_BT_STATE_TURNING_OFF);
    EXPECT_TRUE(btStateResult);

    MockBluetooth::btGapCallback->stateChangeCallback(OHOS_BT_TRANSPORT_BR_EDR, OHOS_GAP_STATE_TURN_OFF);
    btStateResult = g_btStateChangedCtx.Expect(listenerId, SOFTBUS_BR_STATE_TURN_OFF);
    EXPECT_TRUE(btStateResult);
    MockBluetooth::btGapCallback->stateChangeCallback(OHOS_BT_TRANSPORT_LE, OHOS_GAP_STATE_TURN_OFF);
    btStateResult = g_btStateChangedCtx.Expect(listenerId, SOFTBUS_BT_STATE_TURN_OFF);
    EXPECT_TRUE(btStateResult);

    // invalid status
    MockBluetooth::btGapCallback->stateChangeCallback(OHOS_BT_TRANSPORT_BR_EDR, -1);
    btStateResult = g_btStateChangedCtx.Expect(listenerId, -1);
    EXPECT_TRUE(btStateResult);

    EXPECT_EQ(SoftBusRemoveBtStateListener(listenerId), SOFTBUS_OK);
}

/**
 * @tc.name: AdapterBtCommonTest_StateChangeCallback
 * @tc.desc: test bt state listener callback aclStateChangedCallbak
 * @tc.type: FUNC
 * @tc.require: NONE
 */
HWTEST(AdapterBtCommonTest, AclStateChangedCallbak, TestSize.Level3)
{
    int listenerId = -1;
    MockBluetooth mocker;
    auto prepareResult = PrepareBtStateListener(mocker, &listenerId);
    ASSERT_TRUE(prepareResult);

    MockBluetooth::btGapCallback->aclStateChangedCallbak(nullptr, (GapAclState)-1, 0);

    BdAddr bdAddr = {
        .addr = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66},
    };
    SoftBusBtAddr addr = {
        .addr = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66},
    };
    MockBluetooth::btGapCallback->aclStateChangedCallbak(&bdAddr, OHOS_GAP_ACL_STATE_CONNECTED, 0);
    auto aclStateResult = g_btAclStateChangedCtx.Expect(listenerId, &addr, SOFTBUS_ACL_STATE_CONNECTED);
    EXPECT_TRUE(aclStateResult);

    MockBluetooth::btGapCallback->aclStateChangedCallbak(&bdAddr, OHOS_GAP_ACL_STATE_DISCONNECTED, 0);
    aclStateResult = g_btAclStateChangedCtx.Expect(listenerId, &addr, SOFTBUS_ACL_STATE_DISCONNECTED);
    EXPECT_TRUE(aclStateResult);

    MockBluetooth::btGapCallback->aclStateChangedCallbak(&bdAddr, OHOS_GAP_ACL_STATE_LE_CONNECTED, 0);
    aclStateResult = g_btAclStateChangedCtx.Expect(listenerId, &addr, SOFTBUS_ACL_STATE_LE_CONNECTED);
    EXPECT_TRUE(aclStateResult);

    MockBluetooth::btGapCallback->aclStateChangedCallbak(&bdAddr, OHOS_GAP_ACL_STATE_LE_DISCONNECTED, 0);
    aclStateResult = g_btAclStateChangedCtx.Expect(listenerId, &addr, SOFTBUS_ACL_STATE_LE_DISCONNECTED);
    EXPECT_TRUE(aclStateResult);

    // invalid GapAclState
    MockBluetooth::btGapCallback->aclStateChangedCallbak(&bdAddr, (GapAclState)-1, 0);
    aclStateResult = g_btAclStateChangedCtx.Expect(listenerId, &addr, -1);
    EXPECT_TRUE(aclStateResult);

    EXPECT_EQ(SoftBusRemoveBtStateListener(listenerId), SOFTBUS_OK);
}

/**
 * @tc.name: AdapterBtCommonTest_BluetoothPair
 * @tc.desc: test bt state listener callback pairRequestedCallback
 * @tc.type: FUNC
 * @tc.require: NONE
 */
HWTEST(AdapterBtCommonTest, PairRequestedCallback, TestSize.Level3)
{
    BdAddr bdAddr = {
        .addr = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66},
    };
    MockBluetooth mocker;
    EXPECT_CALL(mocker, PairRequestReply(&bdAddr, OHOS_BT_TRANSPORT_LE, true))
        .Times(2)
        .WillOnce(Return(false))
        .WillOnce(Return(true));

    int listenerId = -1;
    auto prepareResult = PrepareBtStateListener(mocker, &listenerId);
    ASSERT_TRUE(prepareResult);

    MockBluetooth::btGapCallback->pairRequestedCallback(nullptr, OHOS_BT_TRANSPORT_INVALID);
    MockBluetooth::btGapCallback->pairRequestedCallback(&bdAddr, OHOS_BT_TRANSPORT_LE);
    MockBluetooth::btGapCallback->pairRequestedCallback(&bdAddr, OHOS_BT_TRANSPORT_LE);
    EXPECT_EQ(SoftBusRemoveBtStateListener(listenerId), SOFTBUS_OK);
}

/**
 * @tc.name: AdapterBtCommonTest_PairConfiremedCallback
 * @tc.desc: test bt state listener callback pairConfiremedCallback
 * @tc.type: FUNC
 * @tc.require: NONE
 */
HWTEST(AdapterBtCommonTest, PairConfiremedCallback, TestSize.Level3)
{
    BdAddr bdAddr = {
        .addr = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66},
    };
    MockBluetooth mocker;
    EXPECT_CALL(mocker, SetDevicePairingConfirmation(&bdAddr, OHOS_BT_TRANSPORT_LE, true))
        .Times(AtLeast(2))
        .WillOnce(Return(false))
        .WillRepeatedly(Return(true));
    int listenerId = -1;
    auto prepareResult = PrepareBtStateListener(mocker, &listenerId);
    ASSERT_TRUE(prepareResult);

    MockBluetooth::btGapCallback->pairConfiremedCallback(nullptr, OHOS_BT_TRANSPORT_INVALID, 0, 0);
    MockBluetooth::btGapCallback->pairConfiremedCallback(&bdAddr, OHOS_BT_TRANSPORT_LE, 0, 0);
    MockBluetooth::btGapCallback->pairConfiremedCallback(&bdAddr, OHOS_BT_TRANSPORT_LE, 0, 0);
    EXPECT_EQ(SoftBusRemoveBtStateListener(listenerId), SOFTBUS_OK);
}

static void StubOnBtStateChanged(int32_t listenerId, int32_t state)
{
    g_btStateChangedCtx.Update(listenerId, state);
}

static void StubOnBtAclStateChanged(int32_t listenerId, const SoftBusBtAddr *addr, int32_t aclState)
{
    g_btAclStateChangedCtx.Update(listenerId, addr, aclState);
}

static SoftBusBtStateListener *GetMockBtStateListener()
{
    static SoftBusBtStateListener listener = {
        .OnBtStateChanged = StubOnBtStateChanged,
        .OnBtAclStateChanged = StubOnBtAclStateChanged,
    };
    return &listener;
}

} // namespace OHOS
