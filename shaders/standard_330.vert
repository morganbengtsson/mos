#version 330 core
struct Fragment {
    vec3 position;
    vec3 normal;
    vec3 normal_world;
    vec2 uv;
    vec2 lightmap_uv;
    vec3 shadow;
    vec3 view;
    mat3 tbn;
};

struct Camera {
    vec3 position;
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
uniform Camera camera;
uniform mat4 depth_bias_model_view_projection;
uniform mat4 model; // NOT SET!
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
    vec4 P = model * vec4(position, 1.0);
    vec3 V = P.xyz;
    vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
    vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
    vec3 B = cross(T, N);

    fragment.tbn = mat3(T,B,N);

    vec4 pos_ls = depth_bias_model_view_projection * vec4(position, 1.0);
    fragment.shadow = pos_ls.xyz / pos_ls.w;
    fragment.uv = uv;
    fragment.lightmap_uv = lightmap_uv;
    fragment.position = (model * vec4(position, 1.0)).xyz;
    fragment.normal = normal_matrix_world * normal;
    fragment.normal_world = normal_matrix_world * normal;

    fragment.view = normalize(camera.position - fragment.position);

    gl_Position = model_view_projection * vec4(position, 1.0);
}
