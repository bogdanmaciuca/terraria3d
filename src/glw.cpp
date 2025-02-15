#include "glw.hpp"
#include <cassert>
#include <fstream>
#include <iostream> // TODO: using logging library and only include it on debug builds
#include <glad/glad.h>

namespace glw {
    void VertexArrayObject::AddAttrib(GLenum type, i32 num) {
        glVertexAttribPointer(_idx, num, type, GL_FALSE, _stride, reinterpret_cast<void*>(_offset));
        glEnableVertexAttribArray(_idx);
        u8 size;
        switch(type) {
            case GL_BYTE: case GL_UNSIGNED_BYTE: size = 1; break;
            case GL_SHORT: case GL_UNSIGNED_SHORT: size = 2; break;
            case GL_INT: case GL_UNSIGNED_INT: case GL_FLOAT: size = 4; break;
        }
        _idx++;
        _offset += num * size;
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
    void Shader::Source(const std::string& vertex_filename, const std::string& fragment_filename) {
        _vertex_filename = vertex_filename;
        _fragment_filename = fragment_filename;
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

    void Camera::ProcessMouse() {
        double mouse_x, mouse_y;
        glfwGetCursorPos(window, &mouse_x, &mouse_y);
        float delta_x = mouse_x - _last_mouse_x;
        float delta_y = mouse_y - _last_mouse_y;
        _yaw += delta_x * _sensitivity;
        _pitch -= delta_y * _sensitivity;
        _last_mouse_x = mouse_x; _last_mouse_y = mouse_y;

        _pitch = glm::clamp(_pitch, -89.0f, 89.0f);

        _front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
        _front.y = sin(glm::radians(_pitch));
        _front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
        _front = glm::normalize(_front);
    }

    void Camera::Move(MoveDir dir, float delta_time) {
        switch(dir) {
            case MoveDir::Forward: pos += speed * _front * delta_time; break;
            case MoveDir::Backward: pos -= speed * _front * delta_time; break;
            case MoveDir::Left: pos -= speed * glm::normalize(glm::cross(_front, _up)) * delta_time; break;
            case MoveDir::Right: pos += speed * glm::normalize(glm::cross(_front, _up)) * delta_time; break;
        }
    }

    void Initialize(i16 window_width, i16 window_height) {
        assert(glfwInit() != 0);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        window = glfwCreateWindow(window_width, window_height, "Terraria 3D", nullptr, nullptr); // TODO: generalize
        assert(window != nullptr);
        glfwMakeContextCurrent(window);
        assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));
        glEnable(GL_DEPTH_TEST);
    }
    void Cleanup() {
        glfwTerminate();
    }
}

