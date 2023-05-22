/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef NET_MANAGER_TIMER_H
#define NET_MANAGER_TIMER_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>

#include "net_mgr_log_wrapper.h"

namespace OHOS {
namespace NetManagerStandard {
static constexpr const int TIMER_MAX_INTERVAL_MS = 200;
class Timer {
public:
    Timer() : stopStatus_(true), tryStopFlag_(false) {}

    Timer(const Timer &timer)
    {
        stopStatus_ = timer.stopStatus_.load();
        tryStopFlag_ = timer.tryStopFlag_.load();
    }

    ~Timer()
    {
        Stop();
    }

    void Start(int interval, std::function<void()> taskFun)
    {
        if (stopStatus_ == false) {
            return;
        }
        NETMGR_LOG_D("start thread...");
        stopStatus_ = false;
        std::thread([this, interval, taskFun]() {
            while (!tryStopFlag_) {
                OneTiming(interval);
                if (!tryStopFlag_) {
                    taskFun();
                }
            }

            std::lock_guard<std::mutex> locker(mutex_);
            stopStatus_ = true;
            timerCond_.notify_one();
        }).detach();
    }

    void Stop()
    {
        if (stopStatus_ || tryStopFlag_) {
            return;
        }
        NETMGR_LOG_D("stop thread...");
        tryStopFlag_ = true;
        std::unique_lock<std::mutex> locker(mutex_);
        timerCond_.wait(locker, [this] { return stopStatus_ == true; });

        if (stopStatus_ == true) {
            tryStopFlag_ = false;
        }
    }

private:
    void OneTiming(int time)
    {
        int repeatCount = (time > TIMER_MAX_INTERVAL_MS) ? (time / TIMER_MAX_INTERVAL_MS) : 0;
        int remainTime = (time > TIMER_MAX_INTERVAL_MS) ? (time % TIMER_MAX_INTERVAL_MS) : time;
        while (!tryStopFlag_) {
            if (repeatCount > 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(TIMER_MAX_INTERVAL_MS));
            } else {
                if (remainTime) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(remainTime));
                    remainTime = 0;
                }
                break;
            }
            repeatCount--;
        }
    }

private:
    std::atomic<bool> stopStatus_;
    std::atomic<bool> tryStopFlag_;
    std::mutex mutex_;
    std::condition_variable timerCond_;
};
} // namespace NetManagerStandard
} // namespace OHOS
#endif // NET_MANAGER_TIMER_H
