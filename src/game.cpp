#include "game.hpp"
#include <string>
#include <vector>
#include <iostream>
#include "FastNoise/FastNoise.h"
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
        UpdateLogic();
    }
}

void Game::Initialize() {
    TerrainFile file;
    if (file.Initialize("tests/maps/1.terrain", 3))
        std::cout << "existed!\n";
    else
        std::cout << "created just now!\n";

    exit(0);

    shader.Source("./assets/shaders/vert.glsl", "./assets/shaders/frag.glsl");
    shader.Compile();
    std::vector<Vertex> vertices = {
        {-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f} ,
        {0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f } ,
        {0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f } ,
        {0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f } ,
        {-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f} ,
        {-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f} ,
                                                   
        {-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f} ,
        {0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f } ,
        {0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f } ,
        {0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f } ,
        {-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f} ,
        {-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f} ,

        {-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f} ,
        {-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f} ,
        {-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f} ,
        {-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f} ,
        {-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f} ,
        {-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f} ,
                                                 
        {0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f } ,
        {0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f } ,
        {0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f } ,
        {0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f } ,
        {0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f } ,
        {0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f } ,
                                                  
        {-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f} ,
        {0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f } ,
        {0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f } ,
        {0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f } ,
        {-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f} ,
        {-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f} ,
                                                
        {-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f} ,
        {0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f } ,
        {0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f } ,
        {0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f } ,
        {-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f} ,
        {-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f }
    };

    camera.Initialize(77.0f, (float)_window_width / _window_height);

    vao.Initialize(sizeof(Vertex));
    vao.Bind();

    vbo.Initialize();
    vbo.Bind();
    vbo.Source(vertices);

    vao.AddAttrib(GL_FLOAT, 3);
    vao.AddAttrib(GL_FLOAT, 3);
    vao.Unbind();

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
}

void Game::RenderFrame() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.Bind();
    shader.SetMat4("uViewProj", camera.GetViewProjection());
    shader.SetMat4("uModel", glm::mat4(1));
    shader.SetVec3("uLightDir", glm::vec3(0.5f, glm::sin(light_theta)+1.0f, cos(light_theta)));
    vao.Bind();
    glw::Draw(vbo.Length());
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
    light_theta += 0.3f * _delta_time;
}

void Game::Cleanup() {
    glw::Cleanup();
}

