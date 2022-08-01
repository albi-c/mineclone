#pragma once

#include <unordered_set>

#define BETTER_ENUMS_MACRO_FILE "../lib/enum_macros.h"
#include "../lib/enum.h"

BETTER_ENUM(Material, int,
    AIR = 0,
    STONE,
    DIRT,
    GRASS_BLOCK,
    LOG,
    LEAVES,
    SAND,
    PLANKS,
    GRASS,
    FLOWER,
    DEAD_BUSH
);
BETTER_ENUMS_DECLARE_STD_HASH(Material);

namespace MaterialGroup {
    const extern std::unordered_set<Material> transparent;
    const extern std::unordered_set<Material> plant;
};
