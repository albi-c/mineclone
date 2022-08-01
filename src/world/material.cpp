#include "material.hpp"

namespace MaterialGroup {
    const std::unordered_set<Material> transparent = {
        Material::AIR,
        Material::LEAVES,
        Material::FLOWER,
        Material::DEAD_BUSH,
        Material::GRASS
    };

    const std::unordered_set<Material> plant = {
        Material::FLOWER,
        Material::DEAD_BUSH,
        Material::GRASS
    };
};
