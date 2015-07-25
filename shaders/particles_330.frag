#version 330 core
in vec3 fragment_position;
in vec4 fragment_color;
layout(location = 0) out vec4 color;

void main() {
    color = vec4(fragment_color);
};
