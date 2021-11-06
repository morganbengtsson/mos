layout(location = 0) out vec4 out_color;
in vec2 frag_uv;

uniform sampler2D color_sampler;
uniform sampler2D bloom_sampler;
uniform float strength;

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
  float exposureBias = 2.0;
  vec3 curr = uncharted2_tonemap(exposureBias * color);
  vec3 whiteScale = 1.0 / uncharted2_tonemap(vec3(W));
  return curr * whiteScale;
}

void main() {
  // Bloom
  const vec3 bloom = texture(bloom_sampler, frag_uv).rgb;
  vec3 color = texture(color_sampler, frag_uv).rgb + bloom * strength;

  // Tonemap
  // TODO: Check the tonemapping, compare to Blender
  out_color = vec4(uncharted2(color * 1.6), 1.0);
}
