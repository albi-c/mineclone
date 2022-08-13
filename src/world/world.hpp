#pragma once

#include <memory>
#include <vector>
#include <map>

#include "chunk.hpp"
#include "block.hpp"

class World {
public:
    World(int seed);

    Block get(int x, int y, int z);
    inline Block get(const BlockPosition& pos) {
        return get(pos.x, pos.y, pos.z);
    }
    
    void set(int x, int y, int z, const Block& block);
    inline void set(const BlockPosition& pos, const Block& block) {
        set(pos.x, pos.y, pos.z, block);
    }
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
    inline void fill(const BlockPosition& pos1, const BlockPosition& pos2, const Block& block) {
        fill(pos1.x, pos1.y, pos1.z, pos2.x, pos2.y, pos2.z, block);
    }
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

    std::shared_ptr<Chunk> chunk(int x, int z);
    inline std::shared_ptr<Chunk> chunk(ChunkPosition pos) {
        return chunk(pos.x, pos.z);
    }

private:
    int seed;

    std::map<std::pair<int, int>, std::shared_ptr<Chunk>> chunks;
};
