#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

namespace su {
    inline std::vector<std::string> split(const std::string& str, char delim, int count = -1) {
        std::vector<std::string> elems;

        std::istringstream iss(str);
        std::string item;
        std::string elem;
        int counter = 0;
        while (std::getline(iss, item, delim)) {
            if (count == -1 || counter < count)
                elems.push_back(item);
            else
                elem += item + delim;

            counter++;
        }

        if (!elem.empty())
            elems.push_back(elem.substr(0, elem.size() - 1));
        
        return elems;
    }

    inline std::string replace(std::string str, const std::string& from, const std::string& to, int count = -1) {
        if (from.empty())
            return str;
        
        size_t start_pos = 0;
        int counter = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos && (count == -1 || counter < count)) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
            counter++;
        }

        return str;
    }

    inline std::string to_lower(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }
    inline std::string to_upper(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        return str;
    }

    inline bool starts_with(const std::string& str, const std::string& with) {
        return str.rfind(with, 0) == 0;
    }
    inline bool ends_with(const std::string& str, const std::string& with) {
        return str.length() >= with.length() && !str.compare(str.length() - with.length(), with.length(), with);
    }
};
