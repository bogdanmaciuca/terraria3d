#include "glw.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include "../vendor/glad/glad.h"

namespace glw {
    void GlfwErrorCallback(int error, const char* desc) {
        std::cout << "Error: " << desc << "\n";
    }
    // TODO: Make this faster and place it in a separate file if another file needs it
    void ReadFile(const std::string& filename, std::string* out) {
        std::ifstream file(filename);
        assert(file.is_open());
        std::string line;
        while(std::getline(file, line))
            (*out) += line + '\n';
        file.close();
    }
    void CheckErrors(unsigned int shader, const std::string& filename) {
        int success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE) {
            int error_len;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &error_len);
            char* error_msg = static_cast<char*>(malloc(error_len * sizeof(char)));
            glGetShaderInfoLog(shader, error_len, nullptr, error_msg);
            std::cout << filename << ": " << error_msg << "\n";
        }
    }
    unsigned int CreateShader(GLenum type, const std::string& filename) {
        unsigned int shader = glCreateShader(type);
        std::string source;
        ReadFile(filename, &source);
        const char* source_c_str = source.c_str();
        glShaderSource(shader, 1, &source_c_str, nullptr);
        glCompileShader(shader);
        CheckErrors(shader, filename);
        return shader;
    }
    Shader::Shader(const std::string& vertex_filename, const std::string& fragment_filename)
        : _vertex_filename(vertex_filename), _fragment_filename(fragment_filename)
    {
        Compile();
    }
    void Shader::Compile() {
        unsigned int vertex_shader = CreateShader(GL_VERTEX_SHADER, _vertex_filename);
        unsigned int fragment_shader = CreateShader(GL_FRAGMENT_SHADER, _fragment_filename);
        _ID = glCreateProgram();
        glAttachShader(_ID, vertex_shader);
        glAttachShader(_ID, fragment_shader);
        glLinkProgram(_ID);
        glDetachShader(_ID, vertex_shader);
        glDeleteShader(vertex_shader);
        glDetachShader(_ID, fragment_shader);
        glDeleteShader(fragment_shader);
    }
    void Shader::Reload() {
        glDeleteProgram(_ID);
        Compile();
    }
    void Initialize() {
        assert(glfwInit() != 0);
        glfwSetErrorCallback(GlfwErrorCallback);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        window = glfwCreateWindow(1024, 576, "Terraria 3D", nullptr, nullptr); // TODO: generalize
        assert(window != nullptr);
        glfwMakeContextCurrent(window);
        assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));
    }
}

