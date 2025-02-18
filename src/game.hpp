#pragma once
#include <vector>
#include "int.hpp"
#include "glw.hpp"
#include "gameobjects.hpp"
#include "terrain.hpp"
#include "surface_nets.hpp"

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
        float nx, ny, nz;
    };
    float light_theta = 0;
    std::vector<Item> items;
    glw::Shader shader;
    glw::Shader debug_shader;
    glw::Camera camera;
    glw::VertexArrayObject grid_vao;
    glw::VertexArrayObject points_vao;
    glw::VertexArrayObject nets_vao;
    glw::VertexBuffer<glm::vec3> grid_vbo;
    glw::VertexBuffer<glm::vec3> points_vbo;
    glw::VertexBuffer<glm::vec3> nets_vbo;
    glw::IndexBuffer<u32> nets_ibo;
    SurfaceNets<glm::vec3, u32> surface_nets;
    Chunk chunk;
    void LoadDataJSON();
};

