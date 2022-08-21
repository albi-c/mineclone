#pragma once

#include <mutex>
#include <thread>

#include <GLFW/glfw3.h>

namespace tu {
    inline bool mutex_lock_timeout(std::mutex& mutex, double timeout = 10.0) {
        double time = glfwGetTime();

        while (!mutex.try_lock()) {
            if (glfwGetTime() - time > timeout * 1000.0) {
                return false;
            }
        }

        return true;
    }

    inline void mutex_lock_timeout_exc(std::mutex& mutex, double timeout = 10.0) {
        if (!mutex_lock_timeout(mutex, timeout))
            throw std::runtime_error("mutex_lock_timeout_exc");
    }
};
