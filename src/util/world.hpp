#pragma once

#include <utility>

#include "lib/opengl.hpp"

namespace wu {
    inline std::pair<int, int> chunk_pos(int x, int z) {
        return {
            x < 0 ? -(::abs(x) >> 4) : x >> 4,
            z < 0 ? -(::abs(z) >> 4) : z >> 4
        };
    }
    inline std::pair<int, int> chunk_pos(const std::pair<int, int>& block_pos) {
        return chunk_pos(block_pos.first, block_pos.second);
    }
};
