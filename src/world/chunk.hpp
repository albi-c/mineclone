#pragma once

#define CHUNK_HEIGHT 256
#define CHUNK_SIZE 16

#include <utility>
#include <array>

#include "block.hpp"
#include "../graphics/mesh.hpp"
#include "../graphics/texture.hpp"

class Chunk {
public:
    Chunk(int cx, int cy);

    Block& block(int x, int y, int z);
    Block& block(const BlockPosition& pos);
    Block block_safe(int x, int y, int z);
    Block block_safe(const BlockPosition& pos);

    std::vector<std::pair<BlockPosition, Block&>> list_blocks();

    void generate();

    MeshData mesh(Texture3D* tex);

private:
    int cx, cy;

    // XYZ order
    Block blocks[CHUNK_SIZE][CHUNK_HEIGHT][CHUNK_SIZE];
};
