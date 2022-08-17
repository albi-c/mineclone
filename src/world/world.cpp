#include "world.hpp"

void WorldHandlers::chunk_redraw_event_handler(const EventChunkRedraw& e) {
    if (world->chunks.find({e.cx, e.cz}) != world->chunks.end())
        world->required_chunk_meshes.insert({e.cx, e.cz});
}

World::World(World* other)
    : seed(other->seed), x(other->x), z(other->z), render_distance(other->render_distance), required_chunks(other->required_chunks), chunks(other->chunks) {
    
    WorldHandlers::world = this;
    EventManager::listen(handlers.chunk_redraw_event_queue);
}
World::World(int seed, unsigned int render_distance, int x, int z)
    : seed(seed), render_distance(render_distance), x(x), z(z) {
    
    WorldHandlers::world = this;
    EventManager::listen(handlers.chunk_redraw_event_queue);

    move(x, z);
}

void World::update() {
    for (auto& [pos, chunk] : chunks) {
        chunk->update();
    }

    handlers.chunk_redraw_event_queue.process();
}
void World::update_loaded() {
    required_chunks.clear();
    required_chunk_meshes.clear();

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
        auto pos = *(--required_chunks.end());
        required_chunks.erase(pos);

        auto chunk = std::make_shared<Chunk>(new Chunk(seed, pos.first, pos.second));
        chunks[pos] = chunk;

        update_neighbors(pos);

        EventManager::fire(EventChunkLoad{chunk, chunk->mesh(texture_array), pos.first, pos.second});

        return;
    }
    if (!required_chunk_meshes.empty()) {
        auto pos = *(--required_chunk_meshes.end());
        required_chunk_meshes.erase(pos);

        auto c = chunk(pos);
        if (c) {
            EventManager::fire(EventChunkLoad{c, c->mesh(texture_array), pos.first, pos.second});
        }
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
    auto cpos = wu::chunk_pos(x, z);
    int cx = cpos.first;
    int cz = cpos.second;
    if (chunks.find({cx, cz}) != chunks.end()) {
        return chunks[{cx, cz}]->get(x, y, z);
        // TODO: x and z should be transformed to chunk coordinates
    }
    return Block();
}

void World::set(int x, int y, int z, const Block& block) {
    auto cpos = wu::chunk_pos(x, z);
    int cx = cpos.first;
    int cz = cpos.second;
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

bool World::chunk_exists(int x, int z) {
    return chunks.find({x, z}) != chunks.end();
}

void World::update_neighbors(const std::pair<int, int>& pos) {
    auto c = chunk(pos);
    
    auto c_nx = chunk(pos.first - 1, pos.second);
    if (c_nx) {
        c_nx->set_neighbor(ChunkNeighbor::PX, c);
        c->set_neighbor(ChunkNeighbor::NX, c_nx);
    }

    auto c_px = chunk(pos.first + 1, pos.second);
    if (c_px) {
        c_px->set_neighbor(ChunkNeighbor::NX, c);
        c->set_neighbor(ChunkNeighbor::PX, c_px);
    }

    auto c_nz = chunk(pos.first, pos.second - 1);
    if (c_nz) {
        c_nz->set_neighbor(ChunkNeighbor::PZ, c);
        c->set_neighbor(ChunkNeighbor::NZ, c_nz);
    }
    
    auto c_pz = chunk(pos.first, pos.second + 1);
    if (c_pz) {
        c_pz->set_neighbor(ChunkNeighbor::NZ, c);
        c->set_neighbor(ChunkNeighbor::PZ, c_pz);
    }
}
