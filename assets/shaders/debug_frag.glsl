#version 330

out vec4 FragColor;

in vec3 oColor;
uniform vec3 uColor;
uniform bool uFade;

void main() {
    float opacity = 1.0 - gl_FragCoord.z;
    if (!uFade) opacity = 1.0;
    FragColor = vec4(oColor, opacity);
}


