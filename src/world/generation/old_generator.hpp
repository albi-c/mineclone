#pragma once

#include "world/generation/generator.hpp"

class OldWorldGenerator : public WorldGenerator {
public:
    OldWorldGenerator(int seed);

    std::shared_ptr<Chunk> generate(glm::ivec2 pos) override;

private:
    void generate_biomes(Biome* biomes, glm::ivec2 pos);
};
