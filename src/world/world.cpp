#include "world.hpp"

World::World(int seed, unsigned int render_distance, int x, int z)
    : seed(seed), render_distance(render_distance), x(x), z(z) {
    
    move(x, z);
}

void World::update() {
    for (auto& [pos, chunk] : chunks) {
        chunk->update();
    }
}
void World::update_loaded() {
    required_chunks.clear();

    auto cpos = wu::chunk_pos(x, z);
    int cx = cpos.first;
    int cz = cpos.second;
    for (int x = cx - (int)render_distance; x <= cx + (int)render_distance; x++) {
        for (int z = cz - (int)render_distance; z <= cz + (int)render_distance; z++) {
            required_chunks.insert({x, z});
        }
    }
    std::set<std::pair<int, int>> unloaded_chunks;
    for (auto& [pos, chunk] : chunks) {
        if (!required_chunks.contains(pos)) {
            unloaded_chunks.insert(pos);
        } else {
            required_chunks.erase(pos);
        }
    }
    for (auto& pos : unloaded_chunks) {
        chunks.erase(pos);
        EventManager::fire(EventChunkUnload{pos.first, pos.second});
    }
}
void World::generate(const TextureArray& texture_array) {
    if (!required_chunks.empty()) {
        auto pos = *required_chunks.end();
        required_chunks.erase(pos);

        chunks[pos] = std::make_shared<Chunk>(new Chunk(seed, pos.first, pos.second));

        EventManager::fire(EventChunkLoad{chunks[pos], chunks[pos]->mesh(texture_array), pos.first, pos.second});
    }
}

void World::move(int x, int z) {
    this->x = x;
    this->z = z;

    update_loaded();
}

void World::set_render_distance(unsigned int render_distance) {
    this->render_distance = render_distance;

    update_loaded();
}

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
