#pragma once

#define CHUNK_HEIGHT 256
#define CHUNK_SIZE 16
#define CHUNK_LENGTH (CHUNK_SIZE * CHUNK_SIZE * CHUNK_HEIGHT)

#include <utility>
#include <array>
#include <map>
#include <memory>

#include "world/block.hpp"
#include "world/biome.hpp"
#include "event/event.hpp"
#include "graphics/mesh/mesh.hpp"
#include "lib/opengl.hpp"
#include "graphics/textures/texture_array.hpp"

enum class ChunkNeighbor {
    PX = 0,
    NX,
    PZ,
    NZ
};

struct ChunkPosition {
    int x, z;

    ChunkPosition() {}
    ChunkPosition(int x, int z)
        : x(x), z(z){}
    
    operator std::pair<int, int>() const {
        return {x, z};
    }
};

class Chunk {
public:
    Chunk();
    Chunk(Chunk* other);
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

    std::shared_ptr<MeshData> mesh(const TextureArray& tex);

    bool has_neighbor(ChunkNeighbor neighbor);
    std::shared_ptr<Chunk> get_neighbor(ChunkNeighbor neighbor);
    void set_neighbor(ChunkNeighbor neighbor, std::shared_ptr<Chunk> chunk);

    ChunkPosition position();

private:
    int cx, cz;
    int seed;

    // XZY order
    Block* blocks;

    std::weak_ptr<Chunk> neighbors[4];

    static std::map<std::pair<int, int>, std::vector<std::pair<BlockPosition, Block>>> blocks_to_set;

    void generate_biomes(Biome output[CHUNK_SIZE][CHUNK_SIZE]);
};

struct EventChunkRedraw : Event {
    int cx, cz;

    EventChunkRedraw(int cx, int cz)
        : cx(cx), cz(cz) {}
};
