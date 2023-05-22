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

#include <stdint.h>
#include <linux/bpf.h>

#define SEC(NAME) __attribute__((section(NAME), used))

static SEC("xlat/egress/ipv4/raw") int bpf_xlat_egress_ipv4_raw(struct __sk_buff *skb) { return 0; }

static SEC("xlat/ingress/ipv6/ether") int bpf_xlat_ingress_ipv6_ether(struct __sk_buff *skb) { return 0; }

static SEC("xlat/ingress/ipv6/raw") int bpf_xlat_ingress_ipv6_raw(struct __sk_buff *skb) { return 0; }

char _license[] SEC("license") = "GPL";