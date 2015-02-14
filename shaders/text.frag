#ifdef GL_ES
precision mediump float;
precision mediump int;
#else
#version 120
#endif
varying vec3 v_position;
varying vec2 v_uv;
uniform sampler2D texture;
uniform float opacity;

void main() {
    gl_FragColor = texture2D(texture, v_uv);
    gl_FragColor.w = gl_FragColor.w * opacity;
};
