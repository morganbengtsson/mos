#ifdef GL_ES
precision mediump float;
precision mediump int;
#else
#version 120
#endif
uniform mat4 model_view_projection;
uniform mat4 model_view;
attribute vec3 position;
attribute vec2 uv;

varying vec2 v_position;
varying vec2 v_uv;

void main(){
    v_position = (model_view * vec4(position, 0.0)).xy;
    v_uv = uv;
    gl_Position = model_view_projection * vec4(position, 1.0);
};
