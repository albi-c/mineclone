#pragma once

#include <array>

#ifndef BETTER_ENUMS_MACRO_FILE
#define BETTER_ENUMS_MACRO_FILE "enum_macros.h"
#endif
#include "enum.h"

#define BIOME_TABLE_SIZE 8

BETTER_ENUM(Biome, int,
    PLAINS = 0,
    FOREST,
    MOUNTAINS,
    SNOWY_PLAINS,
    DESERT
);
BETTER_ENUMS_DECLARE_STD_HASH(Biome);

class BiomeTable {
public:
    static Biome get(float temperature, float humidity);

private:
    static const Biome table[BIOME_TABLE_SIZE][BIOME_TABLE_SIZE];
};
