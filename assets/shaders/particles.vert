#version 430 core

uniform mat4 model_view_projection;
uniform mat4 model_view;
uniform vec2 resolution;
uniform mat4 projection;
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in float size;
layout(location = 3) in float opacity;
out vec3 fragment_position;
out vec4 fragment_color;
out float fragment_opacity;

void main() {
    vec4 eye_pos = model_view * vec4(position, 1.0);
    vec4 projVoxel = projection * vec4(size, size, eye_pos.z, eye_pos.w);
    vec2 projSize = resolution * projVoxel.xy / projVoxel.w;
    gl_PointSize = 0.25 * (projSize.x+projSize.y);

    fragment_color = color;
    fragment_opacity = opacity;
    fragment_position = position;

    gl_Position = projection * eye_pos;
}
