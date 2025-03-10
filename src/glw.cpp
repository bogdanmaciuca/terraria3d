#include "glw.hpp"
#include <cassert>
#include <fstream>
#include <iostream> // TODO: using logging library and only include it on debug builds
#include <glad/glad.h>
#include "core.hpp"

GLWrapper::GLWrapper(i32 window_width, i32 window_height) {
    Initialize(window_width, window_height);
}
void GLWrapper::Initialize(i32 window_width, i32 window_height) {
    assert(glfwInit() != 0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    _window = glfwCreateWindow(window_width, window_height, "Terraria 3D", nullptr, nullptr); // TODO: generalize
    assert(_window != nullptr);
    glfwMakeContextCurrent(_window);
    assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    //glEnable(GL_POINT_SPRITE); // Debug
    //glPointSize(5.0);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
}

GLWrapper::VertexArrayObject& GLWrapper::AllocVertexArrayObject() {
}

Shader& AllocShader();

Texture& AllocTexture();

Framebuffer& AllocFramebuffer();

Renderbuffer& AllocRenderbuffer();

void GLWrapper::VertexArrayObject::Initialize(i32 stride) {
    glGenVertexArrays(1, &_ID); _stride = stride; 
}
void GLWrapper::VertexArrayObject::Bind() {
    glBindVertexArray(_ID); 
}
void GLWrapper::VertexArrayObject::AddAttrib(GLenum type, i32 num) {
    glVertexAttribPointer(_idx, num, type, GL_FALSE, _stride, reinterpret_cast<void*>(_offset));
    glEnableVertexAttribArray(_idx);
    u8 size = 0;
    switch(type) {
        case GL_BYTE: case GL_UNSIGNED_BYTE: size = 1; break;
        case GL_SHORT: case GL_UNSIGNED_SHORT: size = 2; break;
        case GL_INT: case GL_UNSIGNED_INT: case GL_FLOAT: size = 4; break;
    }
    assert(size != 0);
    _idx++;
    _offset += num * size;
}
void GLWrapper::VertexArrayObject::Unbind() { glBindVertexArray(0); }

// Shader
void CheckErrors(u32 shader, const std::string& filename) {
    i32 success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE) {
        i32 error_len;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &error_len);
        char* error_msg = static_cast<char*>(malloc(error_len * sizeof(char)));
        glGetShaderInfoLog(shader, error_len, nullptr, error_msg);
        std::cout << filename << ": " << error_msg << "\n";
    }
}
u32 CreateShader(GLenum type, const std::string& filename) {
    u32 shader = glCreateShader(type);
    std::string source;
    core::File(filename.c_str()).ReadAll(&source);
    const char* source_c_str = source.c_str();
    glShaderSource(shader, 1, &source_c_str, nullptr);
    glCompileShader(shader);
    CheckErrors(shader, filename);
    return shader;
}
void GLWrapper::Shader::Source(const std::string& vertex_filename, const std::string& fragment_filename) {
    _vertex_filename = vertex_filename;
    _fragment_filename = fragment_filename;
}
void GLWrapper::Shader::Compile() {
    u32 vertex_shader = CreateShader(GL_VERTEX_SHADER, _vertex_filename);
    u32 fragment_shader = CreateShader(GL_FRAGMENT_SHADER, _fragment_filename);
    _ID = glCreateProgram();
    glAttachShader(_ID, vertex_shader);
    glAttachShader(_ID, fragment_shader);
    glLinkProgram(_ID);
    glDetachShader(_ID, vertex_shader);
    glDeleteShader(vertex_shader);
    glDetachShader(_ID, fragment_shader);
    glDeleteShader(fragment_shader);
}
void GLWrapper::Shader::Reload() {
    glDeleteProgram(_ID);
    Compile();
}
void GLWrapper::Shader::SetVec3(const std::string& name, const glm::vec3& value) {
    glUniform3fv(glGetUniformLocation(_ID, name.c_str()), 1, &value[0]); 
}
void GLWrapper::Shader::SetMat4(const std::string& name, const glm::mat4& value) {
    glUniformMatrix4fv(glGetUniformLocation(_ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
}
void GLWrapper::Shader::SetInt(const std::string& name, int value) {
    glUniform1i(glGetUniformLocation(_ID, name.c_str()), value);
}

// Texture
void GLWrapper::Texture::Initialize(i32 width, i32 height) {
    _width = width; _height = height;
    glGenTextures(1, &_ID);
    glBindTexture(GL_TEXTURE_2D, _ID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}
void GLWrapper::Texture::Bind() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _ID);
}

// Framebuffer
void GLWrapper::Framebuffer::Initialize() {
    glGenFramebuffers(1, &_ID);
}
void GLWrapper::Framebuffer::Bind(GLenum target) {
    glBindFramebuffer(target, _ID);
}
void GLWrapper::Framebuffer::AddTexture(const Texture& texture) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.GetID(), 0);
}
void GLWrapper::Framebuffer::BindDefault(GLenum target) {
    glBindFramebuffer(target, 0);
}

// Renderbuffer
void GLWrapper::Renderbuffer::Initialize(i32 width, i32 height) {
    glGenRenderbuffers(1, &_ID);
    glBindRenderbuffer(GL_RENDERBUFFER, _ID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
}
void GLWrapper::Renderbuffer::Bind(GLenum target) {
    glBindFramebuffer(target, _ID);
}
void GLWrapper::Renderbuffer::AttachToFramebuffer() {
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        _ID
    );
}

// Camera
void GLWrapper::Camera::Initialize(GLFWwindow* window, float FOV, float w_h_ratio) {
    _window = window;
    _projection = glm::perspective(glm::radians(FOV), w_h_ratio, _z_near, _z_far);
}
glm::mat4 GLWrapper::Camera::GetViewMatrix() {
    return glm::lookAt(pos, pos + _front, _up);
}
glm::mat4 GLWrapper::Camera::GetViewProjection() {
    return _projection * GetViewMatrix();
}
void GLWrapper::Camera::ProcessMouse() {
    double mouse_x, mouse_y;
    glfwGetCursorPos(_window, &mouse_x, &mouse_y);
    float delta_x = (float)(mouse_x - _last_mouse_x);
    float delta_y = (float)(mouse_y - _last_mouse_y);
    _yaw += delta_x * _sensitivity;
    _pitch -= delta_y * _sensitivity;
    _last_mouse_x = mouse_x; _last_mouse_y = mouse_y;

    _pitch = glm::clamp(_pitch, -89.0f, 89.0f);

    _front.x = glm::cos(glm::radians(_yaw)) * glm::cos(glm::radians(_pitch));
    _front.y = glm::sin(glm::radians(_pitch));
    _front.z = glm::sin(glm::radians(_yaw)) * glm::cos(glm::radians(_pitch));
    _front = glm::normalize(_front);
}
void GLWrapper::Camera::Move(MoveDir dir, float delta_time) {
    switch(dir) {
        case MoveDir::Forward: pos += speed * _front * delta_time; break;
        case MoveDir::Backward: pos -= speed * _front * delta_time; break;
        case MoveDir::Left: pos -= speed * glm::normalize(glm::cross(_front, _up)) * delta_time; break;
        case MoveDir::Right: pos += speed * glm::normalize(glm::cross(_front, _up)) * delta_time; break;
    }
}

void GLWrapper::Initialize(i32 window_width, i32 window_height) {
    assert(glfwInit() != 0);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    _window = glfwCreateWindow(window_width, window_height, "Terraria 3D", nullptr, nullptr); // TODO: generalize
    assert(_window != nullptr);
    glfwMakeContextCurrent(_window);
    assert(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress));
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
    //glEnable(GL_POINT_SPRITE); // Debug
    //glPointSize(5.0);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
}
GLWrapper::~GLWrapper() {
    glfwTerminate();
}
void DrawIndexed(u32 indices_num, GLenum mode, GLenum type) {
    glDrawElements(mode, (i32)indices_num, type, 0);
}
void Draw(u32 vertices_num, GLenum mode) {
    glDrawArrays(mode, 0, (i32)vertices_num);
}

