#include "world/chunk.hpp"
#include "FastNoise/Generators/Fractal.h"
#include "FastNoise/Generators/Perlin.h"
#include "FastNoise/Generators/Simplex.h"
#include "world/block.hpp"
#include "graphics/mesh/mesh.hpp"
#include "graphics/textures/texture_array.hpp"
#include "material.hpp"
#include "util/time.hpp"
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <memory>

// xyz to block index
#define BP(x, y, z) ((x) | ((z) << 4) | ((y) << 8))

const static int block_neighbors[6][3] = {
    {0, 0, -1},
    {0, 0, 1},
    {-1, 0, 0},
    {1, 0, 0},
    {0, -1, 0},
    {0, 1, 0}
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

const static float plant_normals[4][3] = {
    {-0.5f, 0.0f, -0.5f},
    {0.5f, 0.0f, 0.5f},
    {0.5f, 0.0f, -0.5f},
    {-0.5f, 0.0f, 0.5f}
};
const std::array<std::array<float, 30>, 4> plant_faces = {{
    {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // 1
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // 2
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // 3
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // 4
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // 5
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // 6
    }, {
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 6
         0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // 5
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 2
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 6
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 2
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // 1
    }, {
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 1.0f, 0.0f
    }, {
         0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
         0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 1.0f, 0.0f
    }
}};

static int block_occluders[6][6][2][3] = {0};
static bool data_initialized = false;

static void init_data() {
    if (data_initialized) {
        return;
    }

    for (int face = 0; face < 6; face++) {
        auto& normal = block_normals[face];

        for (int part = 0; part < 6; part++) {
            int axis = -1;
            int dir;
            int o1;
            int o2;
            for (int i = 0; i < 3; i++) {
                if (normal[i] != 0) {
                    axis = i;
                    dir = normal[i];
                    break;
                }
            }
            if (axis < 0 || axis > 2) {
                std::cerr << "Invalid block normal in occluder generator\n";
                break;
            }
            switch (axis) {
                case 0:
                    o1 = 1;
                    o2 = 2;
                    break;
                case 1:
                    o1 = 0;
                    o2 = 2;
                    break;
                case 2:
                    o1 = 0;
                    o2 = 1;
                    break;
            }

            switch (part) {
                case 0:
                case 4:
                    block_occluders[face][part][0][axis] = dir;
                    block_occluders[face][part][0][o1] = 0;
                    block_occluders[face][part][0][o2] = -1;
                    block_occluders[face][part][1][axis] = dir;
                    block_occluders[face][part][1][o1] = -1;
                    block_occluders[face][part][1][o2] = 0;
                    break;
                case 1:
                case 3:
                    block_occluders[face][part][0][axis] = dir;
                    block_occluders[face][part][0][o1] = 0;
                    block_occluders[face][part][0][o2] = 1;
                    block_occluders[face][part][1][axis] = dir;
                    block_occluders[face][part][1][o1] = 1;
                    block_occluders[face][part][1][o2] = 0;
                    break;
                case 2:
                    block_occluders[face][part][0][axis] = dir;
                    block_occluders[face][part][0][o1] = 0;
                    block_occluders[face][part][0][o2] = -1;
                    block_occluders[face][part][1][axis] = dir;
                    block_occluders[face][part][1][o1] = -1;
                    block_occluders[face][part][1][o2] = 0;
                    break;
                case 5:
                    block_occluders[face][part][0][axis] = dir;
                    block_occluders[face][part][0][o1] = -1;
                    block_occluders[face][part][0][o2] = 0;
                    block_occluders[face][part][1][axis] = dir;
                    block_occluders[face][part][1][o1] = 0;
                    block_occluders[face][part][1][o2] = -1;
                    break;
            }
        }
    }

    data_initialized = true;
}

static float block_textures[Material::_size()][6] = {0};
static bool block_textures_initialized = false;
static bool block_textures_initializing = false;

static void init_block_textures(const TextureArray& tex) {
    if (block_textures_initialized) {
        return;
    } else {
        if (block_textures_initializing) {
            while (block_textures_initializing);
            return;
        }
    }

    block_textures_initializing = true;

    for (size_t mat = 0; mat < Material::_size(); mat++) {
        if (mat == Material::AIR) {
            continue;
        }

        Block block(Material::_from_index(mat));

        for (int face = 0; face < 6; face++) {
            block_textures[mat][face] = tex.position(block.face_texture(face));
        }
    }

    block_textures_initialized = true;
    block_textures_initializing = false;
}

std::mutex Chunk::blocks_to_set_mutex;
std::map<std::pair<int, int>, std::vector<std::pair<BlockPosition, Block>>> Chunk::blocks_to_set;

Chunk::Chunk(int cx, int cz, Block* blocks)
    : cx(cx), cz(cz), blocks(blocks) {}

Chunk::~Chunk() {
    if (blocks != nullptr)
        delete[] blocks;
}

Block Chunk::get(int x, int y, int z) {
    if (x < 0 || y < 0 || z < 0 || x > CHUNK_SIZE - 1 || y > CHUNK_HEIGHT - 1 || z > CHUNK_SIZE - 1) {
        if (x > -CHUNK_SIZE && z > -CHUNK_SIZE && x < CHUNK_SIZE * 2 - 1 && z < CHUNK_SIZE * 2 - 1) {
            if (x < 0 && has_neighbor(ChunkNeighbor::NX)) {
                auto neighbor = get_neighbor(ChunkNeighbor::NX);
                if (neighbor)
                    return neighbor->get(x + CHUNK_SIZE, y, z);
            }
            
            if (x > CHUNK_SIZE - 1 && has_neighbor(ChunkNeighbor::PX)) {
                auto neighbor = get_neighbor(ChunkNeighbor::PX);
                if (neighbor)
                    return neighbor->get(x - CHUNK_SIZE, y, z);
            }
            
            if (z < 0 && has_neighbor(ChunkNeighbor::NZ)) {
                auto neighbor = get_neighbor(ChunkNeighbor::NZ);
                if (neighbor)
                    return neighbor->get(x, y, z + CHUNK_SIZE);
            }
            
            if (z > CHUNK_SIZE - 1 && has_neighbor(ChunkNeighbor::PZ)) {
                auto neighbor = get_neighbor(ChunkNeighbor::PZ);
                if (neighbor)
                    return neighbor->get(x, y, z - CHUNK_SIZE);
            }
        }
        return Block();
    }
    
    return blocks[BP(x, y, z)];
}
Block Chunk::get(const BlockPosition& pos) {
    return get(pos.x, pos.y, pos.z);
}

void Chunk::set(int x, int y, int z, const Block& block) {
    if (x < 0 || y < 0 || z < 0 || x > CHUNK_SIZE - 1 || y > CHUNK_HEIGHT - 1 || z > CHUNK_SIZE - 1) {
        tu::mutex_lock_timeout_exc(blocks_to_set_mutex);
        
        if (x < 0) {
            Chunk::blocks_to_set[{cx - 1, cz}].push_back({{x + CHUNK_SIZE, y, z}, block});
        } else if (z < 0) {
            Chunk::blocks_to_set[{cx, cz - 1}].push_back({{x, y, z + CHUNK_SIZE}, block});
        } else if (x > CHUNK_SIZE - 1) {
            Chunk::blocks_to_set[{cx + 1, cz}].push_back({{x - CHUNK_SIZE, y, z}, block});
        } else if (z > CHUNK_SIZE - 1) {
            Chunk::blocks_to_set[{cx, cz + 1}].push_back({{x, y, z - CHUNK_SIZE}, block});
        }

        blocks_to_set_mutex.unlock();
    } else {
        blocks[BP(x, y, z)] = block;

        if (x == 0)
            EventManager::fire(EventChunkRedraw{cx - 1, cz});
        if (x == CHUNK_SIZE - 1)
            EventManager::fire(EventChunkRedraw{cx + 1, cz});
        if (z == 0)
            EventManager::fire(EventChunkRedraw{cx, cz - 1});
        if (z == CHUNK_SIZE - 1)
            EventManager::fire(EventChunkRedraw{cx, cz + 1});

        EventManager::fire(EventChunkRedraw{cx, cz});
    }
}
void Chunk::set(const BlockPosition& pos, const Block& block) {
    set(pos.x, pos.y, pos.z, block);
}

void Chunk::fill(int x1, int y1, int z1, int x2, int y2, int z2, const Block& block) {
    for (int x = x1; x <= x2; x++) {
        for (int y = y1; y <= y2; y++) {
            for (int z = z1; z <= z2; z++) {
                set(x, y, z, block);
            }
        }
    }
}
void Chunk::fill(const BlockPosition& pos1, const BlockPosition& pos2, const Block& block) {
    fill(pos1.x, pos1.y, pos2.x, pos2.x, pos2.y, pos2.z, block);
}

void Chunk::update() {
    tu::mutex_lock_timeout_exc(blocks_to_set_mutex);

    for (auto& [pos, block] : Chunk::blocks_to_set[{cx, cz}]) {
        set_nolock(pos.x, pos.y, pos.z, block);
    }
    Chunk::blocks_to_set.erase({cx, cz});
    
    blocks_to_set_mutex.unlock();
}

std::shared_ptr<MeshData<Chunk::MeshType>> Chunk::mesh(const TextureArray& tex) {
    Timer timer("Chunk Meshing");

    init_block_textures(tex);

    std::vector<float> vertices;

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                int i = BP(x, y, z);
                Block& block = blocks[i];

                if (block == Material::AIR)
                    continue;
                
                #define PART_SIZE 11

                if (block.plant()) {
                    #define ADD_PLANT_PART(texture, block, data, face, part, x, y, z) \
                        data[PART_SIZE * part + 0] = plant_faces[face][5 * part + 0] + x; \
                        data[PART_SIZE * part + 1] = plant_faces[face][5 * part + 1] + y; \
                        data[PART_SIZE * part + 2] = plant_faces[face][5 * part + 2] + z; \
                        data[PART_SIZE * part + 3] = plant_faces[face][5 * part + 3]; \
                        data[PART_SIZE * part + 4] = plant_faces[face][5 * part + 4]; \
                        data[PART_SIZE * part + 5] = block_textures[block.material][face]; \
                        data[PART_SIZE * part + 6] = plant_normals[face][0]; \
                        data[PART_SIZE * part + 7] = plant_normals[face][1]; \
                        data[PART_SIZE * part + 8] = plant_normals[face][2]; \
                        data[PART_SIZE * part + 9] = (float)block.material; \
                        data[PART_SIZE * part + 10] = 1.0f
                
                    #define ADD_PLANT(vertices, texture, block, data, face, x, y, z) \
                        ADD_PLANT_PART(texture, block, data, face, 0, x, y, z); \
                        ADD_PLANT_PART(texture, block, data, face, 1, x, y, z); \
                        ADD_PLANT_PART(texture, block, data, face, 2, x, y, z); \
                        ADD_PLANT_PART(texture, block, data, face, 3, x, y, z); \
                        ADD_PLANT_PART(texture, block, data, face, 4, x, y, z); \
                        ADD_PLANT_PART(texture, block, data, face, 5, x, y, z); \
                        vertices.insert(vertices.end(), &data[0], &data[PART_SIZE * 6])
                
                    float plant_data[PART_SIZE * 6];

                    ADD_PLANT(vertices, tex, block, plant_data, 0, x, y, z);
                    ADD_PLANT(vertices, tex, block, plant_data, 1, x, y, z);
                    ADD_PLANT(vertices, tex, block, plant_data, 2, x, y, z);
                    ADD_PLANT(vertices, tex, block, plant_data, 3, x, y, z);

                    continue;
                }
                
                #define GET_OCCLUSION_CORNER(face, part, corner) \
                    (GET_OFFSET(x, y, z, block_occluders[face][part][corner]).transparent() ? 0.25 : 0.1)

                #define GET_OCCLUSION(face, part) \
                    (GET_OCCLUSION_CORNER(face, part, 0) + GET_OCCLUSION_CORNER(face, part, 1))

                #define ADD_FACE_PART(face, part, occlusion) \
                    data[PART_SIZE * part + 0] = block_faces[face][5 * part + 0] + x; \
                    data[PART_SIZE * part + 1] = block_faces[face][5 * part + 1] + y; \
                    data[PART_SIZE * part + 2] = block_faces[face][5 * part + 2] + z; \
                    data[PART_SIZE * part + 3] = block_faces[face][5 * part + 3]; \
                    data[PART_SIZE * part + 4] = block_faces[face][5 * part + 4]; \
                    data[PART_SIZE * part + 5] = block_textures[block.material][face]; \
                    data[PART_SIZE * part + 6] = block_normals[face][0]; \
                    data[PART_SIZE * part + 7] = block_normals[face][1]; \
                    data[PART_SIZE * part + 8] = block_normals[face][2]; \
                    data[PART_SIZE * part + 9] = (float)block.material; \
                    data[PART_SIZE * part + 10] = occlusion
                
                #define ADD_FACE(face) \
                    for (int i = 0; i < 6; i++) { \
                        ADD_FACE_PART(face, i, GET_OCCLUSION(face, i)); \
                    } \
                    vertices.insert(vertices.end(), &data[0], &data[PART_SIZE * 6])
                
                #define GET_OFFSET(x, y, z, offset) \
                    GET_BLOCK(x + (offset)[0], y + (offset)[1], z + (offset)[2])
                    
                #define TRANSPARENT_CHECK(xo, yo, zo) \
                    (block.transparent() ? GET_BLOCK(x + xo, y + yo, z + zo) != block : GET_BLOCK(x + xo, y + yo, z + zo).transparent())
                
                if (x > 0 && y > 0 && z > 0 && x < CHUNK_SIZE - 1 && y < CHUNK_HEIGHT - 1 && z < CHUNK_SIZE - 1) {
                    #define GET_BLOCK(x, y, z) \
                        blocks[BP((x), (y), (z))]

                    float data[PART_SIZE * 6];

                    if (TRANSPARENT_CHECK(-1, 0, 0)) {
                        ADD_FACE(2);
                    }
                    if (TRANSPARENT_CHECK(1, 0, 0)) {
                        ADD_FACE(3);
                    }

                    if (TRANSPARENT_CHECK(0, 0, -1)) {
                        ADD_FACE(0);
                    }
                    if (TRANSPARENT_CHECK(0, 0, 1)) {
                        ADD_FACE(1);
                    }

                    if (y <= 0 || TRANSPARENT_CHECK(0, -1, 0)) {
                        ADD_FACE(4);
                    }
                    if (y + 2 > CHUNK_HEIGHT || TRANSPARENT_CHECK(0, 1, 0)) {
                        ADD_FACE(5);
                    }

                    #undef GET_BLOCK
                } else {
                    #define GET_BLOCK(x, y, z) \
                        get((x), (y), (z))

                    float data[PART_SIZE * 6];

                    if (TRANSPARENT_CHECK(-1, 0, 0)) {
                        ADD_FACE(2);
                    }
                    if (TRANSPARENT_CHECK(1, 0, 0)) {
                        ADD_FACE(3);
                    }

                    if (TRANSPARENT_CHECK(0, 0, -1)) {
                        ADD_FACE(0);
                    }
                    if (TRANSPARENT_CHECK(0, 0, 1)) {
                        ADD_FACE(1);
                    }

                    if (y <= 0 || TRANSPARENT_CHECK(0, -1, 0)) {
                        ADD_FACE(4);
                    }
                    if (y + 2 > CHUNK_HEIGHT || TRANSPARENT_CHECK(0, 1, 0)) {
                        ADD_FACE(5);
                    }
                }
            }
        }
    }

    return std::make_shared<MeshData<Chunk::MeshType>>(std::vector{
        MeshDataPart(3),
        MeshDataPart(3),
        MeshDataPart(3),
        MeshDataPart(1),
        MeshDataPart(1)
    }, vertices);
}

bool Chunk::has_neighbor(ChunkNeighbor neighbor) {
    return neighbors[(int)neighbor].lock() != nullptr;
}
std::shared_ptr<Chunk> Chunk::get_neighbor(ChunkNeighbor neighbor) {
    return neighbors[(int)neighbor].lock();
}
void Chunk::set_neighbor(ChunkNeighbor neighbor, std::shared_ptr<Chunk> chunk) {
    neighbors[(int)neighbor] = chunk;
    EventManager::fire(EventChunkRedraw{cx, cz});
}
bool Chunk::has_all_neighbors() {
    for (int i = 0; i < 0; i++) {
        if (neighbors[i].lock() == nullptr) {
            return false;
        }
    }

    return true;
}

ChunkPosition Chunk::position() {
    return {cx, cz};
}

void Chunk::set_nolock(int x, int y, int z, const Block& block) {
    if (x < 0 || y < 0 || z < 0 || x > CHUNK_SIZE - 1 || y > CHUNK_HEIGHT - 1 || z > CHUNK_SIZE - 1) {
        if (x < 0) {
            Chunk::blocks_to_set[{cx - 1, cz}].push_back({{x + CHUNK_SIZE, y, z}, block});
        } else if (z < 0) {
            Chunk::blocks_to_set[{cx, cz - 1}].push_back({{x, y, z + CHUNK_SIZE}, block});
        } else if (x > CHUNK_SIZE - 1) {
            Chunk::blocks_to_set[{cx + 1, cz}].push_back({{x - CHUNK_SIZE, y, z}, block});
        } else if (z > CHUNK_SIZE - 1) {
            Chunk::blocks_to_set[{cx, cz + 1}].push_back({{x, y, z - CHUNK_SIZE}, block});
        }
    } else {
        blocks[BP(x, y, z)] = block;

        if (x == 0)
            EventManager::fire(EventChunkRedraw{cx - 1, cz});
        if (x == CHUNK_SIZE - 1)
            EventManager::fire(EventChunkRedraw{cx + 1, cz});
        if (z == 0)
            EventManager::fire(EventChunkRedraw{cx, cz - 1});
        if (z == CHUNK_SIZE - 1)
            EventManager::fire(EventChunkRedraw{cx, cz + 1});

        EventManager::fire(EventChunkRedraw{cx, cz});
    }
}

void Chunk::set_unchecked(int x, int y, int z, const Block& block) {
    blocks[BP(x, y, z)] = block;
}
void Chunk::set_unchecked(int x, int y, int z, Material material) {
    blocks[BP(x, y, z)] = Block(material);
}

void Chunk::fill_unchecked(int x1, int y1, int z1, int x2, int y2, int z2, const Block& block) {
    if (x1 == x2 && z1 == z2) {
        for (int y = y1; y <= y2; y++) {
            blocks[BP(x1, y, z1)] = block;
        }
    }

    for (int x = x1; x <= x2; x++) {
        for (int y = y1; y <= y2; y++) {
            for (int z = z1; z <= z2; z++) {}
        }
    }
}
void Chunk::fill_unchecked(int x1, int y1, int z1, int x2, int y2, int z2, Material material) {
    fill_unchecked(x1, y1, z1, x2, y2, z1, Block(material));
}

int Chunk::highest_block(int x, int z) {
    for (int y = CHUNK_HEIGHT; y > 0; y--) {
        if(get(x, y, z))
            return y;
    }
    return CHUNK_HEIGHT - 1;
}

void Chunk::init() {
    init_data();
}
