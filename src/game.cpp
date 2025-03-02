#include "game.hpp"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
#include "int.hpp"
#include "glw.hpp"
#include "debug.hpp"

Game::Game(i16 window_width, i16 window_height, const std::string& world_path) {
    _window_width = window_width; _window_height = window_height;
    _world_path = std::string(world_path);
    glw::Initialize(window_width, window_height);
    Initialize();
}
Game::~Game() {
    glw::Cleanup();
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
    shader.Bind();
    shader.SetVec3("uLightDir", glm::vec3(1, 1, 0));
    debug_shader.Source(
        "./assets/shaders/debug_vert.glsl",
        "./assets/shaders/debug_frag.glsl"
    );
    debug_shader.Compile();

    camera.Initialize(77.0f, (float)_window_width / _window_height);

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

void Game::RenderFrame() {
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
        camera.Move(glw::Camera::Forward, _delta_time);
    if (glfwGetKey(glw::window, GLFW_KEY_S))
        camera.Move(glw::Camera::Backward, _delta_time);
    if (glfwGetKey(glw::window, GLFW_KEY_A))
        camera.Move(glw::Camera::Left, _delta_time);
    if (glfwGetKey(glw::window, GLFW_KEY_D))
        camera.Move(glw::Camera::Right, _delta_time);
    if (glfwGetKey(glw::window, GLFW_KEY_LEFT_SHIFT))
        camera.speed = 8.0f;
    else
        camera.speed = 16.0f;
    chunk_manager.UpdateChunks(camera.pos);
    //light_theta += 0.3f * _delta_time;
}

