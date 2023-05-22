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

#ifndef NETSTACK_THREAD_POOL
#define NETSTACK_THREAD_POOL

#include <atomic>
#include <condition_variable>
#include <queue>
#include <thread>
#include <vector>

namespace OHOS::NetStack {
template <typename Task, const size_t DEFAULT_THREAD_NUM, const size_t MAX_THREAD_NUM> class ThreadPool {
public:
    /**
     * disallow default constructor
     */
    ThreadPool() = delete;

    /**
     * disallow copy and move
     */
    ThreadPool(const ThreadPool &) = delete;

    /**
     * disallow copy and move
     */
    ThreadPool &operator=(const ThreadPool &) = delete;

    /**
     * disallow copy and move
     */
    ThreadPool(ThreadPool &&) = delete;

    /**
     * disallow copy and move
     */
    ThreadPool &operator=(ThreadPool &&) = delete;

    /**
     * make DEFAULT_THREAD_NUM threads
     * @param timeout if timeout and runningThreadNum_ < DEFAULT_THREAD_NUM, the running thread should be terminated
     */
    explicit ThreadPool(uint32_t timeout) : timeout_(timeout), idleThreadNum_(0), needRun_(true)
    {
        for (int i = 0; i < DEFAULT_THREAD_NUM; ++i) {
            std::thread([this] { RunTask(); }).detach();
        }
    }

    /**
     * if ~ThreadPool, terminate all thread
     */
    ~ThreadPool()
    {
        // set needRun_ = false, and notify all the thread to wake and terminate
        needRun_ = false;
        while (runningNum_ > 0) {
            needRunCondition_.notify_all();
        }
    }

    /**
     * push it to taskQueue_ and notify a thread to run it
     * @param task new task to Execute
     */
    void Push(const Task &task)
    {
        PushTask(task);

        if (runningNum_ < MAX_THREAD_NUM && idleThreadNum_ == 0) {
            std::thread([this] { RunTask(); }).detach();
        }

        needRunCondition_.notify_all();
    }

private:
    bool IsQueueEmpty()
    {
        std::lock_guard<std::mutex> guard(mutex_);
        return taskQueue_.empty();
    }

    bool GetTask(Task &task)
    {
        std::lock_guard<std::mutex> guard(mutex_);

        // if taskQueue_ is empty, means timeout
        if (taskQueue_.empty()) {
            return false;
        }

        // if run to this line, means that taskQueue_ is not empty
        task = taskQueue_.top();
        taskQueue_.pop();
        return true;
    }

    void PushTask(const Task &task)
    {
        std::lock_guard<std::mutex> guard(mutex_);
        taskQueue_.push(task);
    }

    class NumWrapper {
    public:
        NumWrapper() = delete;

        explicit NumWrapper(std::atomic<uint32_t> &num) : num_(num)
        {
            ++num_;
        }

        ~NumWrapper()
        {
            --num_;
        }

    private:
        std::atomic<uint32_t> &num_;
    };

    void Sleep()
    {
        std::mutex needRunMutex;
        std::unique_lock<std::mutex> lock(needRunMutex);

        /**
         * if the thread is waiting, it is idle
         * if wake up, this thread is not idle:
         *     1 this thread should return
         *     2 this thread should run task
         *     3 this thread should go to next loop
         */
        NumWrapper idleWrapper(idleThreadNum_);
        (void)idleWrapper;

        needRunCondition_.wait_for(lock, std::chrono::seconds(timeout_),
                                   [this] { return !needRun_ || !IsQueueEmpty(); });
    }

    void RunTask()
    {
        NumWrapper runningWrapper(runningNum_);
        (void)runningWrapper;

        while (needRun_) {
            Task task;
            if (GetTask(task)) {
                task.Execute();
                continue;
            }

            Sleep();

            if (!needRun_) {
                return;
            }

            if (GetTask(task)) {
                task.Execute();
                continue;
            }

            if (runningNum_ > DEFAULT_THREAD_NUM) {
                return;
            }
        }
    }

private:
    /**
     * other thread put a task to the taskQueue_
     */
    std::mutex mutex_;
    std::priority_queue<Task> taskQueue_;
    /**
     * 1 terminate the thread if it is idle for timeout_ seconds
     * 2 wait for the thread started util timeout_
     * 3 wait for the thread notified util timeout_
     * 4 wait for the thread terminated util timeout_
     */
    uint32_t timeout_;
    /**
     * if idleThreadNum_ is zero, make a new thread
     */
    std::atomic<uint32_t> idleThreadNum_;
    /**
     * when ThreadPool object is deleted, wait until runningNum_ is zero.
     */
    std::atomic<uint32_t> runningNum_;
    /**
     * when ThreadPool object is deleted, set needRun_ to false, mean that all thread should be terminated
     */
    std::atomic_bool needRun_;
    std::condition_variable needRunCondition_;
};
} // namespace OHOS::NetStack
#endif /* NETSTACK_THREAD_POOL */
