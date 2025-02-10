#pragma once
#include "int.hpp"
#include <vector>
#include "glw.hpp"
#include "gameobjects.hpp"

struct Game {
    Game(i16 window_width, i16 window_height);
    void Run();
    void Cleanup();
private:
    std::vector<Item> items;
    glw::Shader shader;
    void LoadDataJSON();
};

