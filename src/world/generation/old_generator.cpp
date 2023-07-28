#include "world/generation/old_generator.hpp"
#include "FastNoise/Generators/Simplex.h"
#include "util/time.hpp"
#include "world/chunk.hpp"
#include "world/material.hpp"
#include <memory>

OldWorldGenerator::OldWorldGenerator(int seed)
    : WorldGenerator(seed) {}

std::shared_ptr<Chunk> OldWorldGenerator::generate(glm::ivec2 pos) {
    Timer timer("Chunk Generation [old_generator]");

    int i;

    Block* blocks = new Block[CHUNK_LENGTH];

    Biome* biomes = (Biome*) malloc(sizeof(Biome) * CHUNK_SIZE * CHUNK_SIZE);

    generate_biomes(biomes, pos);

    auto height_generator_simplex = FastNoise::New<FastNoise::Simplex>();
    auto height_generator = FastNoise::New<FastNoise::FractalFBm>();
    height_generator->SetSource(height_generator_simplex);
    height_generator->SetOctaveCount(4);

    auto heightmap_r1 = generate_noise(height_generator, pos, 0.005f);
    auto heightmap_r2 = generate_noise(height_generator, pos, 0.001f, 1);
    auto heightmap_scale = generate_noise(height_generator_simplex, pos, 0.004f, 2);

    auto heightmap = new float[CHUNK_SIZE * CHUNK_SIZE];
    i = 0;
    for (int z = 0; z < CHUNK_SIZE; z++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            heightmap[i] = heightmap_r1[i] * 0.25f + heightmap_r2[i] * 0.75f;
            i++;
        }
    }

    struct drand48_data rand_data;
    srand48_r(seed * pos.x + pos.y, &rand_data);
    
    i = 0;
    for (int z = 0; z < CHUNK_SIZE; z++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            int stone_h = 80 + heightmap[i] * 50 * heightmap_scale[i];

            fill_col(blocks, {x, z}, {0, stone_h}, Material::STONE);

            double random;

            Biome biome = biomes[i];
            if (biome == (Biome)Biome::PLAINS || biome == (Biome)Biome::FOREST) {
                fill_col(blocks, {x, z}, {stone_h + 1, stone_h + 5}, Material::DIRT);
                set(blocks, {x, stone_h + 6, z}, Material::GRASS_BLOCK);
                
                drand48_r(&rand_data, &random);
                if (random >= 0.995f) {
                    set(blocks, {x, stone_h + 7, z}, Material::FLOWER);
                }
                drand48_r(&rand_data, &random);
                if (random >= 0.98f) {
                    set(blocks, {x, stone_h + 7, z}, Material::GRASS);
                }
                drand48_r(&rand_data, &random);
                if (biome == (Biome)Biome::FOREST && random > 0.98f) {
                    int tree_h = stone_h + 7 + glm::linearRand<int>(3, 6);
                    // TODO: leaves on chunk boundaries
                    // fill(blocks, {x - 2, stone_h + 10, z - 2}, {x + 2, tree_h + 1, z + 2}, Material::LEAVES);
                    fill_col(blocks, {x, z}, {stone_h + 7, tree_h}, Material::LOG);
                }
            } else if (biome == (Biome)Biome::MOUNTAINS) {
                int height = 6;
                fill_col(blocks, {x, z}, {stone_h, stone_h + height}, Material::STONE);
                // TODO: snow caps
            } else if (biome == (Biome)Biome::SNOWY_PLAINS) {
                fill_col(blocks, {x, z}, {stone_h + 1, stone_h + 5}, Material::DIRT);
                set(blocks, {x, stone_h + 6, z}, Material::SNOW);

                drand48_r(&rand_data, &random);
                if (random >= 0.995f) {
                    set(blocks, {x, stone_h + 7, z}, Material::DEAD_BUSH);
                }
            } else if (biome == (Biome)Biome::DESERT) {
                fill_col(blocks, {x, z}, {stone_h + 1, stone_h + 6}, Material::SAND);

                drand48_r(&rand_data, &random);
                if (random >= 0.99f) {
                    set(blocks, {x, stone_h + 7, z}, Material::DEAD_BUSH);
                }
            }

            set(blocks, {x, 0, z}, Material::BEDROCK);

            i++;
        }
    }

    delete[] heightmap;
    delete[] heightmap_scale;
    delete[] heightmap_r1;
    delete[] heightmap_r2;

    free(biomes);

    return std::make_shared<Chunk>(pos.x, pos.y, blocks);
}

void OldWorldGenerator::generate_biomes(Biome* biomes, glm::ivec2 pos) {
    auto noise = FastNoise::New<FastNoise::Simplex>();

    auto temp_map = generate_noise(noise, pos);
    auto hum_map = generate_noise(noise, pos, 0.001f, 3);

    int i = 0;
    for (int z = 0; z < CHUNK_SIZE; z++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            biomes[i] = BiomeTable::get((temp_map[i] + 1.0f) / 2.0f, (hum_map[i] + 1.0f) / 2.0f);
            i++;
        }
    }

    delete[] temp_map;
    delete[] hum_map;
}
