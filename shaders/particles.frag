#ifdef GL_ES
precision mediump float;
precision mediump int;
#elses
#version 120
#endif
varying vec3 v_position;
varying vec4 v_color;

void main() {
    gl_FragColor = vec4(v_color);
};
