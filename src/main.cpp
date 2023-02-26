#include "game/game.hpp"
#include "util/time.hpp"
#include "world/chunk.hpp"
#include <ctime>

int main() {
    srand(time(0));

    Chunk::init();

    Game::init();
    Game::run();

    Timer::end();

    return 0;
}
