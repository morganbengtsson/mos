#version 330 core
uniform mat4 model_view_projection;
uniform mat4 model_view;
uniform vec2 resolution;
uniform mat4 projection;
layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;
layout(location = 2) in float size;
out vec3 fragment_position;
out vec4 fragment_color;

void main() {

    vec4 eye_pos = model_view * vec4(position, 1.0);
    vec4 projVoxel = projection * vec4(size, size, eye_pos.z, eye_pos.w);
    vec2 projSize = resolution * projVoxel.xy / projVoxel.w;
    gl_PointSize = 0.25 * (projSize.x+projSize.y);

    //gl_PointSize = size;
    fragment_color = color;
    fragment_position = (model_view * vec4(position, 0.0)).xyz;
    //gl_Position = model_view_projection * vec4(position, 1.0);

    gl_Position = projection * eye_pos;
}
