#include "game.hpp"
#include "../vendor/simdjson/simdjson.h"
#include <string>
#include "glw.hpp"

Game::Game(i16 window_width, i16 window_height) {
    _window_width = window_width; _window_height = window_height;
    glw::Initialize(window_width, window_height);
    Initialize();
}

void Game::Run() {
    double now;
    while (!glfwWindowShouldClose(glw::window)) {
        double then = now;
        now = glfwGetTime();
        _delta_time = now - then;
        RenderFrame();
        glfwSwapBuffers(glw::window);
        glfwPollEvents();
        UpdateLogic();
    }
}

void Game::Initialize() {
    LoadDataJSON();
    shader.Source("./assets/shaders/vert.glsl", "./assets/shaders/frag.glsl");
    shader.Compile();
    std::vector<Vertex> vertices = {
        {0.5f,  0.5f, 0.0f},
        {0.5f, -0.5f, 0.0f},
        {-0.5f, -0.5f, 0.0f},
        {-0.5f,  0.5f, 0.0f}
    };
    std::vector<u32> indices = {
        0, 1, 3,
        1, 2, 3
    };

    camera.Initialize(77.0f, (float)_window_width / _window_height);

    vao.Initialize(sizeof(Vertex));
    vao.Bind();

    vbo.Initialize();
    vbo.Bind();
    vbo.Source(vertices);

    ibo.Initialize();
    ibo.Bind();
    ibo.Source(indices);

    vao.glAddAttrib(0, 3, GL_FLOAT, 0);
    vao.Unbind();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

void Game::RenderFrame() {
    glClear(GL_COLOR_BUFFER_BIT);
    shader.Bind();
    shader.SetMat4("uMVP", camera.GetViewProjection());
    vao.Bind();
    glw::DrawIndexed(ibo.Length());
}

void Game::UpdateLogic() {
    camera.ProcessMouse();
    if (glfwGetKey(glw::window, GLFW_KEY_W))
        camera.Move(glw::Camera::Forward, _delta_time);
    if (glfwGetKey(glw::window, GLFW_KEY_S))
        camera.Move(glw::Camera::Backward, _delta_time);
    if (glfwGetKey(glw::window, GLFW_KEY_A))
        camera.Move(glw::Camera::Left, _delta_time);
    if (glfwGetKey(glw::window, GLFW_KEY_D))
        camera.Move(glw::Camera::Right, _delta_time);
}

void Game::Cleanup() {
    glw::Cleanup();
}

// TODO: error handling
void Game::LoadDataJSON() {
    using namespace simdjson;
    ondemand::parser parser;
    padded_string json = padded_string::load("./assets/data.json");
    ondemand::document data = parser.iterate(json);
    //for (auto e: data["items"]) {
    //    // ...
    //}
}

