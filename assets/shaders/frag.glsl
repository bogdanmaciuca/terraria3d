#version 330

out vec4 FragColor;

in vec3 FragPos;

//uniform vec3 uLightDir;
uniform vec3 uLightPos;

void main() {
    float ambient_coef = 0.2;
    vec3 ambient = ambient_coef * vec3(1);

    vec3 normal_cs = normalize(cross(dFdx(FragPos), dFdy(FragPos)));

    vec3 light_color = vec3(1);
    vec3 light_dir = uLightPos - FragPos;
    vec3 diffuse = light_color * max(dot(normal_cs, normalize(light_dir)), 0.0);

    vec3 object_color = vec3(22, 140, 73) / 255.0;
    FragColor = vec4((ambient + diffuse) * object_color, 1);
}

