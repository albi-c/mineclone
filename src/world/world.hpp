#pragma once

#include <memory>
#include <vector>
#include <map>
#include <set>

#include "chunk.hpp"
#include "block.hpp"
#include "event.hpp"
#include "util/world.hpp"

class World;
struct WorldHandlers {
    static inline World* world;

    static void chunk_redraw_event_handler(const EventChunkRedraw& e);

    FunctionEventQueue<EventChunkRedraw> chunk_redraw_event_queue = FunctionEventQueue<EventChunkRedraw>(WorldHandlers::chunk_redraw_event_handler);
};

class World {
public:
    World(World* other);
    World(int seed, unsigned int render_distance = 1, int x = 0, int z = 0);

    void update();
    void update_loaded();
    void generate(const TextureArray& texture_array);

    void move(int x, int z);

    void set_render_distance(unsigned int distance);

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
    inline std::shared_ptr<Chunk> chunk(const ChunkPosition& pos) {
        return chunk(pos.x, pos.z);
    }
    inline std::shared_ptr<Chunk> chunk(const std::pair<int, int>& pos) {
        return chunk(pos.first, pos.second);
    }

    bool chunk_exists(int x, int z);
    inline bool chunk_exists(const ChunkPosition& pos) {
        return chunk_exists(pos.x, pos.z);
    }
    inline bool chunk_exists(const std::pair<int, int>& pos) {
        return chunk_exists(pos.first, pos.second);
    }

private:
    int seed;
    int x, z;

    unsigned int render_distance = 1;

    std::set<std::pair<int, int>> required_chunks;
    std::set<std::pair<int, int>> required_chunk_meshes;
    std::map<std::pair<int, int>, std::shared_ptr<Chunk>> chunks;

    friend struct WorldHandlers;
    WorldHandlers handlers;

    void update_neighbors(const std::pair<int, int>& pos);
};
