#include "chunk.hpp"

const static std::array<BlockPosition, 6> block_neighbors = {{
    {0, 0, -1},
    {0, 0, 1},
    {-1, 0, 0},
    {1, 0, 0},
    {0, -1, 0},
    {0, 1, 0},
}};

const static std::array<std::array<float, 30>, 6> block_faces = {{
    {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    }, {
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    }, {
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    }, {
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    }, {
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    }, {
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    }
}};

Chunk::Chunk(int cx, int cy)
    : cx(cx), cy(cy) {
    
    generate();
}

Block& Chunk::block(int x, int y, int z) {
    return blocks[x][y][z];
}
Block& Chunk::block(const BlockPosition& pos) {
    return block(pos.x, pos.y, pos.z);
}
Block Chunk::block_safe(int x, int y, int z) {
    if (x < 0 || y < 0 || z < 0 || x > CHUNK_SIZE - 1 || y > CHUNK_HEIGHT - 1 || z > CHUNK_SIZE - 1)
        return Block();
    
    return blocks[x][y][z];
}
Block Chunk::block_safe(const BlockPosition& pos) {
    return block_safe(pos.x, pos.y, pos.z);
}

std::vector<std::pair<BlockPosition, Block&>> Chunk::list_blocks() {
    std::vector<std::pair<BlockPosition, Block&>> blocks;
    blocks.reserve(CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT);

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                blocks.push_back({{x, y, z}, this->blocks[x][y][z]});
            }
        }
    }

    return blocks;
}

void Chunk::generate() {
    for (auto& [pos, block] : list_blocks()) {
        if (pos.y < 61) {
            block = Block(Material::STONE);
        } else if (pos.y > 60 && pos.y < 63) {
            block = Block(Material::DIRT);
        } else if (pos.y == 63) {
            block = Block(Material::GRASS_BLOCK);
        }
    }
}

MeshData Chunk::mesh(Texture3D* tex) {
    std::vector<float> vertices;

    for (auto& [pos, block] : list_blocks()) {
        if (block == Material::AIR)
            continue;

        int i = 0, faces = 0;
        for (auto& neighbor : block_neighbors) {
            if (block_safe(pos + neighbor).transparent()) {
                faces |= 1 << i;
            }
            i++;
        }
        
        for (int i = 0; i < 6; i++) {
            if (faces & (1 << i)) {
                for (int j = 0; j < 6; j++) {
                    vertices.push_back(block_faces[i][0 + j * 5] + pos.x);
                    vertices.push_back(block_faces[i][1 + j * 5] + pos.y);
                    vertices.push_back(block_faces[i][2 + j * 5] + pos.z);

                    vertices.push_back(block_faces[i][3 + j * 5]);
                    vertices.push_back(block_faces[i][4 + j * 5]);

                    vertices.push_back(tex->position(block.face_texture(i)));
                }
            }
        }
    }

    return MeshData({3, 3}, vertices);
}
