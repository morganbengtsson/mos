#version 430 core

layout(location = 0) out vec4 out_color;
in vec2 frag_uv;

uniform sampler2D color_sampler;
uniform sampler2D bloom_sampler;
uniform float strength;

float rand(vec2 co) {
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 uncharted2_tonemap(vec3 x) {
  float A = 0.15;
  float B = 0.50;
  float C = 0.10;
  float D = 0.20;
  float E = 0.02;
  float F = 0.30;
  return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

vec3 uncharted2(vec3 color) {
  const float W = 11.2;
  float exposure_bias = 2.0;
  vec3 curr = uncharted2_tonemap(exposure_bias * color);
  vec3 white_scale = 1.0 / uncharted2_tonemap(vec3(W));
  return curr * white_scale;
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

  float exposure = 0.0;
  out_color = vec4(uncharted2(color * pow(2.0, exposure)), 1.0);
  out_color.rgb *= vig;
}
