#version 430 core

layout(location = 0) out vec4 color;
in vec2 frag_uv;

uniform sampler2D color_sampler;

uniform bool horizontal;
uniform float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{
  vec2 tex_offset = 1.0 / textureSize(color_sampler, 0);
  vec4 tex = texture(color_sampler, frag_uv);
  vec4 result = tex * weight[0];
  for(int i = 1; i < 5; ++i) {
    result += texture(color_sampler, frag_uv + vec2(horizontal ? tex_offset.x * i : 0, horizontal ? 0 : tex_offset.y * i)).rgba * weight[i];
    result += texture(color_sampler, frag_uv - vec2(horizontal ? tex_offset.x * i : 0, horizontal ? 0 : tex_offset.y * i)).rgba * weight[i];
  }
  color = vec4(clamp(result, vec4(0.0), vec4(1.0)));
}
