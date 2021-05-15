#version 430 core

const float PI = 3.14159265359;

struct Material {
  vec3 albedo;
  vec3 emission;
  float roughness;
  float metallic;
  float index_of_refraction;
  float transmission;
  float alpha;
  float ambient_occlusion;
  sampler2D albedo_sampler;
  sampler2D emission_sampler;
  sampler2D normal_sampler;
  sampler2D metallic_sampler;
  sampler2D roughness_sampler;
  sampler2D ambient_occlusion_sampler;
};

struct Directional_light{
  vec3 position;
  vec3 direction;
  vec3 color;
  float strength;
};

struct Spot_light {
  vec3 position;
  vec3 color;
  float strength;
  mat4 view;
  mat4 projection;
  float angle;
  float blend;
  vec3 direction;
};

struct Camera {
  vec3 position;
  ivec2 resolution;
  //float far;
  //float near;
};

struct Environment {
  vec3 position;
  vec3 extent;
  float strength;
  float falloff;
};

struct Fog {
  vec3 color_near;
  vec3 color_far;
  float attenuation_factor;
  float min;
  float max;
};

struct Fragment {
  vec3 position;
  vec3 normal;
  vec2 uv;
  mat3 tbn;
  vec4[4] proj_shadow; //TODO: Rename
  vec4[4] cascaded_proj_shadow; // TODO: Rename
  vec3 camera_to_surface;
};

uniform mat4 model;
uniform mat4 model_view;
uniform mat4 view; // TODO: Move to camera

uniform Camera camera;
uniform Fog fog;
uniform Material material;
uniform Spot_light[4] spot_lights;
uniform Directional_light directional_light;

uniform sampler2D[4] shadow_samplers;
uniform sampler2D[4] cascaded_shadow_samplers;
uniform vec4 cascade_splits;

uniform Environment[2] environments;
uniform samplerCube[2] environment_samplers;

uniform sampler2D brdf_lut_sampler;

in Fragment fragment;

layout(location = 0) out vec4 out_color;

// Defined in functions.frag
float rand(vec2 co);
mat3 rotate(const vec3 axis, const float a);
vec3 box_correct(const vec3 box_extent, const vec3 box_pos, const vec3 dir, const vec3 fragment_position);
float sample_variance_shadow_map(sampler2D shadow_map, vec2 uv, float compare);
float sample_shadow_map(sampler2D shadow_map, const vec2 uv, const float compare);
float distribution_GGX(vec3 N, vec3 H, float roughness);
float geometry_schlick_GGX(float NdotV, float roughness);
float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnel_schlick(float cosTheta, vec3 F0);
vec3 fresnel_schlick_roughness(float cosTheta, vec3 F0, float roughness);
float fog_attenuation(const float dist, const float factor);
bool inside_box(const vec3 point, const vec3 position, const vec3 extent);

vec3 sample_environment(samplerCube sampler, const vec3 direction, float mip_level, const float angle, const float step){
  int num_samples = 0;
  vec3 filtered = vec3(0.0);
  for(float x = -angle; x <= angle; x += step) {
    for(float y = -angle; y <= angle; y += step ) {
      const mat3 rotx = rotate(vec3(1.0, 0.0, 0.0), x);
      const mat3 roty = rotate(vec3(0.0, 1.0, 0.0), y);
      const mat3 rotz = rotate(vec3(0.0, 0.0, 1.0), y);
      filtered += textureLod(sampler, rotx * roty * rotz * direction, mip_level).rgb;
      num_samples += 1;
    }
  }
  filtered /= num_samples;
  return filtered;
}


float environment_attenuation(const vec3 point, const vec3 position, const vec3 extent, const float falloff) {
  const float distance_x = smoothstep(abs(position.x) + extent.x,abs(position.x) + extent.x * (1.0 - falloff), abs(point.x));
  const float distance_y = smoothstep(abs(position.y) + extent.y,abs(position.y) + extent.y * (1.0 - falloff), abs(point.y));
  const float distance_z = smoothstep(abs(position.z) + extent.z,abs(position.z) + extent.z * (1.0 - falloff), abs(point.z));
  return min(distance_x, min(distance_y, distance_z));
}

vec3 shade_spotlights(const in Spot_light[4] lights,
                  const in vec4[4] shadow_projs,
                  const in sampler2D[4] shadow_samplers,
                  const in vec3 position,
                  const in vec3 normal,
                  const in vec3 view_vector,
                  const in float normal_dot_view_vector,
                  const in vec3 albedo,
                  const in float metallic,
                  const in float roughness)
{
  const vec3 F0 = mix(vec3(0.02), albedo, metallic); // Move outside
  vec3 direct = vec3(0.0, 0.0, 0.0);
  for(int i = 0; i < spot_lights.length(); i++) {
    Spot_light light = spot_lights[i];
    if (light.strength > 0.0) {
        const vec3 light_vector = normalize(light.position - position);
        const vec3 half_vector = normalize(view_vector + light_vector);

        const float normal_dot_light_vector = max(dot(normal, light_vector), 0.0);

        const vec3 shadow_map_uv = shadow_projs[i].xyz / shadow_projs[i].w;
        const vec2 texel_size = 1.0 / textureSize(shadow_samplers[i], 0);
        const float shadow = sample_variance_shadow_map(shadow_samplers[i], shadow_map_uv.xy + texel_size, shadow_map_uv.z);

        const float light_fragment_distance = distance(light.position, position);
        const float attenuation = 1.0 / (light_fragment_distance * light_fragment_distance);

        const float spot_light_intensity_factor = 0.085; // TODO: move ?
        const vec3 radiance = light.strength * spot_light_intensity_factor * light.color * attenuation;

        const float NDF = distribution_GGX(normal, half_vector, roughness);
        const float G = geometry_smith(normal, view_vector, light_vector, roughness);
        const vec3 F = fresnel_schlick(clamp(dot(half_vector, view_vector), 0.0, 1.0), F0);

        const vec3 nominator = NDF * G * F;
        const float denominator = 4 * normal_dot_view_vector * normal_dot_light_vector + 0.001;
        const vec3 specular = clamp(nominator / denominator, vec3(0), vec3(0.2));

        const vec3 kS = F;
        const vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

        const float cos_dir = dot(light_vector, -light.direction);
        const float spot_effect = smoothstep(cos(light.angle / 2.0), cos((light.angle / 2.0) * (1.0 - light.blend)), cos_dir);
        const vec3 spot_color = vec3(1.0, 1.0, clamp(spot_effect, 0.7, 1.0));

        direct += (kD * albedo / PI + specular) * radiance * normal_dot_light_vector * spot_effect * shadow * (1.0 - material.transmission) * spot_color;
    }
  }
  return direct;
}

vec3 shade_directional_light(const in Directional_light light,
                             const in vec4[4] shadow_projs,
                             const in sampler2D[4] shadow_samplers,
                             const in vec3 normal,
                             const in vec3 view_vector,
                             const in float normal_dot_view_vector,
                             const in vec3 albedo,
                             const in float metallic,
                             const in float roughness)
{
  if (directional_light.strength > 0.0) {
    vec3 F0 = vec3(0.02);
    F0 = mix(F0, albedo, metallic);

    const vec3 L = normalize(-light.direction);
    const vec3 H = normalize(view_vector + L);

    const float NdotL = max(dot(normal, L), 0.0);

    int cascade_index = 0;

    for (int i = 0; i < 4; i++) {
      const vec3 shadow_map_uv = shadow_projs[i].xyz / shadow_projs[i].w;
      if (shadow_map_uv.x < 1.0 && shadow_map_uv.x > 0.0 && shadow_map_uv.y < 1.0 && shadow_map_uv.y > 0.0){
        cascade_index = i;
        break;
      }
    }

    const vec3 shadow_map_uv = shadow_projs[cascade_index].xyz / shadow_projs[cascade_index].w;
    const float shadow = sample_variance_shadow_map(shadow_samplers[cascade_index], shadow_map_uv.xy, shadow_map_uv.z);

    const vec3 radiance = directional_light.strength * directional_light.color;

    const float NDF = distribution_GGX(normal, H, roughness);
    const float G = geometry_smith(normal, view_vector, L, roughness);
    const vec3 F = fresnel_schlick(clamp(dot(H, view_vector), 0.0, 1.0), F0);

    const vec3 nominator = NDF * G * F;
    const float denominator = 4 * normal_dot_view_vector * NdotL + 0.001;
    const vec3 specular = clamp(nominator / denominator, vec3(0), vec3(0.2));

    const vec3 kS = F;
    const vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

    return (kD * albedo / PI + specular) * radiance * NdotL  * (1.0 - material.transmission) * shadow;
  } else {
    return vec3(0, 0, 0);
  }
}

vec3 sample_normal(const in vec3 normal, const in sampler2D normal_sampler, const in mat3 tbn, const in vec2 uv) {
  vec3 out_normal = normal;
  bool has_normal_map = textureSize(normal_sampler, 0).x != 1;
  if (has_normal_map){
    vec3 normal_from_map = texture(normal_sampler, uv).rgb * 2.0 - vec3(1.0);
    normal_from_map = normalize(tbn * normal_from_map);
    out_normal = normal_from_map;
  }

  if (!gl_FrontFacing) {
    out_normal = -out_normal;
  }
  return out_normal;
}

vec4 sample_albedo_alpha(const in vec3 albedo, const in float alpha, const in sampler2D sampler, const in vec2 uv) {
  bool has_albedo_map = textureSize(sampler, 0).x != 1;
  vec4 albedo_from_map = texture(sampler, fragment.uv);

  if (alpha == 0.0) {
    discard;
  }
  else if (albedo_from_map.a + float(!has_albedo_map) < 0.9) {
      discard;
  }

  return has_albedo_map ? albedo_from_map.rgba : vec4(albedo.rgb, alpha);
}

vec3 sample_replace(const in vec3 color, const in sampler2D sampler, const in vec2 uv) {
  bool has_map = textureSize(sampler, 0).x != 1;
  vec4 from_map = texture(sampler, uv);
  return has_map ? from_map.rgb : color;
}

float sample_replace(const in float value, const in sampler2D sampler, const in vec2 uv) {
  bool has_map = textureSize(sampler, 0).x != 1;
  vec4 from_map = texture(sampler, uv);
  return has_map ? from_map.r : value;
}

void main() {
  vec3 N = sample_normal(fragment.normal, material.normal_sampler, fragment.tbn, fragment.uv);

  vec4 albedo_alpha = sample_albedo_alpha(material.albedo, material.alpha, material.albedo_sampler, fragment.uv);
  vec3 emission = sample_replace(material.emission, material.emission_sampler, fragment.uv);
  float metallic = sample_replace(material.metallic, material.metallic_sampler, fragment.uv);
  float roughness = sample_replace(material.roughness, material.roughness_sampler, fragment.uv);
  float ambient_occlusion = sample_replace(material.ambient_occlusion, material.ambient_occlusion_sampler, fragment.uv);

  const vec3 V = normalize(camera.position - fragment.position);
  const vec3 R = -reflect(fragment.camera_to_surface, N);

  const float NdotV = max(dot(N, V), 0.0);

  vec3 F0 = vec3(0.02);
  F0 = mix(F0, albedo_alpha.rgb, metallic);

  vec3 direct = shade_spotlights(spot_lights, fragment.proj_shadow, shadow_samplers, fragment.position, N, V, NdotV, albedo_alpha.rgb, metallic, roughness);
  direct += shade_directional_light(directional_light, fragment.cascaded_proj_shadow, cascaded_shadow_samplers, N, V, NdotV, albedo_alpha.rgb, metallic, roughness);

  vec3 ambient = vec3(0.0, 0.0, 0.0);
  float attenuation = 0.0f;

  for (int i = 0; i < environments.length(); i++) {
    if (environments[i].strength > 0.0 && inside_box(fragment.position, environments[i].position, environments[i].extent)) {
      const vec3 corrected_N = box_correct(environments[i].extent, environments[i].position, N, fragment.position);
      const vec3 corrected_R = box_correct(environments[i].extent, environments[i].position, R, fragment.position);

      const vec2 environment_texture_size = textureSize(environment_samplers[i], 0);
      const float maxsize = max(environment_texture_size.x, environment_texture_size.x);
      const float num_levels = textureQueryLevels(environment_samplers[i]);
      const float mip_level = clamp(pow(roughness, 0.25) * num_levels, 0.0, 10.0);

      const vec3 F_env = fresnel_schlick_roughness(NdotV, F0, roughness);
      const vec3 kS_env = F_env;
      const vec3 kD_env = (1.0 - kS_env) * (1.0 - metallic);

      vec3 filtered = vec3(0,0,0);
      if (roughness > 0.4) {
        filtered = sample_environment(environment_samplers[i], corrected_R, mip_level, 0.5 * roughness, 1.0 * roughness);
      } else {
        filtered = textureLod(environment_samplers[i], corrected_R, mip_level).rgb;
      }

      const vec2 brdf  = texture(brdf_lut_sampler, vec2(NdotV, roughness)).rg;

      vec3 refraction = vec3(0,0,0);
      if (material.transmission > 0.0) {
        const float refractive_index = material.index_of_refraction;
        const vec3 RF = refract(V, N, 1.0 / refractive_index);
        const vec3 corrected_RF = box_correct(environments[i].extent, environments[i].position, RF, fragment.position);
        const vec3 refraction = textureLod(environment_samplers[i], corrected_RF, mip_level).rgb * material.transmission;
      }

      float horiz = dot(corrected_R,N);
      const float horiz_fade_power = 1.0 - roughness;
      horiz = clamp( 1.0 + horiz_fade_power * horiz, 0.0, 1.0 );
      horiz *= horiz;

      vec3 specular_environment = mix(refraction, filtered, F_env * brdf.x + brdf.y) * horiz * environments[i].strength;

      vec3 irradiance = sample_environment(environment_samplers[i], corrected_N, num_levels - 1.5, 0.5, 1.0);

      const vec3 diffuse_environment = irradiance * albedo_alpha.rgb * environments[i].strength;

      attenuation = (attenuation == 0.0) ? environment_attenuation(fragment.position, environments[i].position, environments[i].extent, environments[i].falloff) : 1.0 - attenuation;

      ambient += attenuation * clamp((kD_env * diffuse_environment * (1.0 - material.transmission) + specular_environment) * ambient_occlusion, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
      if (attenuation == 1.0) {
        break;
      }
    }
  }
  out_color.rgb = (direct + ambient) + emission;
  out_color.a = clamp(albedo_alpha.a, 0.0, 1.0);

  //Fog
  float fog_distance = distance(fragment.position, camera.position);
  float fog_att = fog_attenuation(fog_distance, fog.attenuation_factor);
  vec3 fog_color = mix(fog.color_far, fog.color_near, fog_att);
  out_color.rgb = mix(fog_color, out_color.rgb, clamp(fog_att, fog.min, fog.max));
}
