#version 330

in vec3 Normal;
out vec4 FragColor;

uniform vec3 uLightDir;

void main() {
    float ambient_coef = 0.2;
    vec3 ambient = ambient_coef * vec3(1);

    vec3 light_color = vec3(1);
    vec3 diffuse = light_color * max(dot(normalize(Normal), uLightDir), 0.0);

    vec3 object_color = vec3(1, 0, 0);
    FragColor = vec4((ambient + diffuse) * object_color, 1);
}

