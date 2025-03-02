#version 330

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 uViewProj;
uniform mat4 uModel;

void main() {
    gl_Position = uViewProj * uModel * vec4(aPos, 1.0);
    Normal = normalize(mat3(transpose(inverse(uModel))) * normalize(aNormal));
    FragPos = vec3(uModel * vec4(aPos, 1.0));
}

