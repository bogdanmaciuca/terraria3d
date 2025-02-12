#pragma once
#include "int.hpp"
#include <cassert>
#include <string>
#include <vector>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "../vendor/glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace glw {
    inline GLFWwindow* window;

    struct VertexArrayObject {
    public:
        inline void Initialize(i32 stride) { glGenVertexArrays(1, &_ID); _stride = stride; }
        inline void Bind() { glBindVertexArray(_ID); }
        inline void glAddAttrib(i32 idx, i32 num, GLenum type, void* offset) {
            glVertexAttribPointer(idx, num, type, GL_FALSE, _stride, offset);
            glEnableVertexAttribArray(idx);
        }
        inline static void Unbind() { glBindVertexArray(0); }
    private:
        u32 _ID;
        i32 _stride;
    };

    template<GLenum TBuffer, typename TElem, GLenum TUsage>
    struct Buffer {
    public:
        inline void Initialize() { glGenBuffers(1, &_ID); }
        inline void Bind() { glBindBuffer(TBuffer, _ID); }
        inline void Source(const std::vector<TElem>& data) {
            _length = data.size();
            glBufferData(TBuffer, _length * sizeof(TElem), data.data(), TUsage);
        }
        inline u32 Length() { return _length; }
    private:
        u32 _ID;
        u32 _length;
    };

    template<typename TElem, GLenum TUsage = GL_STATIC_DRAW>
    using VertexBuffer = Buffer<GL_ARRAY_BUFFER, TElem, TUsage>;

    template<typename TElem, GLenum TUsage = GL_STATIC_DRAW>
    using IndexBuffer = Buffer<GL_ELEMENT_ARRAY_BUFFER, TElem, TUsage>;

    struct Shader {
    public:
        void Source(const std::string& vertex_filename, const std::string& fragment_filename);
        void Compile();
        void Reload();
        inline void Bind() { glUseProgram(_ID); }
        inline void SetVec3(const std::string& name, const glm::vec3& value) {
            glUniform3fv(glGetUniformLocation(_ID, name.c_str()), 1, &value[0]); 
        }
        inline void SetMat4(const std::string& name, const glm::mat4& value) {
            glUniformMatrix4fv(glGetUniformLocation(_ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
        }
    private:
        u32 _ID;
        std::string _vertex_filename, _fragment_filename;
    };

    struct Camera {
    public:
        enum MoveDir { Forward, Backward, Left, Right };
        glm::vec3 pos = glm::vec3(0);
        float speed = 1.0f;
        inline void Initialize(float FOV, float w_h_ratio) {
            _projection = glm::perspective(glm::radians(FOV), w_h_ratio, _z_near, _z_far);
        }
        inline glm::mat4 GetViewMatrix() { return glm::lookAt(pos, pos + _front, _up); }
        inline glm::mat4 GetViewProjection() { return _projection * GetViewMatrix(); }
        //inline glm::mat4 GetViewProjection() { return GetViewMatrix() * _projection; }
        void ProcessMouse();
        void Move(MoveDir dir, float delta_time);
    private:
        double _last_mouse_x = 0 , _last_mouse_y = 0;
        float _sensitivity = 0.25f;
        float _yaw, _pitch;
        const glm::vec3 _up = glm::vec3(0, 1, 0);
        glm::vec3 _front;
        glm::mat4 _projection;
        const float _z_near = 0.1f;
        const float _z_far = 100.0f;
    };

    void Initialize(i16 window_width, i16 window_height);
    void Cleanup();
    inline void DrawIndexed(i32 indices_num, GLenum mode = GL_TRIANGLES, GLenum type = GL_UNSIGNED_INT) {
        glDrawElements(mode, indices_num, type, 0);
    }
}

