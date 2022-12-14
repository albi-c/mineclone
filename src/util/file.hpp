#pragma once

#include <string>
#include <fstream>

namespace fu {
    inline bool exists(const std::string& filename) {
        std::ifstream file(filename);
        return file.good();
    }
};
