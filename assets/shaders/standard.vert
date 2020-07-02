#version 430 core

struct Fragment {
    vec3 position;
    vec3 normal;
    vec2 uv;
    mat3 tbn;
    vec4[4] proj_shadow;
    vec3 camera_to_surface;
};

struct Camera {
    vec3 position;
    ivec2 resolution;
};

struct Light {
    vec3 position;
    vec3 color;
    float strength;
    mat4 view;
    mat4 projection;
    float angle;
    vec3 direction;
};

uniform Light[4] lights;
uniform Camera camera;
uniform mat4[4] depth_bias_model_view_projections;
uniform mat4 model;
uniform mat4 model_view_projection;
uniform mat3 normal_matrix;
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 uv;

out Fragment fragment;

void main() {
    vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
    vec3 N = normalize(normal_matrix * normal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
    fragment.tbn = mat3(T,B,N);

    for (int i = 0; i < depth_bias_model_view_projections.length(); i++){
        fragment.proj_shadow[i] = depth_bias_model_view_projections[i] * vec4(position, 1.0);
    }

    fragment.uv = uv;
    fragment.position = (model * vec4(position, 1.0)).xyz;
    fragment.normal = normalize(normal_matrix * normal);
    fragment.camera_to_surface = normalize(camera.position - fragment.position);
    gl_Position = model_view_projection * vec4(position, 1.0);
}
