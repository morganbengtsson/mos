#version 430 core

layout(location = 0) out vec4 out_color;
in vec2 frag_uv;
uniform sampler2D color_sampler;
uniform sampler2D blurred_color_sampler;
uniform sampler2DMS depth_sampler;

const float focal_length = 5;
const float focal_length_sharpness = 100.0;
const float blur_rate = 6;

void main() {

  float far= 500.0;
  float near = 1.0;

  vec2 texture_size = textureSize(depth_sampler);
  ivec2 pixel_uv = ivec2(floor(texture_size * frag_uv));
  float depth = texelFetch(depth_sampler, pixel_uv, 0).r;

  float z = depth * 2.0 - 1.0;
  z = (2.0 * near * far) / (far + near - z * (far - near));

  float blur = clamp(pow(blur_rate,abs(z - focal_length))/ focal_length_sharpness, 0, 1);


  vec4 focus_color = texture(color_sampler, frag_uv);
  vec4 unfocus_color = texture(blurred_color_sampler, frag_uv);

  out_color = mix(focus_color, unfocus_color, blur);
}
