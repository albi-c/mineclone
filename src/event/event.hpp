#pragma once

#include <map>
#include <vector>
#include <mutex>
#include <type_traits>
#include <queue>
#include <functional>

struct Event {
    virtual ~Event() {}
};

template <class T>
class EventHandler {
public:
    inline virtual void event_(const Event& event) {
        dynamic_cast<EventHandler<T>*>(this)->event(static_cast<const T&>(event));
    }
    virtual void event(const T& event) =0;
};

template <class T>
class FunctionEventHandler : public EventHandler<T> {
public:
    inline FunctionEventHandler(std::function<void(T)> callback)
        : callback(callback) {}

    inline virtual void event(const T& event) override {
        callback(event);
    }

private:
    std::function<void(T)> callback;
};

template <class T>
class EventQueue : public EventHandler<T> {
public:
    inline virtual void event(const T& event) override {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(event);
    }

    inline T pop() {
        std::lock_guard<std::mutex> lock(mutex);
        T& value = queue.front();
        queue.pop();
        return value;
    }
    inline bool empty() {
        std::lock_guard<std::mutex> lock(mutex);
        return queue.empty();
    }

private:
    std::queue<T> queue;
    std::mutex mutex;
};

template <class T>
class FunctionEventQueue : public EventQueue<T> {
public:
    inline FunctionEventQueue(std::function<void(const T&)> callback)
        : callback(callback) {}
    
    inline void process() {
        while (!this->empty())
            callback(this->pop());
    }

    inline void process_one() {
        if (!this->empty())
            callback(this->pop());
    }

private:
    std::function<void(const T&)> callback;
};

class EventManager {
public:
    template <class T>
    inline static void fire(const T& event) {
        size_t type = typeid(T).hash_code();
        std::lock_guard<std::mutex> lock(EventManager::_mutex(type));
        for (auto& handler : EventManager::_handlers(type)) {
            handler(event);
        }
    }

    template <class T>
    inline static void listen(EventHandler<T>& handler) {
        size_t type = typeid(T).hash_code();
        std::lock_guard<std::mutex> lock(EventManager::_mutex(type));
        EventManager::_handlers(type).push_back(std::function([&handler](const Event& event) { handler.event_(event); }));
    }

private:
    static std::vector<std::function<void(const Event&)>>& _handlers(std::size_t id);
    static std::mutex& _mutex(std::size_t id);
};
