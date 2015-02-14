#ifdef GL_ES
precision mediump float;
recision mediump int;
#else
#version 120
#endif
uniform mat4 model_view_projection;
uniform mat4 model_view;
attribute vec3 position;
attribute vec4 color;

varying vec3 v_position;
varying vec4 v_color;

void main(){
#ifdef GL_ES
#else
    gl_PointSize = 0.8;
#endif
    v_color = color;
    v_position = (model_view * vec4(position, 0.0)).xyz;
    gl_Position = model_view_projection * vec4(position, 1.0);
};
