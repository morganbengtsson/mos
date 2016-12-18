#version 330 core
struct Fragment {
    vec3 position;
    vec3 normal;
    vec3 normal_world;
    vec2 uv;
    vec2 lightmap_uv;
    vec3 shadow;
};

struct Light {
    vec3 position;
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    mat4 view;
    mat4 projection;
};

uniform Light light;
uniform mat4 depth_bias_model_view_projection;
uniform mat4 model;
uniform mat4 model_view_projection;
uniform mat4 model_view;
uniform mat3 normal_matrix;
uniform mat3 normal_matrix_world;
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 uv;
layout(location = 4) in vec2 lightmap_uv;
out Fragment fragment;
void main()
{    
    vec4 pos_ls = depth_bias_model_view_projection * vec4(position, 1.0);
    fragment.shadow = pos_ls.xyz / pos_ls.w;
    fragment.uv = uv;
    fragment.lightmap_uv = lightmap_uv;
    fragment.position = (model_view * vec4(position, 1.0)).xyz;
    fragment.normal = normal_matrix * normal;
    fragment.normal_world = normal_matrix_world * normal;
    gl_Position = model_view_projection * vec4(position, 1.0);
}
