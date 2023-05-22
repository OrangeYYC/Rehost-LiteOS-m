/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef NET_POLICY_FILE_H
#define NET_POLICY_FILE_H

#include <climits>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>

#include <json/json.h>

#include "singleton.h"

#include "netmanager_base_common_utils.h"
#include "net_policy_constants.h"
#include "net_policy_inner_define.h"
#include "net_quota_policy.h"

namespace OHOS {
namespace NetManagerStandard {
enum NetUidPolicyOpType {
    NET_POLICY_UID_OP_TYPE_DO_NOTHING = 0,
    NET_POLICY_UID_OP_TYPE_ADD = 1,
    NET_POLICY_UID_OP_TYPE_DELETE = 2,
    NET_POLICY_UID_OP_TYPE_UPDATE = 3,
};

class NetPolicyFile : public std::enable_shared_from_this<NetPolicyFile> {
    DECLARE_DELAYED_SINGLETON(NetPolicyFile);

public:
    /**
     * Init by reading policy from file.
     * @return true Return true means init policy successful.
     * @return false Return false means init policy failed.
     */
    bool InitPolicy();

    /**
     * Judge if this uid is exist.
     * @param uid The specified UID of application.
     * @return true Return true means this uid is exist.
     * @return false Return false means this uid is not exist.
     */
    bool IsUidPolicyExist(uint32_t uid);

    /**
     * Write struct vector of quotaPolicies to file.
     * @param quotaPolicies The struct vector of quotaPolicies.
     * @return true Return true means write quotaPolicies to file successful.
     * @return false Return false means write quotaPolicies to file failed.
     */
    bool WriteFile(const std::vector<NetQuotaPolicy> &quotaPolicies);

    /**
     * Write uid and policy to file.
     * @param uid The specified UID of application.
     * @param policy See {@link NetUidPolicy}.
     */
    void WriteFile(uint32_t uid, uint32_t policy);

    /**
     * Get policy by uid from vector UidPolicy of struct NetPolicy.
     * @param uid The specified UID of application.
     * @return NetUidPolicy See {@link NetUidPolicy}.
     */
    NetUidPolicy GetPolicyByUid(uint32_t uid);

    /**
     * Get uids by policy from vector UidPolicy of struct NetPolicy.
     * @param policy See {@link NetUidPolicy}.
     * @param uids The specified UIDS of application.
     * @return true Return true means get uids that policy equal input policy.
     * @return false Return false means get none uids that policy equal input policy.
     */
    bool GetUidsByPolicy(uint32_t policy, std::vector<uint32_t> &uids);

    /**
     * Add quota policies into struct vector quotaPolicies.
     * @param quotaPolicies The struct vector of quotaPolicies.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t ReadQuotaPolicies(std::vector<NetQuotaPolicy> &quotaPolicies);

    /**
     * Use netType and iccid to judge if this quota Policy in NetPolicyQuota vector of struct NetPolicy.
     * @param netType For details, see {@link NetBearType}.
     * @param iccid The string type of iccid.
     * @param quotaPolicy The struct vector of quotaPolicies.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t GetNetQuotaPolicy(int32_t netType, const std::string &iccid, NetQuotaPolicy &quotaPolicy);

    /**
     * Clear the uid and policy in struct NetPolicy's uidPolicy vector,
     * reset the background policy status to default,
     * reset the netQuotaPolicy which IccId equal iccid,
     * write these changes to file.
     * @param iccid The net quota policy's sim id.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t ResetPolicies(const std::string &iccid);

    /**
     * Write the background policy to file.
     * @param allowBackground When the allowBackground is true,it means "allow" background policy,
     * when the allowBackground is false,if means "reject" background policy.
     * @return int32_t Returns 0 success. Otherwise fail, {@link NetPolicyResultCode}.
     */
    int32_t SetBackgroundPolicy(bool allowBackground);

    /**
     * Get background policy from file.
     * @return true True means allow background policy.
     * @return false False means reject background policy.
     */
    bool GetBackgroundPolicy();

    /**
     * Get struct vector uid and policy from file.
     * @return const std::vector<UidPolicy>& Return struct vector netPolicy_.uidPolicies.
     */
    const std::vector<UidPolicy> &GetNetPolicies();

private:
    bool FileExists(const std::string &fileName);
    bool CreateFile(const std::string &fileName);
    bool WriteFile(const std::string &fileName);
    bool WriteFile(uint32_t netUidPolicyOpType, uint32_t uid, uint32_t policy);
    bool ReadFile(const std::string &fileName, std::string &content);
    bool Json2Obj(const std::string &content, NetPolicy &netPolicy);
    void AppendUidPolicy(Json::Value &root);
    void AppendBackgroundPolicy(Json::Value &root);
    void AppendQuotaPolicy(Json::Value &root);
    void ParseUidPolicy(const Json::Value &root, NetPolicy &netPolicy);
    void ParseBackgroundPolicy(const Json::Value &root, NetPolicy &netPolicy);
    void ParseQuotaPolicy(const Json::Value &root, NetPolicy &netPolicy);
    void ParseCellularPolicy(const Json::Value &root, NetPolicy &netPolicy);
    bool UpdateQuotaPolicyExist(const NetQuotaPolicy &quotaPolicy);
    uint32_t ArbitrationWritePolicyToFile(uint32_t uid, uint32_t policy);
    inline void ToQuotaPolicy(const NetPolicyQuota& netPolicyQuota, NetQuotaPolicy &quotaPolicy)
    {
        quotaPolicy.lastLimitRemind = CommonUtils::StrToLong(netPolicyQuota.lastLimitSnooze, REMIND_NEVER);
        quotaPolicy.limitBytes = CommonUtils::StrToLong(netPolicyQuota.limitBytes, DATA_USAGE_UNKNOWN);
        quotaPolicy.metered = CommonUtils::StrToBool(netPolicyQuota.metered, false);
        quotaPolicy.netType = CommonUtils::StrToInt(netPolicyQuota.netType, BEARER_DEFAULT);
        quotaPolicy.periodDuration = netPolicyQuota.periodDuration;
        quotaPolicy.periodStartTime = CommonUtils::StrToLong(netPolicyQuota.periodStartTime);
        quotaPolicy.source = CommonUtils::StrToInt(netPolicyQuota.source);
        quotaPolicy.iccid = netPolicyQuota.iccid;
        quotaPolicy.warningBytes = CommonUtils::StrToLong(netPolicyQuota.warningBytes, DATA_USAGE_UNKNOWN);
    }

private:
    NetPolicy netPolicy_;
    std::mutex mutex_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_POLICY_FILE_H
