#version 330
struct Fragment {
    vec3 position;
    vec3 normal;
    vec2 uv;
    vec2 lightmap_uv;
    vec3 shadow;
};

uniform mat4 model_view_projection;
uniform mat4 model_view;
layout(location = 0) in vec3 position;
layout(location = 2) in vec2 uv;
out Fragment fragment;

void main() {
    fragment.position = (model_view * vec4(position, 0.0)).xyz;
    fragment.uv = uv;
    gl_Position = model_view_projection * vec4(position, 1.0);
};
