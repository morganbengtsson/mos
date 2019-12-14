#version 430 core

layout(location = 0) out vec4 out_color;
in vec2 frag_uv;

uniform sampler2D color_sampler;
uniform sampler2D bloom_sampler;
uniform float strength;

float rand(vec2 co) {
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
  const vec3 bloom = texture(bloom_sampler, frag_uv).rgb;
  vec3 color = texture(color_sampler, frag_uv).rgb + bloom * strength;

  float r = rand(frag_uv);
  color.rgb *= (1.0 - r * 0.1);

  float exposure = 1.0;
  out_color = vec4(vec3(1.0) - exp(-color * exposure), 1.0);
}
