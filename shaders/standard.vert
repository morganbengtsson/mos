#ifdef GL_ES
precision mediump float;
precision mediump int;
#else\n
#version 120
#endif
uniform mat4 model;
uniform mat4 model_view_projection;
uniform mat4 model_view;
uniform mat3 normal_matrix;
attribute vec3 position;
attribute vec3 normal;
attribute vec2 uv;
attribute vec2 lightmap_uv;
varying vec3 fragment_position;
varying vec3 fragment_normal;
varying vec2 fragment_uv;
varying vec2 fragment_lightmap_uv;
void main()
{
    fragment_uv = uv;
    fragment_lightmap_uv = lightmap_uv;
    fragment_position = (model_view * vec4(position, 1.0)).xyz;
    fragment_normal = normal_matrix * normal;
    gl_Position = model_view_projection * vec4(position, 1.0);
};
