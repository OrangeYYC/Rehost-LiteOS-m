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

#ifndef NETMANAGER_BASE_LRU_CACHE_H
#define NETMANAGER_BASE_LRU_CACHE_H

#include <list>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace OHOS::NetManagerStandard {
static constexpr const size_t DEFAULT_CAPABILITY = 100;
template <typename T> class LRUCache {
public:
    LRUCache() : capacity_(DEFAULT_CAPABILITY) {}

    std::vector<T> Get(const std::string &key)
    {
        std::lock_guard<std::mutex> guard(mutex_);

        if (cache_.find(key) == cache_.end()) {
            return {};
        }
        auto it = cache_[key];
        auto value = it->value;
        MoveNodeToHead(it);
        return value;
    }

    void Put(const std::string &key, const T &value)
    {
        std::lock_guard<std::mutex> guard(mutex_);

        if (cache_.find(key) == cache_.end()) {
            AddNode(Node(key, {value}));
            if (cache_.size() > capacity_) {
                EraseTailNode();
            }
            return;
        }

        auto it = cache_[key];
        it->value.emplace_back(value);
        MoveNodeToHead(it);
        if (cache_.size() > capacity_) {
            EraseTailNode();
        }
    }

    void Delete(const std::string &key)
    {
        std::lock_guard<std::mutex> guard(mutex_);

        if (cache_.find(key) == cache_.end()) {
            return;
        }

        auto it = cache_[key];
        cache_.erase(key);
        nodeList_.erase(it);
    }

    void Clear()
    {
        std::lock_guard<std::mutex> guard(mutex_);

        cache_.clear();
        nodeList_.clear();
    }

private:
    struct Node {
        std::string key;
        std::vector<T> value;

        Node() = delete;

        Node(std::string key, std::vector<T> value) : key(std::move(key)), value(std::move(value)) {}
    };

    void AddNode(const Node &node)
    {
        nodeList_.emplace_front(node);
        cache_[node.key] = nodeList_.begin();
    }

    void MoveNodeToHead(const typename std::list<Node>::iterator &it)
    {
        std::string key = it->key;
        auto value = it->value;
        nodeList_.erase(it);
        nodeList_.emplace_front(key, value);
        cache_[key] = nodeList_.begin();
    }

    void EraseTailNode()
    {
        if (nodeList_.empty()) {
            return;
        }
        Node node = nodeList_.back();
        nodeList_.pop_back();
        cache_.erase(node.key);
    }

    std::mutex mutex_;
    std::unordered_map<std::string, typename std::list<Node>::iterator> cache_;
    std::list<Node> nodeList_;
    size_t capacity_;
};
} // namespace OHOS::NetManagerStandard
#endif // NETMANAGER_BASE_LRU_CACHE_H
