#version 330
layout(location = 0) out vec4 color;

void main() {
  float depth = gl_FragCoord.z;
  float dx = dFdx(depth);
  float dy = dFdy(depth);

  //color = vec4(depth, pow(depth, 2.0) + 0.25*(dx*dx + dy*dy), 0.0, 1.0);
  color = vec4(gl_FragCoord.z, gl_FragCoord.z * gl_FragCoord.z, 0.0, 1.0);
  //color.rgba = vec4(1.0, 1.0, 1.0, 1.0);
}
