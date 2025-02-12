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
    i16 _window_width, _window_height;
    float _delta_time;
    void Initialize();
    void RenderFrame();
    void UpdateLogic();

    struct Vertex {
        float x, y, z;
    };
    std::vector<Item> items;
    glw::Shader shader;
    glw::Camera camera;
    glw::VertexArrayObject vao;
    glw::VertexBuffer<Vertex> vbo;
    glw::IndexBuffer<u32> ibo;
    void LoadDataJSON();
};

