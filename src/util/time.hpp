#pragma once

#include <GLFW/glfw3.h>
#include <iostream>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>

#define ENABLE_TIMER 1

class Timer {
public:
    inline Timer(const std::string& name = "unnamed")
        : name(name), startTime(glfwGetTime()) {}
    
    inline ~Timer() {
        if (!ended) {
            stop();
        }
    }

    inline void stop() {
        #if ENABLE_TIMER

        ended = true;
        double endTime = glfwGetTime();
        double time = endTime - startTime;

        mutex.lock();

        auto& timeData = times[name];
        timeData.total += time;
        timeData.count++;
        timeData.max = std::max(timeData.max, time);
        if (time > timeData.max * 0.5) {
            timeData.above_med++;
        }

        mutex.unlock();

        #endif
    }

    inline static void end() {
        #if ENABLE_TIMER

        mutex.lock();

        for (auto& [name, data] : times) {
            std::cout << "TIMER [" << name << "] | avg: " << (data.total / (double) data.count) << " | total: " <<
                      data.total << " | count: " << data.count << " | max: " << data.max << " | above: " << data.above_med << "\n";
        }

        mutex.unlock();

        #endif
    }

private:
    struct TimeData {
        double total = 0.0;
        size_t count = 0;
        double max = 0.0;
        size_t above_med = 0;
    };

    std::string name;
    bool ended = false;
    double startTime;

    inline static std::mutex mutex;
    inline static std::unordered_map<std::string, TimeData> times;
};
