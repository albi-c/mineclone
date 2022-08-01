#pragma once

#include <map>
#include <utility>
#include <vector>
#include <set>
#include <algorithm>

#include "chunk.hpp"
#include "../graphics/texture.hpp"
#include "../graphics/shader.hpp"
#include "../graphics/renderer.hpp"

struct WorldOptions {
    int seed;
    Texture3D* tex3d;
    Shader* shader;
    std::map<std::string, Texture*> textures;
    std::map<std::string, Texture3D*> textures3d;
    Renderer* renderer;
};

class World {
public:
    World(const WorldOptions& options);

    void move(int x, int z);
    void generate();
    void render_distance(int distance);

private:
    WorldOptions opt;

    std::set<std::pair<int, int>> load_chunks;
    std::vector<std::pair<int, int>> chunk_queue;
    std::map<std::pair<int, int>, std::pair<bool, Chunk*>> chunks;
    std::map<std::pair<int, int>, Mesh> meshes;
    int x = 0, z = 0;
    int rdist;
};
