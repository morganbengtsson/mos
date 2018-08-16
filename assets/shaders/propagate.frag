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
  vec3 norm = vec3(1.0, 0.0, 0.0);
  vec2 uv = frag_uv - vec2(0.5, 0.5);
  if (side == 0 ) {
    dir =  normalize(vec3(0.5, -uv.y, -uv.x));
    norm = vec3(-dir.x, dir.y, dir.z);
  }
  else if (side == 1) {
    dir =  normalize(vec3(-0.5, -uv.y, uv.x));
    norm = vec3(-dir.x, dir.y, dir.z);
  }
  else if (side == 2) {
    dir =  normalize(vec3(uv.x, 0.5, uv.y));
    norm = vec3(dir.x, -dir.y, dir.z);
  }
  else if (side == 3){ // MAY BE WRONG
    dir =  normalize(vec3(uv.x, -0.5, -uv.y));
    norm = vec3(dir.x, -dir.y, dir.z);
  }
  else if (side == 4){ // MAY BE WRONG
    dir =  normalize(vec3(uv.x, uv.y, 0.5));
    norm = vec3(dir.x, dir.y, -dir.z);
  }
  else if (side == 5) {
    dir =  normalize(vec3(-uv.x, -uv.y, -0.5));
    norm = vec3(dir.x, dir.y, -dir.z);
  }

  vec3 irradiance = textureLod(environment_map, norm, num_levels - 1.5).rgb;

  vec3 albedo = texture(environment_albedo_map, dir).rgb;

  vec3 diffuse_environment = irradiance * albedo;

  vec3 pass0 = texture(environment_map, dir).rgb;

  color = vec4(diffuse_environment + pass0, 1.0);
}
