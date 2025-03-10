#pragma once
#include <cassert>
#include <string>
#include <vector>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "int.hpp"

// TODO: initialize as many resources at once as possible
class GLWrapper {
public:
    GLWrapper() {};
    GLWrapper(i32 window_width, i32 window_height);
    void Initialize(i32 window_width, i32 window_height);

    class VertexArrayObject;
    VertexArrayObject& AllocVertexArrayObject();

    template<GLenum TBuffer, typename TElem, GLenum TUsage>
    class Buffer;

    template<typename TElem, GLenum TUsage = GL_STATIC_DRAW>
    using VertexBuffer = Buffer<GL_ARRAY_BUFFER, TElem, TUsage>;

    template<typename TElem, GLenum TUsage = GL_STATIC_DRAW>
    VertexBuffer<TElem, TUsage>& AllocVertexBuffer();

    template<typename TElem, GLenum TUsage = GL_STATIC_DRAW>
    using IndexBuffer = Buffer<GL_ELEMENT_ARRAY_BUFFER, TElem, TUsage>;
    template<typename TElem, GLenum TUsage = GL_STATIC_DRAW>
    IndexBuffer<TElem, TUsage>& AllocIndexBuffer();

    class Shader;
    Shader& AllocShader();

    class Texture;
    Texture& AllocTexture();

    class Framebuffer;
    Framebuffer& AllocFramebuffer();
    
    class Renderbuffer;
    Renderbuffer& AllocRenderbuffer();

    class Camera;

    struct GLObject {
    public:
        u32 GetID() const { return _ID; }
    protected:
        u32 _ID;
    };

    class VertexArrayObject : public GLObject {
    public:
        void Initialize(i32 stride);
        void Bind();
        void AddAttrib(GLenum type, i32 num);
        static void Unbind();
    private:
        i32 _stride;  // TODO: maybe change to smaller data type
        i32 _idx = 0;
        i32 _offset = 0;
    };

    template<GLenum TBuffer, typename TElem, GLenum TUsage>
    class Buffer : public GLObject {
    public:
        void Initialize() { glGenBuffers(1, &_ID); }
        void Bind() { glBindBuffer(TBuffer, _ID); }
        void Source(const std::vector<TElem>& data) {
            _length = data.size();
            glBufferData(TBuffer, _length * sizeof(TElem), data.data(), TUsage);
        }
        void Source(const void* data, std::size_t size, std::size_t length = 0) {
            _length = length;
            glBufferData(TBuffer, size, data, TUsage);
        }
        u32 Length() { return _length; }
    private:
        std::size_t _length;
    };

    class Shader : public GLObject {
    public:
        void Source(const std::string& vertex_filename, const std::string& fragment_filename);
        void Compile();
        void Reload();
        void Bind() { glUseProgram(_ID); }
        void SetVec3(const std::string& name, const glm::vec3& value);
        void SetMat4(const std::string& name, const glm::mat4& value);
        void SetInt(const std::string& name, int value);
    private:
        std::string _vertex_filename, _fragment_filename;
    };

    class Texture : public GLObject {
    public:
        void Initialize(i32 width, i32 height);
        void Bind();
    private:
        u32 _width, _height;
    };

    class Framebuffer : public GLObject {
    public:
        void Initialize();
        void Bind(GLenum target = GL_FRAMEBUFFER);
        void AddTexture(const Texture& texture);
        static void BindDefault(GLenum target = GL_FRAMEBUFFER);
    private:
    };

    // TODO: Maybe store width and height here (if ever needed)
    class Renderbuffer : public GLObject {
    public:
        void Initialize(i32 width, i32 height);
        void Bind(GLenum target = GL_RENDERBUFFER);
        void AttachToFramebuffer();
    };

    class Camera {
    public:
        enum MoveDir { Forward, Backward, Left, Right };
        glm::vec3 pos = glm::vec3(0); // TODO: maybe make these
        float speed = 3.0f;           // private
        void Initialize(GLFWwindow* window, float FOV, float w_h_ratio);
        glm::mat4 GetViewMatrix();
        glm::mat4 GetViewProjection();
        void ProcessMouse();
        void Move(MoveDir dir, float delta_time);
    private:
        GLFWwindow* _window;
        double _last_mouse_x = 0 , _last_mouse_y = 0;
        float _sensitivity = 0.25f;
        float _yaw = 0, _pitch = 0;
        const glm::vec3 _up = glm::vec3(0, 1, 0);
        glm::vec3 _front = glm::vec3(0, 0, 1);
        glm::mat4 _projection;
        const float _z_near = 0.1f;
        const float _z_far = 800.0f;
    };

    void DrawIndexed(u32 indices_num, GLenum mode = GL_TRIANGLES, GLenum type = GL_UNSIGNED_INT);
    void Draw(u32 vertices_num, GLenum mode = GL_TRIANGLES);
private:
    GLFWwindow* _window;
    std::vector<VertexArrayObject> _VAOs;
};

#ifdef GLW_GLOBAL_TYPES
using VertexArrayObject = GLWrapper::VertexArrayObject;
using VertexBuffer = GLWrapper::VertexBuffer;
using IndexBuffer = GLWrapper::IndexBuffer;
using Shader = GLWrapper::Shader;
using Texture = GLWrapper::Texture;
using Framebuffer = GLWrapper::Framebuffer;
using Renderbuffer = GLWrapper::Renderbuffer;
#endif
