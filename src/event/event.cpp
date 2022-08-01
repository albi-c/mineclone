#include "event.hpp"

std::map<EventType, std::vector<void(*)(const Event&)>> Event::callbacks;

void Event::fire(const Event& event) {
    for (auto& callback : Event::callbacks[event.type]) {
        callback(event);
    }
}

void Event::listen(EventType type, void(*callback)(const Event&)) {
    Event::callbacks[type].push_back(callback);
}
