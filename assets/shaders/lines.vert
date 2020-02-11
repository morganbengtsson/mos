#version 430 core

uniform mat4 model_view_projection;
uniform mat4 model_view;
uniform vec2 resolution;
uniform mat4 projection;
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in float size;
layout(location = 3) in float alpha;
out vec3 fragment_position;
out vec3 fragment_color;
out float fragment_alpha;

void main() {
    vec4 eye_pos = model_view * vec4(position, 1.0);

    fragment_color = color;
    fragment_alpha = alpha;
    fragment_position = position;

    gl_Position = projection * eye_pos;
}
