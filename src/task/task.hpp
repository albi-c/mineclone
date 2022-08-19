#pragma once

#include <mutex>
#include <functional>
#include <queue>
#include <map>
#include <thread>
#include <memory>
#include <chrono>

template <class R>
class Task;

class TaskScheduler {
public:
    inline static void init(int threads) {
        for (int i = 0; i < threads; i++) {
            running = true;
            TaskScheduler::threads[i] = new std::thread([]() {
                while (running) {
                    mutex.lock();
                    if (!tasks.empty()) {
                        auto task = tasks.back();
                        tasks.pop();
                        mutex.unlock();

                        task();
                    } else {
                        mutex.unlock();
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    }
                }
            });
        }
    }

    inline static void destroy() {
        for (auto& [id, thread] : threads) {
            thread->join();
            delete thread;
        }
    }

    inline static void execute(std::function<void()> task) {
        std::lock_guard<std::mutex> lock(mutex);
        tasks.push(task);
    }

private:
    inline static std::queue<std::function<void()>> tasks;
    inline static std::mutex mutex;
    inline static std::map<int, std::thread*> threads;
    inline static bool running;
};

template <class R>
class Task {
public:
    inline Task(std::function<R()> task)
        : task(task) {
        
        TaskScheduler::execute([&]() {
            execute();
        });
    }
    
    inline void execute() {
        std::lock_guard<std::mutex> lock(mutex);
        result_ = task();
    }

    inline bool finished() {
        std::lock_guard<std::mutex> lock(mutex);
        return finished_;
    }

    inline R result() {
        std::lock_guard<std::mutex> lock(mutex);
        return result_;
    }

private:
    std::function<R()> task;
    std::mutex mutex;
    R result_;
    bool finished_;
};
