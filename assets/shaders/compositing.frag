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

  vec2 uv = frag_uv;
  uv *=  1.0 - uv.yx;
  float vig = uv.x*uv.y * 15.0;
  vig = pow(vig, 0.25);

  /*
  float offset = 0.002 * (1.0 - vig);
  color.r = texture(color_sampler, frag_uv + vec2(offset, offset)).r;
  color.g = texture(color_sampler, frag_uv + vec2(-offset, -offset)).g;
  color.b = texture(color_sampler, frag_uv + vec2(offset, -offset)).b;
 */

  float r = rand(frag_uv);
  color.rgb *= (1.0 - r * 0.15);

  float exposure = 1.0;
  out_color = vec4(vec3(1.0) - exp(-color * exposure), 1.0);
  out_color.rgb *= vig;
}
