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
    static constexpr glm::vec4 _framebuf_vertices[] = {
        { -1.0f,  1.0f, 0.0f, 1.0f },
        { -1.0f, -1.0f, 0.0f, 0.0f },
        {  1.0f, -1.0f, 1.0f, 0.0f },

        { -1.0f,  1.0f, 0.0f, 1.0f },
        {  1.0f, -1.0f, 1.0f, 0.0f },
        {  1.0f,  1.0f, 1.0f, 1.0f }
    };
    i16 _window_width, _window_height;
    float _delta_time;
    std::string _world_path;
    glw::Framebuffer _framebuf;
    glw::Texture _framebuf_tex;
    glw::Renderbuffer _renderbuf;
    glw::VertexArrayObject _framebuf_vao;
    glw::VertexBuffer<glm::vec4> _framebuf_vbo;
    glw::Shader _framebuf_shader;
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

