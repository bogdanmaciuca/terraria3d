#pragma once
#include "int.hpp"
#include <vector>
#include "gameobjects.hpp"

static const char* DATA_JSON_FILENAME = "./assets/data.json";

struct Game {
    Game(i16 window_width, i16 window_height);
    void Run();
    void Cleanup();
private:
    std::vector<Item> items;
    void LoadDataJSON();
};

