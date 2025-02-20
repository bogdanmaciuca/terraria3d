#include "game.hpp"
#include <string>
#include <vector>
#include <iostream>
#include "int.hpp"
#include "glw.hpp"
#include "terrain.hpp"

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
        if (glfwGetKey(glw::window, GLFW_KEY_R)) {
            // Reload shaders...
        }
        UpdateLogic();
    }
}

void Game::Initialize() {
    shader.Source("./assets/shaders/vert.glsl", "./assets/shaders/frag.glsl");
    shader.Compile();
    shader.Bind();
    shader.SetVec3("uLightDir", glm::vec3(1, 1, 0));
    debug_shader.Source("./assets/shaders/debug_vert.glsl", "./assets/shaders/debug_frag.glsl");
    debug_shader.Compile();

    camera.Initialize(77.0f, (float)_window_width / _window_height);

    grid_vao.Initialize(sizeof(glm::vec3));
    grid_vao.Bind();

    chunk.GenerateTerrain();
    surface_nets.CreateMesh(chunk);

    points_vao.Initialize(sizeof(SurfaceNets::Vertex));
    points_vao.Bind();

    points_vbo.Initialize();
    points_vbo.Bind();
    points_vbo.Source(surface_nets.GetVertices());

    points_vao.AddAttrib(GL_FLOAT, 3);
    points_vao.Unbind();

    nets_vao.Initialize(sizeof(SurfaceNets::Vertex));
    nets_vao.Bind();

    nets_vbo.Initialize();
    nets_vbo.Bind();
    nets_vbo.Source(surface_nets.GetVertices());

    nets_ibo.Initialize();
    nets_ibo.Bind();
    nets_ibo.Source(surface_nets.GetIndices());

    nets_vao.AddAttrib(GL_FLOAT, 3);
    nets_vao.AddAttrib(GL_FLOAT, 3);
    nets_vao.Unbind();
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

void Game::RenderFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    debug_shader.Bind();
    debug_shader.SetMat4("uViewProj", camera.GetViewProjection());
    debug_shader.SetMat4("uModel", glm::mat4(1));

    points_vao.Bind();
    debug_shader.SetVec3("uColor", glm::vec3(1, 0, 0));
    debug_shader.SetInt("uFade", 0);
    //glw::Draw(points_vbo.Length(), GL_POINTS);
    nets_vao.Bind();

    shader.Bind();
    shader.SetMat4("uViewProj", camera.GetViewProjection());
    shader.SetMat4("uModel", glm::mat4(1));
    glw::DrawIndexed(6 * nets_ibo.Length(), GL_TRIANGLES, GL_UNSIGNED_SHORT);
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
        camera.speed = 4.0f;
    else
        camera.speed = 8.0f;
    light_theta += 0.3f * _delta_time;
}

void Game::Cleanup() {
    glw::Cleanup();
}

