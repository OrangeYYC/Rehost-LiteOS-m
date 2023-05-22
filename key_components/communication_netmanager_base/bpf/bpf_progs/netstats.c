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

static const int APP_STATS_MAP_SIZE = 5000;
static const int IFACE_STATS_MAP_SIZE = 1000;
static const int IFACE_NAM_MAP_SIZE = 1000;
static const int IFNAM_SIZE = 16;

typedef struct {
    unsigned int type; // actual bpf_map_type
    unsigned int key_size;
    unsigned int value_size;
    unsigned int max_entries;
    unsigned int map_flags;
    unsigned int inner_map_idx;
    unsigned int numa_node;
} bpf_map_def;

typedef struct {
    uint64_t rx_packets;
    uint64_t rx_bytes;
    uint64_t tx_packets;
    uint64_t tx_bytes;
} stats_value;

typedef struct {
    char name[IFNAM_SIZE];
} iface_name;

bpf_map_def SEC("maps") iface_stats_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(uint32_t),
    .value_size = sizeof(stats_value),
    .max_entries = IFACE_STATS_MAP_SIZE,
    .map_flags = 0,
    .inner_map_idx = 0,
    .numa_node = 0,
};

bpf_map_def SEC("maps") iface_name_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(uint32_t),
    .value_size = sizeof(iface_name),
    .max_entries = IFACE_NAM_MAP_SIZE,
    .map_flags = 0,
    .inner_map_idx = 0,
    .numa_node = 0,
};

bpf_map_def SEC("maps") app_uid_stats_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(uint32_t),
    .value_size = sizeof(stats_value),
    .max_entries = APP_STATS_MAP_SIZE,
    .map_flags = 0,
    .inner_map_idx = 0,
    .numa_node = 0,
};

static SEC("cgroup_skb/uid/ingress") int bpf_cgroup_skb_uid_ingress(struct __sk_buff *skb)
{
    uint32_t sock_uid = bpf_get_socket_uid(skb);

    stats_value *value = bpf_map_lookup_elem(&app_uid_stats_map, &sock_uid);
    if (!value) {
        stats_value newValue = {};
        bpf_map_update_elem(&app_uid_stats_map, &sock_uid, &newValue, BPF_NOEXIST);
        value = bpf_map_lookup_elem(&app_uid_stats_map, &sock_uid);
    }

    if (value) {
        __sync_fetch_and_add(&value->rx_packets, 1);
        __sync_fetch_and_add(&value->rx_bytes, skb->len);

        const char log[] = "[Uid ingress] sock_uid = %d, value->rx_packets = %d, value->rx_bytes = %d";
        bpf_trace_printk(log, sizeof(log), sock_uid, value->rx_packets, value->rx_bytes);
    }
    return 1;
}

static SEC("cgroup_skb/uid/egress") int bpf_cgroup_skb_uid_egress(struct __sk_buff *skb)
{
    uint32_t sock_uid = bpf_get_socket_uid(skb);

    stats_value *value = bpf_map_lookup_elem(&app_uid_stats_map, &sock_uid);
    if (!value) {
        stats_value newValue = {};
        bpf_map_update_elem(&app_uid_stats_map, &sock_uid, &newValue, BPF_NOEXIST);
        value = bpf_map_lookup_elem(&app_uid_stats_map, &sock_uid);
    }

    if (value) {
        __sync_fetch_and_add(&value->tx_packets, 1);
        __sync_fetch_and_add(&value->tx_bytes, skb->len);

        const char log[] = "[Uid egress] sock_uid = %d, value->tx_packets = %d, value->tx_bytes = %d";
        bpf_trace_printk(log, sizeof(log), sock_uid, value->tx_packets, value->tx_bytes);
    }
    return 1;
}

static SEC("socket/iface/ingress") void bpf_socket_iface_ingress(struct __sk_buff *skb)
{
    uint32_t key = skb->ifindex;

    stats_value *value = bpf_map_lookup_elem(&iface_stats_map, &key);
    if (!value) {
        stats_value newValue = {};
        bpf_map_update_elem(&iface_stats_map, &key, &newValue, BPF_NOEXIST);
        value = bpf_map_lookup_elem(&iface_stats_map, &key);
    }

    if (value) {
        __sync_fetch_and_add(&value->rx_packets, 1);
        __sync_fetch_and_add(&value->rx_bytes, skb->len);

        const char log[] = "[Iface ingress] ifindex = %d, value->rx_packets = %d, value->rx_bytes = %d";
        bpf_trace_printk(log, sizeof(log), key, value->rx_packets, value->rx_bytes);
    }
}

static SEC("socket/iface/egress") void bpf_socket_iface_egress(struct __sk_buff *skb)
{
    uint32_t key = skb->ifindex;

    stats_value *value = bpf_map_lookup_elem(&iface_stats_map, &key);
    if (!value) {
        stats_value newValue = {};
        bpf_map_update_elem(&iface_stats_map, &key, &newValue, BPF_NOEXIST);
        value = bpf_map_lookup_elem(&iface_stats_map, &key);
    }

    if (value) {
        __sync_fetch_and_add(&value->tx_packets, 1);
        __sync_fetch_and_add(&value->tx_bytes, skb->len);

        const char log[] = "[Iface egress] ifindex = %d, value->tx_packets = %d, value->tx_bytes = %d";
        bpf_trace_printk(log, sizeof(log), key, value->tx_packets, value->tx_bytes);
    }
}

char _license[] SEC("license") = "GPL";