#pragma once

#include <map>
#include <string>

#include <iostream>

#include "event.hpp"

class Options {
public:
    inline static bool shadows = true;

    static inline int get(const std::string& name) {
        return options[name];
    }
    static inline void set(const std::string& name, int value) {
        options[name] = value;
        EventManager::fire(EventOptionChange{name, value});
    }

private:
    static inline std::map<std::string, int> options = {
        {"shadows", true},
        {"render_distance", 8},
        {"chunk_updates", 8}
    };
};

template <class T = int>
class Option {
public:
    inline Option(const std::string& name)
        : name(name), value((T)Options::get(name)), original_value(value) {}
    
    inline ~Option() {
        if (value != original_value)
            Options::set(name, (int)value);
    }
    
    inline T* operator&() {
        return &value;
    }

private:
    std::string name;
    T value, original_value;
};
