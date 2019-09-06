#version 430 core

layout(location = 0) out vec4 out_color;
in vec2 frag_uv;
uniform sampler2D color_sampler;
uniform sampler2D blurred_color_sampler;
uniform sampler2D depth_sampler;

uniform float camera_near;
uniform float camera_far;
uniform float camera_focus_distance;
uniform float camera_fstop;

void main() {
  //TODO: Adjust parameters for more corectness.
  const float focal_length_sharpness = 1000.0 * camera_fstop;
  const float blur_rate = 6;

  //vec2 texture_size = textureSize(depth_sampler);
  //ivec2 pixel_uv = ivec2(floor(texture_size * frag_uv));
  //float depth = texelFetch(depth_sampler, pixel_uv, 0).r;
  float depth = texture(depth_sampler, frag_uv).x;

  float z = depth * 2.0 - 1.0;
  z = (2.0 * camera_near * camera_far) / (camera_far + camera_near - z * (camera_far - camera_near));

  float blur = clamp(pow(blur_rate,abs(z - camera_focus_distance)) / focal_length_sharpness, 0, 1);

  vec4 focus_color = texture(color_sampler, frag_uv);
  vec4 unfocus_color = texture(blurred_color_sampler, frag_uv);

  out_color = mix(focus_color, unfocus_color, blur);
}
