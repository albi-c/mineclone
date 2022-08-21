#include "world.hpp"

void WorldHandlers::chunk_redraw_event_handler(const EventChunkRedraw& e) {
    if (world->chunks.find({e.cx, e.cz}) != world->chunks.end()) {
        world->required_chunk_meshes_lp.insert({e.cx, e.cz});
    }
}

World::World(World* other)
    : seed(other->seed),
    x(other->x), z(other->z),
    render_distance(other->render_distance),
    required_chunks(other->required_chunks),
    chunks(other->chunks),
    texture_array(other->texture_array) {
    
    WorldHandlers::world = this;
    EventManager::listen(handlers.chunk_redraw_event_queue);
}
World::World(int seed, std::shared_ptr<TextureArray> texture_array, unsigned int render_distance, int x, int z)
    : seed(seed), texture_array(texture_array), render_distance(render_distance), x(x), z(z) {
    
    WorldHandlers::world = this;
    EventManager::listen(handlers.chunk_redraw_event_queue);

    move(x, z);
}

void World::update() {
    tu::mutex_lock_timeout_exc(chunks_mutex);
    for (auto& [pos, chunk] : chunks) {
        chunk->update();
    }
    chunks_mutex.unlock();

    tu::mutex_lock_timeout_exc(required_chunk_meshes_mutex);
    handlers.chunk_redraw_event_queue.process();
    required_chunk_meshes_mutex.unlock();
}
void World::update_loaded() {
    tu::mutex_lock_timeout_exc(required_chunks_mutex);

    required_chunks = std::queue<std::pair<int, int>>();

    auto cpos = wu::chunk_pos(x, z);
    int cx = cpos.first;
    int cz = cpos.second;
    auto required_chunks_spiral = wu::spiral(cpos, render_distance);
    std::vector<std::pair<int, int>> required_chunks_tmp;
    std::set<std::pair<int, int>> unloaded_chunks;
    for (auto& [pos, chunk] : chunks) {
        for (auto& pos_ : required_chunks_spiral) {
            if (pos_ == pos) {
                goto is_loaded;
            }
        }
        unloaded_chunks.insert(pos);
        is_loaded:;
    }
    for (auto& pos : unloaded_chunks) {
        chunks.erase(pos);
        EventManager::fire(EventChunkUnload{pos.first, pos.second});
    }
    for (auto& pos : required_chunks_spiral) {
        for (auto& [pos_, chunk] : chunks) {
            if (pos == pos_) {
                goto already_loaded;
            }
        }
        required_chunks.push(pos);
        continue;
        already_loaded:;
    }

    required_chunks_mutex.unlock();
}
void World::generate(const TextureArray& texture_array) {
    tu::mutex_lock_timeout_exc(required_chunks_mutex);
    if (!required_chunks.empty()) {
        auto pos = required_chunks.front();
        required_chunks.pop();

        required_chunks_mutex.unlock();

        auto chunk = std::make_shared<Chunk>(new Chunk(seed, pos.first, pos.second));

        tu::mutex_lock_timeout_exc(chunks_mutex);
        chunks[pos] = chunk;
        chunks_mutex.unlock();

        update_neighbors({pos.first, pos.second});

        tu::mutex_lock_timeout_exc(required_chunk_meshes_mutex);
        required_chunk_meshes.push(pos);
        required_chunk_meshes_mutex.unlock();
    } else {
        required_chunks_mutex.unlock();
    }

    tu::mutex_lock_timeout_exc(required_chunk_meshes_mutex);
    if (!required_chunk_meshes.empty()) {
        auto pos = required_chunk_meshes.front();
        required_chunk_meshes.pop();

        required_chunk_meshes_mutex.unlock();

        auto c = chunk(glm::ivec2{pos.first, pos.second});
        if (c) {
            EventManager::fire(EventChunkLoad{c, c->mesh(texture_array), pos.first, pos.second});
        }
    } else {
        required_chunk_meshes_mutex.unlock();

        tu::mutex_lock_timeout_exc(required_chunk_meshes_lp_mutex);
        if (!required_chunk_meshes_lp.empty()) {
            auto pos = *(--required_chunk_meshes_lp.end());
            required_chunk_meshes_lp.erase(pos);

            required_chunk_meshes_lp_mutex.unlock();

            auto c = chunk(glm::ivec2{pos.first, pos.second});
            if (c) {
                EventManager::fire(EventChunkLoad{c, c->mesh(texture_array), pos.first, pos.second});
            }
        } else {
            required_chunk_meshes_lp_mutex.unlock();
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

Block World::raycast(Ray& ray, float range) {
    while (ray.distance() < range) {
        ray.step(0.01f);
        Block b = get(ray.position());
        if (b)
            return b;
    }
    return Block();
}

Block World::get(int x, int y, int z) {
    auto cpos = wu::chunk_pos(x, z);
    int cx = cpos.first;
    int cz = cpos.second;
    if (chunks.find({cx, cz}) != chunks.end()) {
        return chunks[{cx, cz}]->get(x & 0xf, y, z & 0xf);
    }
    return Block();
}

void World::set(int x, int y, int z, const Block& block) {
    auto cpos = wu::chunk_pos(x, z);
    int cx = cpos.first;
    int cz = cpos.second;
    if (chunks.find({cx, cz}) != chunks.end()) {
        chunks[{cx, cz}]->set(x & 0xf, y, z & 0xf, block);
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

void World::update_neighbors(const glm::ivec2& pos) {
    auto c = chunk(pos);
    
    auto c_nx = chunk(pos.x - 1, pos.y);
    if (c_nx) {
        c_nx->set_neighbor(ChunkNeighbor::PX, c);
        c->set_neighbor(ChunkNeighbor::NX, c_nx);
    }

    auto c_px = chunk(pos.x + 1, pos.y);
    if (c_px) {
        c_px->set_neighbor(ChunkNeighbor::NX, c);
        c->set_neighbor(ChunkNeighbor::PX, c_px);
    }

    auto c_nz = chunk(pos.x, pos.y - 1);
    if (c_nz) {
        c_nz->set_neighbor(ChunkNeighbor::PZ, c);
        c->set_neighbor(ChunkNeighbor::NZ, c_nz);
    }
    
    auto c_pz = chunk(pos.x, pos.y + 1);
    if (c_pz) {
        c_pz->set_neighbor(ChunkNeighbor::NZ, c);
        c->set_neighbor(ChunkNeighbor::PZ, c_pz);
    }
}

bool World::loaded(int x, int z) {
    return chunks.find({x, z}) != chunks.end();
}

int World::highest_block(int x, int z) {
    auto cpos = wu::chunk_pos(x, z);
    int cx = cpos.first;
    int cz = cpos.second;
    auto c = chunk(cx, cz);
    if (c) {
        return c->highest_block(x, z);
    }
    return CHUNK_HEIGHT - 1;
}
