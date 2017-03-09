#version 330 core

const int max_decals = 20;

struct Fragment {
    vec3 position;
    vec3 normal;
    vec2 uv;
    vec2 light_map_uv;
    vec4 proj_coords[max_decals];
    vec3 shadow;
    vec3 camera_to_surface;
    mat3 tbn;
};

struct Camera {
    vec3 position;
    vec2 resolution;
};

struct Light {
    vec3 position;
    vec3 diffuse;
    vec3 specular;
    mat4 view;
    mat4 projection;
    float linear_attenuation_factor;
    float quadratic_attenuation_factor;
    sampler2D shadow_map;
};

uniform Light light;
uniform Camera camera;
uniform mat4 depth_bias_model_view_projection;
uniform mat4 model; // NOT SET!
uniform mat4 model_view_projection;
uniform mat4 model_view;
uniform mat4 decal_model_view_projections[max_decals];
uniform mat3 normal_matrix;
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 uv;
layout(location = 4) in vec2 light_map_uv;
out Fragment fragment;

void main() {
    vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
    vec3 N = normalize(normal_matrix * normal);
    vec3 B = cross(T, N);
    fragment.tbn = mat3(T,B,N);

    vec4 pos_ls = depth_bias_model_view_projection * vec4(position, 1.0);
    fragment.shadow = pos_ls.xyz / pos_ls.w;

    for (int i = 0; i < max_decals; i++){
        vec4 pos_d = decal_model_view_projections[i] * vec4(position, 1.0);
        fragment.proj_coords[i] = pos_d;
    }

    fragment.uv = uv;
    fragment.light_map_uv = light_map_uv;
    fragment.position = (model * vec4(position, 1.0)).xyz;
    fragment.normal = normalize(normal_matrix * normal);
    fragment.camera_to_surface = normalize(camera.position - fragment.position);
    gl_Position = model_view_projection * vec4(position, 1.0);
}
