#pragma once

#include <utility>

#include "lib/opengl.hpp"

namespace wu {
    inline std::pair<int, int> chunk_pos(int x, int z) {
        return {
            x < 0 ? -(x & 0xf) : x & 0xf,
            z < 0 ? -(z & 0xf) : z & 0xf
        };
    }
    inline std::pair<int, int> chunk_pos(const std::pair<int, int>& block_pos) {
        return chunk_pos(block_pos.first, block_pos.second);
    }
};
