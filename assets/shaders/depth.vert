#version 430 core

uniform mat4 model_view_projection;
layout(location = 0) in vec3 position;
layout(location = 3) in vec2 uv;

out vec2 fragment_uv;

void main() {
    gl_Position = model_view_projection * vec4(position, 1.0);
    fragment_uv = uv;
}
