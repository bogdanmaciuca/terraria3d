#pragma once
#include <cassert>
#include <string>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "../vendor/glad/glad.h"

namespace glw {
    inline GLFWwindow* window;
    struct Shader {
    public:
        Shader() {};
        Shader(const std::string& vertex_filename, const std::string& fragment_filename);
        void Compile();
        void Reload();
        inline void Bind() {
            glUseProgram(_ID);
        }
    private:
        unsigned int _ID;
        std::string _vertex_filename, _fragment_filename;
    };

    void Initialize();
}

