#pragma once

#include <unordered_set>

#ifndef BETTER_ENUMS_MACRO_FILE
#define BETTER_ENUMS_MACRO_FILE "lib/enum_macros.h"
#endif
#include "lib/enum.h"

BETTER_ENUM(Biome, int,
    FOREST = 0,
    PLAINS,
    MOUNTAINS,
    DESERT
);
BETTER_ENUMS_DECLARE_STD_HASH(Biome);
