#include "chunk.hpp"

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

std::map<std::pair<int, int>, std::vector<std::pair<BlockPosition, Block>>> Chunk::blocks_to_set;

Chunk::Chunk()
    : blocks(NULL) {}
Chunk::Chunk(Chunk* other) {
    cx = other->cx;
    cz = other->cz;
    seed = other->seed;
    blocks = other->blocks;
    for (int i = 0; i < 4; i++) {
        neighbors[i] = other->neighbors[i];
    }
}
Chunk::Chunk(int seed, int cx, int cz)
    : seed(seed), cx(cx), cz(cz), blocks(new Block[CHUNK_LENGTH]()) {
    
    generate();
}

Chunk::~Chunk() {
    if (blocks != NULL)
        delete[] blocks;
}

Block Chunk::get(int x, int y, int z) {
    if (x < 0 || y < 0 || z < 0 || x > CHUNK_SIZE - 1 || y > CHUNK_HEIGHT - 1 || z > CHUNK_SIZE - 1) {
        if (x > -CHUNK_SIZE && z > -CHUNK_SIZE && x < CHUNK_SIZE * 2 - 1 && z < CHUNK_SIZE * 2 - 1) {
            if (x < 0 && has_neighbor(ChunkNeighbor::NX))
                return get_neighbor(ChunkNeighbor::NX)->get(x + CHUNK_SIZE, y, z);
            
            if (x > CHUNK_SIZE - 1 && has_neighbor(ChunkNeighbor::PX))
                return get_neighbor(ChunkNeighbor::PX)->get(x - CHUNK_SIZE, y, z);
            
            if (z < 0 && has_neighbor(ChunkNeighbor::NZ))
                return get_neighbor(ChunkNeighbor::NZ)->get(x, y, z + CHUNK_SIZE);
            
            if (z > CHUNK_SIZE - 1 && has_neighbor(ChunkNeighbor::PZ))
                return get_neighbor(ChunkNeighbor::PZ)->get(x, y, z - CHUNK_SIZE);
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

void Chunk::generate() {
    #define DEFAULT_BIOME {Biome::PLAINS, Biome::PLAINS, Biome::PLAINS, Biome::PLAINS, Biome::PLAINS, Biome::PLAINS, Biome::PLAINS, Biome::PLAINS, Biome::PLAINS, Biome::PLAINS, Biome::PLAINS, Biome::PLAINS, Biome::PLAINS, Biome::PLAINS, Biome::PLAINS, Biome::PLAINS}
    #define DEFAULT_BIOME_ROW DEFAULT_BIOME, DEFAULT_BIOME, DEFAULT_BIOME, DEFAULT_BIOME, DEFAULT_BIOME, DEFAULT_BIOME, DEFAULT_BIOME, DEFAULT_BIOME
    Biome biomes[CHUNK_SIZE][CHUNK_SIZE] = {
        DEFAULT_BIOME_ROW, DEFAULT_BIOME_ROW
    };

    generate_biomes(biomes);

    srand(seed + cx * cx & 0xf + cz * cz & 0xf);

    auto heightmap_r = new float[CHUNK_SIZE][CHUNK_SIZE];

    FastNoise::SmartNode<> height_generator = FastNoise::NewFromEncodedNodeTree("IQATAMP1KD8NAAQAAAAAACBACQAAZmYmPwAAAAA/DwADAAAAAAAAQP//AQAAAAAAPwAAAAAAARwAAQcAAAAAoEA=");

    height_generator->GenUniformGrid2D(&heightmap_r[0][0], cx * CHUNK_SIZE + CHUNK_SIZE / 2, cz * CHUNK_SIZE + CHUNK_SIZE / 2, CHUNK_SIZE, CHUNK_SIZE, 0.005f, seed);

    auto heightmap = new float[CHUNK_SIZE][CHUNK_SIZE];
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            heightmap[x][z] = heightmap_r[z][x];
        }
    }

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            int stone_h = 40 + heightmap[x][z] * 10;
            fill(x, 0, z, x, stone_h, z, Material::STONE);

            Biome biome = biomes[x][z];
            if (biome == (Biome)Biome::PLAINS || biome == (Biome)Biome::FOREST) {
                fill(x, stone_h+1, z, x, stone_h+5, z, Material::DIRT);
                set(x, stone_h+6, z, Material::GRASS_BLOCK);

                if (glm::linearRand(0.0f, 1.0f) >= 0.995f) {
                    set(x, stone_h+7, z, Material::FLOWER);
                }
                if (glm::linearRand(0.0f, 1.0f) >= 0.98f) {
                    set(x, stone_h+7, z, Material::GRASS);
                }
                if (biomes[x][z] == (Biome)Biome::FOREST && glm::linearRand(0.0f, 1.0f) > 0.98f) {
                    int treeh = stone_h + 7 + glm::linearRand<int>(3, 6);
                    fill(x-2, stone_h+10, z-2, x+2, treeh+1, z+2, Material::LEAVES);
                    fill(x, stone_h + 7, z, x, treeh, z, Material::LOG);
                }
            } else if (biome == (Biome)Biome::MOUNTAINS) {
                int height = std::clamp(std::pow(std::max(heightmap[x][z], 0.0f) * 10, 2.0f), 6.0f, 100.0f);
                if (height < 70)
                    fill(x, stone_h, z, x, stone_h + height, z, Material::STONE);
                else {
                    fill(x, stone_h, z, x, stone_h + height, z, Material::STONE);
                    fill(x, stone_h + 70 - heightmap[x][z] * 2, z, x, stone_h + height, z, Material::SNOW);
                }
            } else if (biome == (Biome)Biome::DESERT) {
                fill(x, stone_h+1, z, x, stone_h+6, z, Material::SAND);

                if (glm::linearRand(0.0f, 1.0f) >= 0.99f) {
                    set(x, stone_h+7, z, Material::DEAD_BUSH);
                }
            }
        }
    }

    delete[] heightmap;
}

void Chunk::update() {
    for (auto& [pos, block] : Chunk::blocks_to_set[{cx, cz}]) {
        set(pos, block);
    }
    if (blocks_to_set.find({cx, cz}) != blocks_to_set.end())
        Chunk::blocks_to_set.erase({cx, cz});
}

std::shared_ptr<MeshData> Chunk::mesh(const TextureArray& tex) {
    std::vector<float> vertices;

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                int i = BP(x, y, z);
                Block& block = blocks[i];

                if (block == Material::AIR)
                    continue;
                
                if (block.plant()) {
                    #define PLANT_PART_SIZE 10

                    #define ADD_PLANT_PART(texture, block, data, face, part, x, y, z) \
                        data[PLANT_PART_SIZE * part + 0] = plant_faces[face][5 * part + 0] + x; \
                        data[PLANT_PART_SIZE * part + 1] = plant_faces[face][5 * part + 1] + y; \
                        data[PLANT_PART_SIZE * part + 2] = plant_faces[face][5 * part + 2] + z; \
                        data[PLANT_PART_SIZE * part + 3] = plant_faces[face][5 * part + 3]; \
                        data[PLANT_PART_SIZE * part + 4] = plant_faces[face][5 * part + 4]; \
                        data[PLANT_PART_SIZE * part + 5] = tex.position(block.face_texture(face)); \
                        data[PLANT_PART_SIZE * part + 6] = plant_normals[face][0]; \
                        data[PLANT_PART_SIZE * part + 7] = plant_normals[face][1]; \
                        data[PLANT_PART_SIZE * part + 8] = plant_normals[face][2]; \
                        data[PLANT_PART_SIZE * part + 9] = (float)block.material
                    
                    #define ADD_PLANT(vertices, texture, block, data, face, x, y, z) \
                        ADD_PLANT_PART(texture, block, data, face, 0, x, y, z); \
                        ADD_PLANT_PART(texture, block, data, face, 1, x, y, z); \
                        ADD_PLANT_PART(texture, block, data, face, 2, x, y, z); \
                        ADD_PLANT_PART(texture, block, data, face, 3, x, y, z); \
                        ADD_PLANT_PART(texture, block, data, face, 4, x, y, z); \
                        ADD_PLANT_PART(texture, block, data, face, 5, x, y, z); \
                        vertices.insert(vertices.end(), &data[0], &data[PLANT_PART_SIZE * 6])
                    
                    float plant_data[PLANT_PART_SIZE * 6];

                    ADD_PLANT(vertices, tex, block, plant_data, 0, x, y, z);
                    ADD_PLANT(vertices, tex, block, plant_data, 1, x, y, z);
                    ADD_PLANT(vertices, tex, block, plant_data, 2, x, y, z);
                    ADD_PLANT(vertices, tex, block, plant_data, 3, x, y, z);

                    continue;
                }
                
                #define FACE_PART_SIZE 10

                #define ADD_FACE_PART(texture, block, data, face, part, x, y, z) \
                    data[FACE_PART_SIZE * part + 0] = block_faces[face][5 * part + 0] + x; \
                    data[FACE_PART_SIZE * part + 1] = block_faces[face][5 * part + 1] + y; \
                    data[FACE_PART_SIZE * part + 2] = block_faces[face][5 * part + 2] + z; \
                    data[FACE_PART_SIZE * part + 3] = block_faces[face][5 * part + 3]; \
                    data[FACE_PART_SIZE * part + 4] = block_faces[face][5 * part + 4]; \
                    data[FACE_PART_SIZE * part + 5] = tex.position(block.face_texture(face)); \
                    data[FACE_PART_SIZE * part + 6] = block_normals[face][0]; \
                    data[FACE_PART_SIZE * part + 7] = block_normals[face][1]; \
                    data[FACE_PART_SIZE * part + 8] = block_normals[face][2]; \
                    data[FACE_PART_SIZE * part + 9] = (float)block.material

                #define ADD_FACE(vertices, texture, block, data, face, x, y, z) \
                    ADD_FACE_PART(texture, block, face_data, face, 0, x, y, z); \
                    ADD_FACE_PART(texture, block, face_data, face, 1, x, y, z); \
                    ADD_FACE_PART(texture, block, face_data, face, 2, x, y, z); \
                    ADD_FACE_PART(texture, block, face_data, face, 3, x, y, z); \
                    ADD_FACE_PART(texture, block, face_data, face, 4, x, y, z); \
                    ADD_FACE_PART(texture, block, face_data, face, 5, x, y, z); \
                    vertices.insert(vertices.end(), &data[0], &data[FACE_PART_SIZE * 6])
                
                #define TRANSPARENT_CHECK(xo, yo, zo) \
                    (block.transparent() ? get(x + xo, y + yo, z + zo) != block : get(x + xo, y + yo, z + zo).transparent())

                float face_data[FACE_PART_SIZE * 6];

                if (TRANSPARENT_CHECK(-1, 0, 0)) {
                    ADD_FACE(vertices, tex, block, face_data, 2, x, y, z);
                }
                if (TRANSPARENT_CHECK(1, 0, 0)) {
                    ADD_FACE(vertices, tex, block, face_data, 3, x, y, z);
                }

                if (TRANSPARENT_CHECK(0, 0, -1)) {
                    ADD_FACE(vertices, tex, block, face_data, 0, x, y, z);
                }
                if (TRANSPARENT_CHECK(0, 0, 1)) {
                    ADD_FACE(vertices, tex, block, face_data, 1, x, y, z);
                }

                if (y - 1 < 0 || TRANSPARENT_CHECK(0, -1, 0)) {
                    ADD_FACE(vertices, tex, block, face_data, 4, x, y, z);
                }
                if (y + 2 > CHUNK_HEIGHT || TRANSPARENT_CHECK(0, 1, 0)) {
                    ADD_FACE(vertices, tex, block, face_data, 5, x, y, z);
                }
            }
        }
    }

    return std::make_shared<MeshData>(new MeshData({3, 3, 3, 1}, vertices));
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

void Chunk::generate_biomes(Biome output[CHUNK_SIZE][CHUNK_SIZE]) {
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            output[x][z] = Biome::_from_integral((glm::simplex(glm::vec3((cx * CHUNK_SIZE + x) / 512.0f, (cz * CHUNK_SIZE + z) / 512.0f, seed)) + 1.0f) / 2.0f * (Biome::_size()));
        }
    }
}

ChunkPosition Chunk::position() {
    return {cx, cz};
}
