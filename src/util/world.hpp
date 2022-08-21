#pragma once

#include <utility>
#include <vector>

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

    inline std::vector<std::pair<int, int>> spiral(const std::pair<int, int>& pos, int size) {
        int n_points = (size * 2 + 1) * (size * 2 + 1);

        std::vector<std::pair<int, int>> points(n_points + 1);
        points.push_back(pos);

        int dx = 1, dz = 0;

        int sl = 1;

        int x = 0, z = 0;
        int sp = 0;
        for (int i = 0; i < n_points; i++) {
            x += dx;
            z += dz;
            sp += 1;

            points.push_back({x + pos.first, z + pos.second});

            if (sp == sl) {
                sp = 0;

                int t = dx;
                dx = -dz;
                dz = t;

                if (dz == 0)
                    sl += 1;
            }
        }

        return points;
    }
};
