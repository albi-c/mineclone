#include "event.hpp"

static std::map<std::size_t, std::vector<std::function<void(const Event&)>>> EventManager_handlers;
static std::map<std::size_t, std::mutex> EventManager_mutexes;

std::vector<std::function<void(const Event&)>>& EventManager::_handlers(std::size_t id) {
    return EventManager_handlers[id];
}
std::mutex& EventManager::_mutex(std::size_t id) {
    return EventManager_mutexes[id];
}
