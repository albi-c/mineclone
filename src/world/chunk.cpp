#include "chunk.hpp"

// xyz to block index
#define BP(x, y, z) ((x) | ((z) << 4) | ((y) << 8))
#define A_BP(xyz) (BP(xyz[0], yyz[1], xyz[2]))
// offset block index by xyz
#define BPO(pos, x, y, z) ((pos) + BP(x, y, z))
#define A_BPO(pos, xyz) ((pos) + A_BP(xyz))
// block index to x
#define BPX(pos) ((pos) & 0xf)
// block index to y
#define BPY(pos) ((pos) >> 4 & 0xf)
// block index to z
#define BPZ(pos) ((pos) >> 8 & 0x7f)
// block index to xyz
static constexpr void BPP(int pos, int& x, int& y, int& z) {
    x = pos & 0xf;
    y = (pos >> 4) & 0xf;
    z = (pos >> 8) & 0x7f;
}
// is block index at the edge
static constexpr bool BPE(int pos) {
    int x, y, z;
    BPP(pos, x, y, z);
    return x < 0 || y < 0 || z < 0 || x > CHUNK_SIZE - 1 || y > CHUNK_HEIGHT - 1 || z > CHUNK_SIZE - 1;
}

const static int block_neighbors[6][3] = {
    {0, 0, -1},
    {0, 0, 1},
    {-1, 0, 0},
    {1, 0, 0},
    {0, -1, 0},
    {0, 1, 0}
};

const static float block_light[6] = {
    0.95f,
    0.95f,
    0.6f,
    1.0f,
    0.9f,
    0.9f
};

const static float block_normals[6][3] = {
    {0, 0, -1},
    {0, 0, 1},
    {-1, 0, 0},
    {1, 0, 0},
    {0, -1, 0},
    {0, 1, 0}
};

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

static std::map<std::pair<int, int>, std::vector<std::pair<BlockPosition, Block>>> blocks_to_set;

Chunk::Chunk()
    : blocks(new Block[CHUNK_LENGTH]()) {}

Chunk::Chunk(int seed, int cx, int cz)
    : seed(seed), cx(cx), cz(cz), blocks(new Block[CHUNK_LENGTH]()) {
    
    generate();
}

Chunk::~Chunk() {
    delete[] blocks;
}

Block& Chunk::block(int x, int y, int z) {
    return blocks[BP(x, y, z)];
}
Block& Chunk::block(const BlockPosition& pos) {
    return block(pos.x, pos.y, pos.z);
}
Block Chunk::block_safe(int x, int y, int z) {
    if (x < 0 || y < 0 || z < 0 || x > CHUNK_SIZE - 1 || y > CHUNK_HEIGHT - 1 || z > CHUNK_SIZE - 1)
        return Block();
    
    return blocks[BP(x, y, z)];
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
                blocks.push_back({{x, y, z}, this->blocks[BP(x, y, z)]});
            }
        }
    }

    return blocks;
}

void Chunk::generate() {
    auto heightmap = new float[CHUNK_SIZE][CHUNK_SIZE];

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            heightmap[x][z] = glm::simplex(glm::vec3((cx * CHUNK_SIZE + x) / 128.0f, (cz * CHUNK_SIZE + z) / 128.0f, seed)) \
                + glm::simplex(glm::vec3((cx * CHUNK_SIZE + x) / 32.0f, (cz * CHUNK_SIZE + z) / 32.0f, seed)) / 4.0f \
                + glm::simplex(glm::vec3((cx * CHUNK_SIZE + x) / 16.0f, (cz * CHUNK_SIZE + z) / 16.0f, seed)) / 16.0f;
        }
    }

    #define CHUNK_FILL(x, z, from_y, to_y, block) \
        for (int y = (from_y); y <= (to_y); y++) { \
            blocks[BP((x), (y), (z))] = (block); \
        }

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            int stone_h = 40 + heightmap[x][z] * 10;
            CHUNK_FILL(x, z, 0, stone_h, Material::STONE)
            CHUNK_FILL(x, z, stone_h + 1, stone_h + 5, Material::DIRT)
            blocks[BP(x, stone_h + 6, z)] = Material::GRASS_BLOCK;

            if (glm::linearRand(0.0f, 1.0f) > 0.98f) {
                int treeh = stone_h + 7 + glm::linearRand<int>(3, 6);
                blocks[BP(x, treeh+1, z)] = Material::LEAVES;
                CHUNK_FILL(x, z, stone_h + 7, treeh, Material::LOG)
                if (x > 0)
                    CHUNK_FILL(x-1, z, stone_h + 9, treeh, Material::LEAVES)
                if (z > 0)
                    CHUNK_FILL(x, z-1, stone_h + 9, treeh, Material::LEAVES)
                if (x + 1 < CHUNK_SIZE)
                    CHUNK_FILL(x+1, z, stone_h + 9, treeh, Material::LEAVES)
                if (z + 1 < CHUNK_SIZE)
                    CHUNK_FILL(x, z+1, stone_h + 9, treeh, Material::LEAVES)
            }
        }
    }

    delete[] heightmap;
}

MeshData Chunk::mesh(Texture3D* tex) {
    std::vector<float> vertices;
    vertices.reserve(100);

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                int i = BP(x, y, z);
                Block& block = blocks[i];

                if (block == Material::AIR)
                    continue;
                
                #define FACE_PART_SIZE 9

                #define ADD_FACE_PART(texture, block, data, face, part, x, y, z) \
                    data[FACE_PART_SIZE * part + 0] = block_faces[face][5 * part + 0] + x; \
                    data[FACE_PART_SIZE * part + 1] = block_faces[face][5 * part + 1] + y; \
                    data[FACE_PART_SIZE * part + 2] = block_faces[face][5 * part + 2] + z; \
                    data[FACE_PART_SIZE * part + 3] = block_faces[face][5 * part + 3]; \
                    data[FACE_PART_SIZE * part + 4] = block_faces[face][5 * part + 4]; \
                    data[FACE_PART_SIZE * part + 5] = tex->position(block.face_texture(face)); \
                    data[FACE_PART_SIZE * part + 6] = block_normals[face][0]; \
                    data[FACE_PART_SIZE * part + 7] = block_normals[face][1]; \
                    data[FACE_PART_SIZE * part + 8] = block_normals[face][2]

                #define ADD_FACE(vertices, texture, block, data, face, x, y, z) \
                    ADD_FACE_PART(texture, block, face_data, face, 0, x, y, z); \
                    ADD_FACE_PART(texture, block, face_data, face, 1, x, y, z); \
                    ADD_FACE_PART(texture, block, face_data, face, 2, x, y, z); \
                    ADD_FACE_PART(texture, block, face_data, face, 3, x, y, z); \
                    ADD_FACE_PART(texture, block, face_data, face, 4, x, y, z); \
                    ADD_FACE_PART(texture, block, face_data, face, 5, x, y, z); \
                    vertices.insert(vertices.end(), &data[0], &data[FACE_PART_SIZE * 6])

                float face_data[FACE_PART_SIZE * 6];

                if (x - 1 < 0 || blocks[i - 1].transparent()) {
                    ADD_FACE(vertices, tex, block, face_data, 2, x, y, z);
                }
                if (x + 2 > CHUNK_SIZE || blocks[i + 1].transparent()) {
                    ADD_FACE(vertices, tex, block, face_data, 3, x, y, z);
                }

                if (z - 1 < 0 || blocks[i - CHUNK_SIZE].transparent()) {
                    ADD_FACE(vertices, tex, block, face_data, 0, x, y, z);
                }
                if (z + 2 > CHUNK_SIZE || blocks[i + CHUNK_SIZE].transparent()) {
                    ADD_FACE(vertices, tex, block, face_data, 1, x, y, z);
                }

                if (y - 1 < 0 || blocks[i - CHUNK_SIZE * CHUNK_SIZE].transparent()) {
                    ADD_FACE(vertices, tex, block, face_data, 4, x, y, z);
                }
                if (y + 2 > CHUNK_HEIGHT || blocks[i + CHUNK_SIZE * CHUNK_SIZE].transparent()) {
                    ADD_FACE(vertices, tex, block, face_data, 5, x, y, z);
                }
            }
        }
    }

    return MeshData({3, 3, 3}, vertices);
}
