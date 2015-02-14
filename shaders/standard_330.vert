#version 330 core
uniform mat4 model;
uniform mat4 model_view_projection;
uniform mat4 model_view;
uniform mat3 normal_matrix;
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec2 lightmap_uv;
out vec3 fragment_position;
out vec3 fragment_normal;
out vec2 fragment_uv;
out vec2 fragment_lightmap_uv;
void main()
{
    fragment_uv = uv;
    fragment_lightmap_uv = lightmap_uv;
    fragment_position = (model_view * vec4(position, 1.0)).xyz;
    fragment_normal = normal_matrix * normal;
    gl_Position = model_view_projection * vec4(position, 1.0);
};
