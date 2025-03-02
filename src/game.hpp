#pragma once
#include <string>
#include "int.hpp"
#include "glw.hpp"
#include "gameobjects.hpp"
#include "surface_nets.hpp"
#include "chunk.hpp"

struct Game {
    Game(i16 window_width, i16 window_height, const std::string& world_path);
    ~Game();
    void Run();
    void Cleanup();
private:
    i16 _window_width, _window_height;
    float _delta_time;
    std::string _world_path;
    void Initialize();
    void RenderFrame();
    void UpdateLogic();

    float light_theta = 0;
    std::vector<Item> items;
    glw::Shader shader;
    glw::Shader debug_shader;
    glw::Camera camera;
    ChunkManager chunk_manager;
    void LoadDataJSON();
};

