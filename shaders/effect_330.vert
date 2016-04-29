#version 330
uniform mat4 model_view_projection;
uniform mat4 model_view;
layout(location = 0) in vec3 position;
layout(location = 2) in vec2 uv;
out vec3 fragment_position;
out vec2 fragment_uv;

void main() {
    fragment_position = (model_view * vec4(position, 0.0)).xyz;
    fragment_uv = uv;
    gl_Position = model_view_projection * vec4(position, 1.0);
};
