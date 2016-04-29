#version 330 core
uniform mat4 model_view_projection;
uniform mat4 model_view;
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in float size;
out vec3 fragment_position;
out vec4 fragment_color;

void main() {
    gl_PointSize = size;
    fragment_color = color;
    fragment_position = (model_view * vec4(position, 0.0)).xyz;
    gl_Position = model_view_projection * vec4(position, 1.0);
};
