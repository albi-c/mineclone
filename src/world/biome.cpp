#include "biome.hpp"

const Biome BiomeTable::table[BIOME_TABLE_SIZE][BIOME_TABLE_SIZE] = {
    {Biome::MOUNTAINS, Biome::MOUNTAINS, Biome::MOUNTAINS, Biome::MOUNTAINS, Biome::MOUNTAINS, Biome::MOUNTAINS, Biome::SNOWY_PLAINS, Biome::SNOWY_PLAINS},
    {Biome::PLAINS, Biome::PLAINS, Biome::PLAINS, Biome::MOUNTAINS, Biome::MOUNTAINS, Biome::MOUNTAINS, Biome::SNOWY_PLAINS, Biome::SNOWY_PLAINS},
    {Biome::PLAINS, Biome::PLAINS, Biome::FOREST, Biome::PLAINS, Biome::PLAINS, Biome::PLAINS, Biome::PLAINS, Biome::SNOWY_PLAINS},
    {Biome::PLAINS, Biome::FOREST, Biome::FOREST, Biome::FOREST, Biome::FOREST, Biome::FOREST, Biome::FOREST, Biome::FOREST},
    {Biome::DESERT, Biome::FOREST, Biome::FOREST, Biome::FOREST, Biome::FOREST, Biome::FOREST, Biome::FOREST, Biome::FOREST},
    {Biome::DESERT, Biome::DESERT, Biome::PLAINS, Biome::FOREST, Biome::FOREST, Biome::FOREST, Biome::FOREST, Biome::FOREST},
    {Biome::DESERT, Biome::DESERT, Biome::DESERT, Biome::PLAINS, Biome::PLAINS, Biome::PLAINS, Biome::FOREST, Biome::FOREST},
    {Biome::DESERT, Biome::DESERT, Biome::DESERT, Biome::DESERT, Biome::PLAINS, Biome::PLAINS, Biome::PLAINS, Biome::PLAINS}
};

Biome BiomeTable::get(float temperature, float humidity) {
    return table[(int)(temperature * BIOME_TABLE_SIZE - 1)][(int)(humidity * BIOME_TABLE_SIZE - 1)];
}
