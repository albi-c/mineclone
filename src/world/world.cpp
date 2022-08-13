#include "world.hpp"

World::World(int seed)
    : seed(seed) {}

Block World::get(int x, int y, int z) {
    int cx = x % CHUNK_SIZE;
    int cz = z % CHUNK_SIZE;
    if (chunks.find({cx, cz}) != chunks.end()) {
        return chunks[{cx, cz}]->get(x, y, z);
    }
    return Block();
}

void World::set(int x, int y, int z, const Block& block) {
    int cx = x % CHUNK_SIZE;
    int cz = z % CHUNK_SIZE;
    if (chunks.find({cx, cz}) != chunks.end()) {
        chunks[{cx, cz}]->set(x, y, z, block);
    }
}

void World::fill(int x1, int y1, int z1, int x2, int y2, int z2, const Block& block) {
    for (int x = x1; x <= x2; x++) {
        for (int y = y1; y <= y2; y++) {
            for (int z = z1; z <= z2; z++) {
                set(x, y, z, block);
            }
        }
    }
}

std::shared_ptr<Chunk> World::chunk(int x, int z) {
    if (chunks.find({x, z}) != chunks.end()) {
        return chunks[{x, z}];
    }
    return nullptr;
}
