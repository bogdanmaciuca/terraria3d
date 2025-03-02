#include "game.hpp"
#include "chunk.hpp"
#include "int.hpp"
#include <iostream>
#include <string>

int main(i32 argc, char** argv) {
    std::string world_path;
    if (argc > 2) {
        if (argv[1] == std::string("create")) {
            CreateWorld(argv[2]);
            world_path = argv[2];
        }
        else {
            std::cout << "Wrong argument.\n";
            exit(1);
        }
    }
    else if (argc != 2) {
        std::cout << "Wrong number of arguments!\n";
        exit(1);
    }
    else
        world_path = argv[1];

    Game game(1024, 768, world_path);
    game.Run();
}

