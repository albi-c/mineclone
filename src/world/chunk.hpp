#pragma once

#define CHUNK_HEIGHT 256
#define CHUNK_SIZE 16
#define CHUNK_LENGTH (CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT)

#include <utility>
#include <array>
#include <map>

#include "block.hpp"
#include "biome.hpp"
#include "../event/event.hpp"
#include "../graphics/mesh.hpp"
#include "../graphics/opengl.hpp"
#include "../graphics/texture.hpp"

enum class ChunkNeighbor {
    PX = 0,
    NX,
    PZ,
    NZ
};

class Chunk {
public:
    Chunk();
    Chunk(int seed, int cx, int cz);
    ~Chunk();

    Block get(int x, int y, int z);
    Block get(const BlockPosition& pos);

    void set(int x, int y, int z, const Block& block);
    void set(const BlockPosition& pos, const Block& block);
    inline void set(int x, int y, int z, const Material& mat) {
        set(x, y, z, Block(mat));
    }
    inline void set(const BlockPosition& pos, const Material& mat) {
        set(pos.x, pos.y, pos.z, Block(mat));
    }
    inline void set(int x, int y, int z, const Material::_enumerated& mat) {
        set(x, y, z, Block(mat));
    }
    inline void set(const BlockPosition& pos, const Material::_enumerated& mat) {
        set(pos.x, pos.y, pos.z, Block(mat));
    }

    void fill(int x1, int y1, int z1, int x2, int y2, int z2, const Block& block);
    void fill(const BlockPosition& pos1, const BlockPosition& pos2, const Block& block);
    inline void fill(int x1, int y1, int z1, int x2, int y2, int z2, const Material& mat) {
        fill(x1, y1, z1, x2, y2, z2, Block(mat));
    }
    inline void fill(const BlockPosition& pos1, const BlockPosition& pos2, const Material& mat) {
        fill(pos1.x, pos1.y, pos1.z, pos2.x, pos2.y, pos2.z, Block(mat));
    }
    inline void fill(int x1, int y1, int z1, int x2, int y2, int z2, const Material::_enumerated& mat) {
        fill(x1, y1, z1, x2, y2, z2, Block(mat));
    }
    inline void fill(const BlockPosition& pos1, const BlockPosition& pos2, const Material::_enumerated& mat) {
        fill(pos1.x, pos1.y, pos1.z, pos2.x, pos2.y, pos2.z, Block(mat));
    }

    void generate();

    void update();

    MeshData mesh(Texture3D* tex);

    inline Chunk* get_neighbor(ChunkNeighbor neighbor) {
        return neighbors[(int)neighbor];
    }
    inline void set_neighbor(ChunkNeighbor neighbor, Chunk* chunk) {
        neighbors[(int)neighbor] = chunk;
    }

private:
    int cx, cz;
    int seed;

    // XZY order
    Block* blocks;

    Chunk* neighbors[4] = {nullptr, nullptr, nullptr, nullptr};

    static std::map<std::pair<int, int>, std::vector<std::pair<BlockPosition, Block>>> blocks_to_set;

    void generate_biomes(Biome output[16][16]);
};
