#version 430 core

layout(location = 0) out vec4 color;
in vec2 frag_uv;

uniform sampler2D color_sampler;
uniform sampler2D blurred_color_sampler;
uniform sampler2DMS depth_sampler;


void main() {
  const vec3 bloom = texture(bloom_sampler, frag_uv).rgb;
  color = vec4(texture(color_sampler, frag_uv).rgb + bloom * strength, 1.0);

  float r = rand(frag_uv * color.rg);
  color.rgb *= (1.0 - r * 0.1);
}
