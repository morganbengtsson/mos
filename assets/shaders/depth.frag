#version 430 core

layout(location = 0) out vec4 color;
uniform sampler2D albedo_sampler;
uniform vec4 albedo;
uniform float emission;
in vec2 fragment_uv;

void main() {
  vec4 albedo_from_map = texture(albedo_sampler, fragment_uv);

  if (albedo_from_map.a + albedo.a < 0.5) {
      discard;
  }

  color = vec4(gl_FragCoord.z, gl_FragCoord.z * gl_FragCoord.z, 0.0, 1.0);
}
