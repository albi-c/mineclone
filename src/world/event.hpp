#include <memory>

#include "chunk.hpp"
#include "event/event.hpp"

struct EventChunkLoad : Event {
    std::shared_ptr<Chunk> chunk;
    int cx, cz;

    EventChunkLoad(std::shared_ptr<Chunk> chunk, int cx, int cz)
        : chunk(chunk), cx(cx), cz(cz) {}
};
struct EventChunkUnload : Event {
    int cx, cz;

    EventChunkUnload(int cx, int cz)
        : cx(cx), cz(cz) {}
};
