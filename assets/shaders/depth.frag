#version 430 core

layout(location = 0) out vec4 color;
uniform sampler2D albedo_sampler;
uniform vec3 albedo;
uniform vec3 emission;
in vec2 fragment_uv;

void main() {
  bool has_albedo_map = textureSize(albedo_sampler, 0).x != 1;
  vec4 albedo_from_map = texture(albedo_sampler, fragment_uv);

  if (albedo_from_map.a + float(!has_albedo_map) < 0.9) {
      discard;
  }

  color = vec4(gl_FragCoord.z, gl_FragCoord.z * gl_FragCoord.z, 0.0, 1.0);
}
