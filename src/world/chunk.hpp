#pragma once

#define CHUNK_HEIGHT 256
#define CHUNK_SIZE 16
#define CHUNK_LENGTH (CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT)

#include <utility>
#include <array>
#include <map>

#include "block.hpp"
#include "../graphics/mesh.hpp"
#include "../graphics/opengl.hpp"
#include "../graphics/texture.hpp"

class Chunk {
public:
    Chunk();
    Chunk(int seed, int cx, int cz);
    ~Chunk();

    Block& block(int x, int y, int z);
    Block& block(const BlockPosition& pos);
    Block block_safe(int x, int y, int z);
    Block block_safe(const BlockPosition& pos);

    std::vector<std::pair<BlockPosition, Block&>> list_blocks();

    void generate();

    MeshData mesh(Texture3D* tex);

private:
    int cx, cz;
    int seed;

    // XZY order
    Block* blocks;
};
