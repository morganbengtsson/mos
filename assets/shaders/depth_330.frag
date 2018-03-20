#version 330
layout(location = 0) out vec4 color;

void main() {
  float depth = gl_FragCoord.z;
  float dx = dFdx(depth);
  float dy = dFdy(depth);
  color = vec4(gl_FragCoord.z, gl_FragCoord.z * gl_FragCoord.z, 0.0, 1.0);
}
