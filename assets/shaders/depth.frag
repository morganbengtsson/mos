#version 330
layout(location = 0) out vec4 color;

void main() {
  color = vec4(gl_FragCoord.z, gl_FragCoord.z * gl_FragCoord.z, 0.0, 1.0);
}
