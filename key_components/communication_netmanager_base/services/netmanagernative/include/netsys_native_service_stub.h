/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef NETSYS_NATIVE_SERVICE_STUB_H
#define NETSYS_NATIVE_SERVICE_STUB_H

#include <map>

#include "i_netsys_service.h"
#include "iremote_stub.h"

namespace OHOS {
namespace NetsysNative {
enum {
    UID_ROOT = 0,
    UID_SHELL = 2000,
    UID_NET_MANAGER = 1099,
    UID_WIFI = 1010,
    UID_RADIO = 1001,
    UID_HIDUMPER_SERVICE = 1212,
    UID_SAMGR = 5555,
    UID_PARAM_WATCHER = 1101,
};

class NetsysNativeServiceStub : public IRemoteStub<INetsysService> {
public:
    NetsysNativeServiceStub();
    ~NetsysNativeServiceStub() = default;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
    int32_t NetsysFreeAddrinfo(struct addrinfo *aihead);

private:
    using ServiceInterface = int32_t (NetsysNativeServiceStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, ServiceInterface> opToInterfaceMap_;
    void InitBandwidthOpToInterfaceMap();
    void InitFirewallOpToInterfaceMap();
    int32_t CmdEchoServer(MessageParcel &data, MessageParcel &reply);
    int32_t CmdSetResolverConfig(MessageParcel &data, MessageParcel &reply);
    int32_t CmdGetResolverConfig(MessageParcel &data, MessageParcel &reply);
    int32_t CmdCreateNetworkCache(MessageParcel &data, MessageParcel &reply);
    int32_t CmdDestroyNetworkCache(MessageParcel &data, MessageParcel &reply);
    int32_t CmdGetAddrInfo(MessageParcel &data, MessageParcel &reply);
    int32_t CmdSetInterfaceMtu(MessageParcel &data, MessageParcel &reply);
    int32_t CmdGetInterfaceMtu(MessageParcel &data, MessageParcel &reply);

    int32_t CmdRegisterNotifyCallback(MessageParcel &data, MessageParcel &reply);
    int32_t CmdUnRegisterNotifyCallback(MessageParcel &data, MessageParcel &reply);

    int32_t CmdNetworkAddRoute(MessageParcel &data, MessageParcel &reply);
    int32_t CmdNetworkRemoveRoute(MessageParcel &data, MessageParcel &reply);
    int32_t CmdNetworkAddRouteParcel(MessageParcel &data, MessageParcel &reply);
    int32_t CmdNetworkRemoveRouteParcel(MessageParcel &data, MessageParcel &reply);
    int32_t CmdNetworkSetDefault(MessageParcel &data, MessageParcel &reply);
    int32_t CmdNetworkGetDefault(MessageParcel &data, MessageParcel &reply);
    int32_t CmdNetworkClearDefault(MessageParcel &data, MessageParcel &reply);
    int32_t CmdGetProcSysNet(MessageParcel &data, MessageParcel &reply);
    int32_t CmdSetProcSysNet(MessageParcel &data, MessageParcel &reply);
    int32_t CmdNetworkCreatePhysical(MessageParcel &data, MessageParcel &reply);
    int32_t CmdAddInterfaceAddress(MessageParcel &data, MessageParcel &reply);
    int32_t CmdDelInterfaceAddress(MessageParcel &data, MessageParcel &reply);
    int32_t CmdInterfaceSetIpAddress(MessageParcel &data, MessageParcel &reply);
    int32_t CmdInterfaceSetIffUp(MessageParcel &data, MessageParcel &reply);
    int32_t CmdNetworkAddInterface(MessageParcel &data, MessageParcel &reply);
    int32_t CmdNetworkRemoveInterface(MessageParcel &data, MessageParcel &reply);
    int32_t CmdNetworkDestroy(MessageParcel &data, MessageParcel &reply);
    int32_t CmdGetFwmarkForNetwork(MessageParcel &data, MessageParcel &reply);
    int32_t CmdSetInterfaceConfig(MessageParcel &data, MessageParcel &reply);
    int32_t CmdGetInterfaceConfig(MessageParcel &data, MessageParcel &reply);
    int32_t CmdInterfaceGetList(MessageParcel &data, MessageParcel &reply);
    int32_t CmdStartDhcpClient(MessageParcel &data, MessageParcel &reply);
    int32_t CmdStopDhcpClient(MessageParcel &data, MessageParcel &reply);
    int32_t CmdStartDhcpService(MessageParcel &data, MessageParcel &reply);
    int32_t CmdStopDhcpService(MessageParcel &data, MessageParcel &reply);
    int32_t CmdIpEnableForwarding(MessageParcel &data, MessageParcel &reply);
    int32_t CmdIpDisableForwarding(MessageParcel &data, MessageParcel &reply);
    int32_t CmdEnableNat(MessageParcel &data, MessageParcel &reply);
    int32_t CmdDisableNat(MessageParcel &data, MessageParcel &reply);
    int32_t CmdIpfwdAddInterfaceForward(MessageParcel &data, MessageParcel &reply);
    int32_t CmdIpfwdRemoveInterfaceForward(MessageParcel &data, MessageParcel &reply);
    int32_t CmdBandwidthEnableDataSaver(MessageParcel &data, MessageParcel &reply);
    int32_t CmdBandwidthSetIfaceQuota(MessageParcel &data, MessageParcel &reply);
    int32_t CmdBandwidthRemoveIfaceQuota(MessageParcel &data, MessageParcel &reply);
    int32_t CmdBandwidthAddDeniedList(MessageParcel &data, MessageParcel &reply);
    int32_t CmdBandwidthRemoveDeniedList(MessageParcel &data, MessageParcel &reply);
    int32_t CmdBandwidthAddAllowedList(MessageParcel &data, MessageParcel &reply);
    int32_t CmdBandwidthRemoveAllowedList(MessageParcel &data, MessageParcel &reply);
    int32_t CmdFirewallSetUidsAllowedListChain(MessageParcel &data, MessageParcel &reply);
    int32_t CmdFirewallSetUidsDeniedListChain(MessageParcel &data, MessageParcel &reply);
    int32_t CmdFirewallEnableChain(MessageParcel &data, MessageParcel &reply);
    int32_t CmdFirewallSetUidRule(MessageParcel &data, MessageParcel &reply);
    int32_t CmdShareDnsSet(MessageParcel &data, MessageParcel &reply);
    int32_t CmdStartDnsProxyListen(MessageParcel &data, MessageParcel &reply);
    int32_t CmdStopDnsProxyListen(MessageParcel &data, MessageParcel &reply);
    int32_t CmdGetNetworkSharingTraffic(MessageParcel &data, MessageParcel &reply);

private:
    std::vector<int32_t> uids_;
};
} // namespace NetsysNative
} // namespace OHOS
#endif // NETSYS_NATIVE_SERVICE_STUB_H
