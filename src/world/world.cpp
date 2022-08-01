#include "world.hpp"

#define RENDER_DISTANCE 8
// #define CHUNK_MESH_ID(cx, cz) (0xff00000000 | (1 << 14 - (cx)) << 16 | (1 << 14 - (cz)))

static int CHUNK_MESH_ID(int cx, int cz) {
    return (1 << 10 - cx) << 12 | (1 << 10 - cz);
}

World::World(const WorldOptions& options)
    : opt(options) {
    
    render_distance(RENDER_DISTANCE);
    generate();
}

void World::move(int x, int z) {
    this->x = x;
    this->z = z;

    load_chunks.clear();
    for (int x_ = -rdist + x; x_ <= rdist + x; x_++) {
        for (int z_ = -rdist + z; z_ <= rdist + z; z_++) {
            load_chunks.insert({x_, z_});
        }
    }

    generate();
}
void World::generate() {
    std::vector<std::pair<int, int>> unload;
    for (auto& [pos, pair] : chunks) {
        if (!pair.first)
            continue;
        
        // DEBUG
        continue;
        
        if (!load_chunks.contains(pos)) {
            opt.renderer->remove_mesh(CHUNK_MESH_ID(pos.first, pos.second));
            unload.push_back(pos);

            // DEBUG
            std::cout << "Unloading chunk: " << pos.first << ", " << pos.second << "\n";
        }
    }
    for (auto& pos : unload) {
        delete chunks[pos].second;
        chunks.erase(pos);
    }

    for (auto& pos : load_chunks) {
        if (!chunks[pos].first && std::find(chunk_queue.begin(), chunk_queue.end(), pos) == chunk_queue.end()) {
            chunk_queue.push_back(pos);
        }
    }

    if (!chunk_queue.empty()) {
        auto pos = chunk_queue.back();
        chunk_queue.pop_back();

        chunks[pos] = {true, new Chunk(opt.seed, pos.first, pos.second)};
        meshes[pos] = Mesh(chunks[pos].second->mesh(opt.tex3d), opt.shader, opt.textures, opt.textures3d);
        opt.renderer->add_mesh(CHUNK_MESH_ID(pos.first, pos.second), &meshes[pos], glm::vec3(pos.first * CHUNK_SIZE, 0.0f, pos.second * CHUNK_SIZE));

        // DEBUG
        // std::cout << "Loading chunk: " << pos.first << ", " << pos.second << "\n";
        // std::cout << CHUNK_MESH_ID(pos.first, pos.second) << "\n";
        std::cout << x << " | " << z << "\n";
    }
}
void World::render_distance(int distance) {
    rdist = distance;
    move(x, z);
}
