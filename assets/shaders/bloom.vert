#version 430 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uv;

out vec2 frag_uv;

void main() {
    frag_uv = uv;
    gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}
