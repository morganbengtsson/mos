#version 330 core
struct Fragment {
    vec3 position;
    vec3 normal;
    vec2 uv;
    vec2 lightmap_uv;
    vec2 shadow_uv;
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
uniform mat4 model;
uniform mat4 model_view_projection;
uniform mat4 model_view;
uniform mat3 normal_matrix;
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in vec2 lightmap_uv;
out Fragment fragment;
void main()
{
    //Shadow test, todo on CPU
    mat4 bias = mat4(
    0.5, 0.0, 0.0, 0.0,
    0.0, 0.5, 0.0, 0.0,
    0.0, 0.0, 0.5, 0.0,
    0.5, 0.5, 0.5, 1.0
    );
    mat4 depth_bias_mvp = bias * light.projection * light.view * model;
    fragment.shadow_uv = (depth_bias_mvp * vec4(position, 1.0)).xy;

    fragment.uv = uv;
    fragment.lightmap_uv = lightmap_uv;
    fragment.position = (model_view * vec4(position, 1.0)).xyz;
    fragment.normal = normal_matrix * normal;
    gl_Position = model_view_projection * vec4(position, 1.0);
};
