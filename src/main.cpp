#include "game.hpp"

int main() {
    Game game(1024, 768);
    game.Run();
    game.Cleanup();
}

