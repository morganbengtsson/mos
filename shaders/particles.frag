#version 330 core
in vec3 fragment_position;
in vec4 fragment_color;

void main() {
    gl_FragColor = vec4(fragment_color);
};
