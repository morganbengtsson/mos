#version 430 core

struct Fragment {
    vec3 position;
    vec3 normal;
    vec2 uv;
    vec4[4] proj_shadow;
    vec3 camera_to_surface;
};

struct Camera {
    vec3 position;
    float near;
    float far;
};

uniform Camera camera;
uniform mat4[4] depth_bias_model_view_projections;

uniform mat4 model;
uniform mat4 model_view_projection;
uniform mat3 normal_matrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 uv;
layout(location = 4) in float weight;

out Fragment fragment;

void main() {
    for (int i = 0; i < depth_bias_model_view_projections.length(); i++){
        fragment.proj_shadow[i] = depth_bias_model_view_projections[i] * vec4(position, 1.0);
    }
    fragment.uv = uv;
    fragment.position = (model * vec4(position, 1.0)).xyz;
    fragment.normal = normalize(normal_matrix * normal);
    fragment.camera_to_surface = normalize(camera.position - fragment.position);
    gl_Position = model_view_projection * vec4(position, 1.0);
}
