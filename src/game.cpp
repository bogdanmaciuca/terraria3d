#include "game.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
#include "int.hpp"
#include "glw.hpp"
#include "debug.hpp"

Game::Game(i32 window_width, i32 window_height, const std::string& world_path) {
    _window_width = window_width; _window_height = window_height;
    _game_width = _window_width / core::Pixelization;
    _game_height = _window_height / core::Pixelization;
    _world_path = std::string(world_path);
    glw::Initialize(window_width, window_height);
    _framebuf.Initialize();
    _framebuf.Bind();
    _framebuf_tex.Initialize(_game_width, _game_height);
    _framebuf.AddTexture(_framebuf_tex);
    _renderbuf.Initialize(_game_width, _game_height);
    _renderbuf.AttachToFramebuffer();
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    _framebuf_shader.Source(
        "./assets/shaders/framebuffer_vert.glsl",
        "./assets/shaders/framebuffer_frag.glsl"
    );
    _framebuf_shader.Compile();
    _framebuf_shader.Bind();
    _framebuf_shader.SetInt("screenTexture", 0);

    _framebuf_vao.Initialize(sizeof(glm::vec4));
    _framebuf_vao.Bind();
    _framebuf_vbo.Initialize();
    _framebuf_vbo.Bind();
    _framebuf_vbo.Source(
        &_framebuf_vertices[0], sizeof(_framebuf_vertices),
        sizeof(_framebuf_vertices) / sizeof(glm::vec4)
    );
    _framebuf_vbo.Bind();
    _framebuf_vao.AddAttrib(GL_FLOAT, 2); // Position
    _framebuf_vao.AddAttrib(GL_FLOAT, 2); // UV coords

    Initialize();
}
Game::~Game() {
    glw::Cleanup();
}

void Game::Run() {
    double now = glfwGetTime();
    while (!glfwWindowShouldClose(glw::window)) {
        double then = now;
        now = glfwGetTime();
        _delta_time = now - then;

        glEnable(GL_DEPTH_TEST);
        glViewport( 0, 0, _game_width, _game_height);
        _framebuf.Bind();
        RenderFrame();
        _framebuf.BindDefault();
        glDisable(GL_DEPTH_TEST);
        glViewport(0, 0, _window_width, _window_height);

        _framebuf_shader.Bind();
        _framebuf_vao.Bind();
        _framebuf_tex.Bind();
        glw::Draw(_framebuf_vbo.Length());

        glfwSwapBuffers(glw::window);
        glfwPollEvents();

        if (glfwGetKey(glw::window, GLFW_KEY_R)) {
            shader.Reload();
        }
        UpdateLogic();
    }
}

void Game::Initialize() {
    chunk_manager.Initialize(_world_path);

    shader.Source("./assets/shaders/vert.glsl", "./assets/shaders/frag.glsl");
    shader.Compile();

    debug_shader.Source("./assets/shaders/debug_vert.glsl", "./assets/shaders/debug_frag.glsl");
    debug_shader.Compile();

    camera.Initialize(77.0f, (float)_window_width / (float)_window_height);
}

void Game::RenderFrame() {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.Bind();
    shader.SetMat4("uViewProj", camera.GetViewProjection());
    shader.SetVec3("uLightPos", camera.pos);
    debug_shader.Bind();
    debug_shader.SetMat4("uViewProj", camera.GetViewProjection());
    for (auto& it : chunk_manager.GetChunks()) {
        shader.Bind();
        shader.SetMat4("uModel", glm::translate(glm::mat4(1), (float)(core::ChunkSize - 2) * glm::vec3(it.first)));
        it.second.Render();
        //it.second._VAO.Bind();
        //debug_shader.Bind();
        //debug_shader.SetMat4("uModel", glm::translate(glm::mat4(1), (float)(core::ChunkSize - 2) * glm::vec3(it.first)));
        //glw::Draw(it.second._VBO.Length(), GL_POINTS);
    }
}

void Game::UpdateLogic() {
    camera.ProcessMouse();
    if (glfwGetKey(glw::window, GLFW_KEY_W))
        camera.Move(glw::Camera::Forward, static_cast<float>(_delta_time));
    if (glfwGetKey(glw::window, GLFW_KEY_S))
        camera.Move(glw::Camera::Backward, static_cast<float>(_delta_time));
    if (glfwGetKey(glw::window, GLFW_KEY_A))
        camera.Move(glw::Camera::Left, static_cast<float>(_delta_time));
    if (glfwGetKey(glw::window, GLFW_KEY_D))
        camera.Move(glw::Camera::Right, static_cast<float>(_delta_time));
    if (glfwGetKey(glw::window, GLFW_KEY_LEFT_SHIFT))
        camera.speed = 24.0f;
    else
        camera.speed = 64.0f;
    chunk_manager.UpdateChunks(camera.pos);
}

