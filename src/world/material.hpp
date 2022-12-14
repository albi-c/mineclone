#pragma once

#include <unordered_set>

#ifndef BETTER_ENUMS_MACRO_FILE
#define BETTER_ENUMS_MACRO_FILE "lib/enum_macros.h"
#endif
#include "lib/enum.h"

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
    DEAD_BUSH,
    GLASS,
    SNOW,
    BEDROCK
);
BETTER_ENUMS_DECLARE_STD_HASH(Material);

BETTER_ENUM(MaterialProperty, int,
    TRANSPARENT = 0x1,
    PLANT = 0x2,
    WAVING = 0x4
);

extern int MATERIAL_PROPERTIES[Material::_size()];

class MaterialPropertiesInitializer {
public:
    MaterialPropertiesInitializer();
};
