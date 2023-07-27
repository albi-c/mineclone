#include "world/generation/old_generator.hpp"
#include "util/time.hpp"
#include "world/chunk.hpp"
#include "world/material.hpp"
#include <memory>

OldWorldGenerator::OldWorldGenerator(int seed)
    : WorldGenerator(seed) {}

std::shared_ptr<Chunk> OldWorldGenerator::generate(glm::ivec2 pos) {
    Timer timer("Chunk Generation [old_generator]");

    int cx = pos.x;
    int cz = pos.y;

    Block* blocks = new Block[CHUNK_LENGTH];

    Biome* biomes = (Biome*) malloc(sizeof(Biome) * CHUNK_SIZE * CHUNK_SIZE);

    generate_biomes(biomes, cx, cz);

    auto heightmap_r1 = new float[CHUNK_SIZE][CHUNK_SIZE];
    auto heightmap_r2 = new float[CHUNK_SIZE][CHUNK_SIZE];
    auto heightmap_scale = new float[CHUNK_SIZE][CHUNK_SIZE];

    // FastNoise::SmartNode<> height_generator = FastNoise::NewFromEncodedNodeTree("IQATAMP1KD8NAAQAAAAAACBACQAAZmYmPwAAAAA/DwADAAAAAAAAQP//AQAAAAAAPwAAAAAAARwAAQcAAAAAoEA=");
    
    auto height_generator_simplex = FastNoise::New<FastNoise::Simplex>();
    auto height_generator = FastNoise::New<FastNoise::FractalFBm>();
    height_generator->SetSource(height_generator_simplex);
    height_generator->SetOctaveCount(4);

    height_generator->GenUniformGrid2D(&heightmap_r1[0][0], cx * CHUNK_SIZE + CHUNK_SIZE / 2, cz * CHUNK_SIZE + CHUNK_SIZE / 2, CHUNK_SIZE, CHUNK_SIZE, 0.005f, seed);
    height_generator->GenUniformGrid2D(&heightmap_r2[0][0], cx * CHUNK_SIZE + CHUNK_SIZE / 2, cz * CHUNK_SIZE + CHUNK_SIZE / 2, CHUNK_SIZE, CHUNK_SIZE, 0.001f, seed);
    height_generator_simplex->GenUniformGrid2D(&heightmap_scale[0][0], cx * CHUNK_SIZE + CHUNK_SIZE / 2, cz * CHUNK_SIZE + CHUNK_SIZE / 2, CHUNK_SIZE, CHUNK_SIZE, 0.004f, seed);

    auto heightmap = new float[CHUNK_SIZE][CHUNK_SIZE];
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            heightmap[x][z] = heightmap_r1[z][x] * 0.25f + heightmap_r2[z][x] * 0.75f;
        }
    }

    struct drand48_data rand_data;
    srand48_r(seed * cx + cz, &rand_data);
    
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            int stone_h = 80 + heightmap[x][z] * 50 * heightmap_scale[z][x];

            fill_col(blocks, {x, z}, {0, stone_h}, Material::STONE);

            double random;

            Biome biome = biomes[get_index(x, z)];
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
                if (height < 70)
                    fill_col(blocks, {x, z}, {stone_h, stone_h + height}, Material::STONE);
                else {
                    fill_col(blocks, {x, z}, {stone_h, stone_h + height}, Material::STONE);
                    fill_col(blocks, {x, z}, {stone_h + 70 - heightmap[x][z] * 2, stone_h + height}, Material::SNOW);
                }
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
        }
    }

    delete[] heightmap;
    delete[] heightmap_scale;
    delete[] heightmap_r1;
    delete[] heightmap_r2;

    free(biomes);

    return std::make_shared<Chunk>(cx, cz, blocks);
}

void OldWorldGenerator::generate_biomes(Biome* biomes, int cx, int cz) {
    FastNoise::SmartNode<> temp_generator = FastNoise::NewFromEncodedNodeTree("GgABDQADAAAAAAAAQCEADQADAAAAAAAAQAgAAAAAAD8BBwAJAAEaAAAAAIA/AQgAAAAAAD8AAAAAAAAAAAA/");

    auto temp_map = new float[CHUNK_SIZE][CHUNK_SIZE];
    temp_generator->GenUniformGrid2D(&temp_map[0][0], cx * CHUNK_SIZE + CHUNK_SIZE / 2, cz * CHUNK_SIZE + CHUNK_SIZE / 2, CHUNK_SIZE, CHUNK_SIZE, 0.001, seed);

    auto hum_map = new float[CHUNK_SIZE][CHUNK_SIZE];
    temp_generator->GenUniformGrid2D(&hum_map[0][0], cx * CHUNK_SIZE + CHUNK_SIZE / 2, cz * CHUNK_SIZE + CHUNK_SIZE / 2, CHUNK_SIZE, CHUNK_SIZE, 0.001, seed + 5);

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            biomes[get_index(x, z)] = BiomeTable::get((temp_map[z][x] + 1.0f) / 2.0f, (hum_map[z][x] + 1.0f) / 2.0f);
        }
    }

    delete[] temp_map;
    delete[] hum_map;
}
