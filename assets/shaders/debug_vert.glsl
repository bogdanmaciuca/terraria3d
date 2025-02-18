#version 330

layout (location = 0) in vec3 aPos;

out vec3 oColor;

uniform mat4 uViewProj;
uniform mat4 uModel;

void main() {
    gl_Position = uViewProj * uModel * vec4(aPos, 1.0);
    oColor = vec3(sin(aPos.z), cos(aPos.z), sin(aPos.z * 2));
}

