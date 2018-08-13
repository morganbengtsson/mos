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
  vec3 pos = vec3(0, 0, 0);
  if (side == 0 ){
    dir =  vec3(-1.0, 0.0, 0.0);
  }
  else if (side == 1) {
    dir = vec3(1.0, 0.0, 0.0);
  }
  else if (side == 2){
    dir = vec3(0.0, -1.0, 0.0);

  }else if (side == 3){
    dir = vec3(0.0, 1.0, 0.0);
  }
  else if (side == 4){
    dir = vec3(0.0, 0.0, -1.0);
  }
  else if (side == 5){
    dir = vec3(0.0, 0.0, 1.0);
  }

  color = vec4(texture(environment_albedo_map, vec3(-1.0, 0.0, 0.0), 0).rgb + texture(environment_map, vec3(-1.0, 0.0, 0.0), 0).rgb, 1.0);
}
