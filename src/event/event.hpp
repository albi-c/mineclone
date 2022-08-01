#pragma once

#include <map>
#include <vector>

#define BETTER_ENUMS_MACRO_FILE "../lib/enum_macros.h"
#include "../lib/enum.h"

BETTER_ENUM(EventType, int,
    MouseClick = 0,
    ChunkRedraw
);
BETTER_ENUMS_DECLARE_STD_HASH(EventType);

class Event {
public:
    EventType type;
    void* data;

    static void fire(const Event& event);
    inline void fire() const { Event::fire(*this); }

    static void listen(EventType type, void(*callback)(const Event&));

    inline bool operator==(const Event& other) const { return type == other.type && data == other.data; }
    inline bool operator==(EventType other) const { return type == other; }
    inline bool operator==(EventType::_enumerated other) const { return type == (EventType)other; }

private:
    static std::map<EventType, std::vector<void(*)(const Event&)>> callbacks;
};
