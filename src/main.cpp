#include "../vendor/simdjson.h"
#include "game.hpp"

int main() {
    RunTests();

    Game game(1024, 768);
    game.Run();
    game.Cleanup();
}

