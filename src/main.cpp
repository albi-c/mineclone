#include "game/game.hpp"
#include "util/time.hpp"
#include <ctime>

int main() {
    srand(time(0));

    Game::init();
    Game::run();

    Timer::end();

    return 0;
}
