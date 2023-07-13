#include <memory>

#include "world/chunk.hpp"
#include "event/event.hpp"
#include "graphics/mesh/mesh.hpp"

struct EventChunkLoad : Event {
    std::shared_ptr<Chunk> chunk;
    std::shared_ptr<MeshData<Chunk::MeshType>> mesh_data;
    int cx, cz;

    EventChunkLoad(std::shared_ptr<Chunk> chunk, std::shared_ptr<MeshData<Chunk::MeshType>> mesh_data, int cx, int cz)
        : chunk(chunk), mesh_data(mesh_data), cx(cx), cz(cz) {}
};
struct EventChunkUnload : Event {
    int cx, cz;

    EventChunkUnload(int cx, int cz)
        : cx(cx), cz(cz) {}
};
