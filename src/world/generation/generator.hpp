#pragma once

#include <utility>
#include <memory>

#include "world/chunk.hpp"
#include "world/material.hpp"

class Chunk;
class WorldGenerator {
public:
    WorldGenerator(int seed);
    virtual ~WorldGenerator() {}

    virtual std::shared_ptr<Chunk> generate(glm::ivec2 pos) =0;

protected:
    int seed;

    template <class T>
    inline float* generate_noise(const T& source, glm::uvec2 pos, float frequency = 0.001f, int seed_offset = 0) {
        float* data = new float[CHUNK_SIZE * CHUNK_SIZE];
        source->GenUniformGrid2D(data, pos.x * CHUNK_SIZE, pos.y * CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE, frequency, seed + seed_offset);
        return data;
    }

    static inline size_t get_index(unsigned int x, unsigned int z) {
        return x + z * CHUNK_SIZE;
    }
    static inline size_t get_index(glm::uvec2 pos) {
        return pos.x + pos.y * CHUNK_SIZE;
    }

    static inline size_t get_index(unsigned int x, unsigned int y, unsigned int z) {
        return x + z * CHUNK_SIZE + y * CHUNK_SIZE * CHUNK_SIZE;
    }
    static inline size_t get_index(glm::uvec3 pos) {
        return pos.x + pos.z * CHUNK_SIZE + pos.y * CHUNK_SIZE * CHUNK_SIZE;
    }

    inline void set(Block* blocks, glm::uvec3 pos, Material block) {
        set(blocks, pos, Block(block));
    }
    inline void set(Block* blocks, glm::uvec3 pos, Block block) {
        blocks[get_index(pos)] = block;
    }

    inline Block get(Block* blocks, glm::uvec3 pos) {
        return blocks[get_index(pos)];
    }

    inline void fill(Block* blocks, glm::uvec3 pos1, glm::uvec3 pos2, Material block) {
        fill(blocks, pos1, pos2, Block(block));
    }
    inline void fill(Block* blocks, glm::uvec3 pos1, glm::uvec3 pos2, Block block) {
        for (unsigned int x = pos1.x; x <= pos2.x; x++) {
            for (unsigned int y = pos1.y; y <= pos2.y; y++) {
                for (unsigned int z = pos1.z; z <= pos2.z; z++) {
                    blocks[get_index({x, y, z})] = block;
                }
            }
        }
    }

    inline void fill_col(Block* blocks, glm::uvec2 xz, glm::uvec2 y_range, Material block) {
        fill_col(blocks, xz, y_range, Block(block));
    }
    inline void fill_col(Block* blocks, glm::uvec2 xz, glm::uvec2 y_range, Block block) {
        for (unsigned int y = y_range.x; y <= y_range.y; y++) {
            blocks[get_index({xz.x, y, xz.y})] = block;
        }
    }
};
