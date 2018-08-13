#version 430 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 albedo_out;

in vec2 frag_uv;

uniform samplerCube environment_map;
uniform samplerCube environment_albedo_map;
uniform int side;

void main() {
  vec2 environment_texture_size = textureSize(environment_map, 0);
  float maxsize = max(environment_texture_size.x, environment_texture_size.x);
  float num_levels = 1 + floor(log2(maxsize));
  color = vec4(0.0, 0.0, 0.0, 1.0);
  vec3 dir = vec3(1.0, 0.0, 0.0);
  vec2 uv = frag_uv - vec2(0.5, 0.5);
  if (side == 0 ) {
    dir =  normalize(vec3(-0.5, uv.x, uv.y));
  }
  else if (side == 1) {
    dir =  normalize(vec3(0.5, uv.x, uv.y));
  }
  else if (side == 2) {
    dir =  normalize(vec3(uv.x, 0.5, uv.y));
  }
  else if (side == 3){
    dir =  normalize(vec3(uv.x, -0.5, uv.y));
  }
  else if (side == 4){
    dir =  normalize(vec3(uv.x, uv.y, 0.5));
  }
  else if (side == 5) {
    dir =  normalize(vec3(uv.x, uv.y, -0.5));
  }

  color = vec4(texture(environment_albedo_map, dir, 0).rgb, 1.0);
}
