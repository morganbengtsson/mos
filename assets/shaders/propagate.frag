#version 430 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 albedo_out;

in vec2 frag_uv;

uniform samplerCube environment_map;
uniform samplerCube environment_albedo_map;
uniform int side;

vec3 box_correct(const vec3 box_extent, const vec3 box_pos, const vec3 dir, const vec3 fragment_position);
mat3 rotate(const vec3 axis, const float a);

void main() {
  vec2 environment_texture_size = textureSize(environment_map, 0);
  float maxsize = max(environment_texture_size.x, environment_texture_size.x);
  float num_levels = textureQueryLevels(environment_map);
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
  else if (side == 3) {
    dir =  normalize(vec3(uv.x, -0.5, -uv.y));
    norm = vec3(dir.x, -dir.y, dir.z);
  }
  else if (side == 4) {
    dir =  normalize(vec3(uv.x, -uv.y, 0.5));
    norm = vec3(dir.x, dir.y, -dir.z);
  }
  else if (side == 5) {
    dir =  normalize(vec3(-uv.x, -uv.y, -0.5));
    norm = vec3(dir.x, dir.y, -dir.z);
  }

  vec3 irradiance = vec3(0.0, 0.0, 0.0);
  const float m = 0.5;
  float step = 2 * m;
  for(float x = -m; x <= m; x += step) {
      for(float y = -m; y <= m; y += step) {
        for(float z = -m; z <= m; z += step) {
            const mat3 rotx = rotate(vec3(1.0, 0.0, 0.0), x);
            const mat3 roty = rotate(vec3(0.0, 1.0, 0.0), y);
            const mat3 rotz = rotate(vec3(0.0, 0.0, 1.0), z);
            irradiance += textureLod(environment_map, rotx * roty * rotz * norm, num_levels - 1.5).rgb;
          }
      }
  }
  irradiance /= 8.0;

  vec3 albedo = texture(environment_albedo_map, dir).rgb;

  vec3 diffuse_environment = irradiance * albedo;

  vec3 pass0 = texture(environment_map, dir).rgb;

  //TODO: 0.25 factor is not scientific. Attenuation?
  color = vec4(diffuse_environment * 0.25 + pass0, 1.0);
}
