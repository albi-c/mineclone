#pragma once

#include <unordered_set>

#define BETTER_ENUMS_MACRO_FILE "../lib/enum_macros.h"
#include "../lib/enum.h"

BETTER_ENUM(Biome, int,
    FOREST = 0,
    PLAINS,
    DESERT
);
BETTER_ENUMS_DECLARE_STD_HASH(Biome);
