#version 430 core

const float PI = 3.14159265359;

struct Material {
    vec4 albedo;
    float emission;
    float roughness;
    float metallic;
    float opacity;
    float transmission;
    float ambient_occlusion;
    sampler2D albedo_map;
    sampler2D normal_map;
    sampler2D metallic_map;
    sampler2D roughness_map;
    sampler2D ambient_occlusion_map;
};

struct Light {
    vec3 position;
    vec3 color;
    float strength;
    mat4 view;
    mat4 projection;
    float angle;
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
};

struct Fog {
    vec3 color_near;
    vec3 color_far;
    float attenuation_factor;
};

struct Fragment {
    vec3 position;
    vec3 normal;
    vec2 uv;
    mat3 tbn;
    vec4[4] proj_shadow;
    vec3 camera_to_surface;
    float weight;
};

uniform Material material;
uniform Light[4] lights;
uniform sampler2D[4] shadow_maps;

uniform Environment[2] environments;
uniform samplerCube[2] environment_samplers;

uniform Camera camera;
uniform Fog fog;
uniform mat4 model;
uniform mat4 model_view;
uniform mat4 view;
uniform sampler2D brdf_lut;

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

bool inside_box(const vec3 point, const vec3 position, const vec3 extent) {
  vec3 mi = position - extent;
  vec3 ma = position + extent;
  return (mi.x <= point.x
      && point.x <= ma.x
      && mi.y <= point.y
      && point.y <= ma.y
      && mi.z <= point.z
      && point.z <= ma.z);
}

void main() {
    vec3 N = fragment.normal;

    vec3 N_from_map = texture(material.normal_map, fragment.uv).rgb * 2.0 - vec3(1.0);
    N_from_map = normalize(fragment.tbn * N_from_map);

    float amount = texture(material.normal_map, fragment.uv).a;

    if (amount > 0.0f){
        N = normalize(mix(N, N_from_map, amount));
    }

    vec4 albedo_from_map = texture(material.albedo_map, fragment.uv);
    vec3 albedo = mix(material.albedo.rgb, albedo_from_map.rgb, albedo_from_map.a);

    vec4 metallic_from_map = texture(material.metallic_map, fragment.uv);
    float metallic = mix(material.metallic, metallic_from_map.r, metallic_from_map.a);

    vec4 roughnesss_from_map = texture(material.roughness_map, fragment.uv);
    float roughness = mix(material.roughness, roughnesss_from_map.r, roughnesss_from_map.a);

    float ambient_occlusion_from_map = texture(material.ambient_occlusion_map, fragment.uv).r;
    float ambient_occlusion = material.ambient_occlusion * ambient_occlusion_from_map;

    if (material.opacity * (albedo_from_map.a + material.albedo.a) < 0.1 && material.emission == 0.0) {
        discard;
    }

    const vec3 V = normalize(camera.position - fragment.position);
    const vec3 R = -reflect(fragment.camera_to_surface, N);

    const float NdotV = max(dot(N, V), 0.0);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 direct = vec3(0.0, 0.0, 0.0);

    for(int i = 0; i < lights.length(); i++) {
        Light light = lights[i];
        if (light.strength > 0.0) {
            const vec3 L = normalize(light.position - fragment.position);
            const vec3 H = normalize(V + L);

            const float NdotL = max(dot(N, L), 0.0);

            const vec3 shadow_map_uv = fragment.proj_shadow[i].xyz / fragment.proj_shadow[i].w;
            const vec2 texel_size = 1.0 / textureSize(shadow_maps[i], 0);
            const float shadow = sample_variance_shadow_map(shadow_maps[i], shadow_map_uv.xy + texel_size, shadow_map_uv.z);

            const float light_fragment_distance = distance(light.position, fragment.position);
            const float attenuation = 1.0 / (light_fragment_distance * light_fragment_distance);
            const vec3 radiance = light.strength * 0.09 * light.color * attenuation;

            const float NDF = distribution_GGX(N, H, roughness);
            const float G = geometry_smith(N, V, L, roughness);
            const vec3 F = fresnel_schlick(clamp(dot(H, V), 0.0, 1.0), F0);

            const vec3 nominator = NDF * G * F;
            const float denominator = 4 * NdotV * NdotL + 0.001;
            const vec3 specular = nominator / denominator;

            const vec3 kS = F;
            const vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

            const float cos_dir = dot(L, -light.direction);
            const float spot_effect = smoothstep(cos(light.angle / 2.0), cos(light.angle / 2.0 - 0.1), cos_dir);

            direct += (kD * albedo / PI + specular) * radiance * NdotL * spot_effect * shadow * (1.0 - material.transmission);
        }
    }

    vec3 ambient = vec3(0.0, 0.0, 0.0);

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
          const float m0 = 0.5 * roughness;
          const float step = 1.0 * roughness;
          for(float x = -m0; x <= m0; x += step) {
              for(float y = -m0; y <= m0; y += step ) {
                for(float z = -m0; z <= m0; z += step ) {
                    const mat3 rotx = rotate(vec3(1.0, 0.0, 0.0), x);
                    const mat3 roty = rotate(vec3(0.0, 1.0, 0.0), y);
                    const mat3 rotz = rotate(vec3(0.0, 0.0, 1.0), z);
                    filtered += textureLod(environment_samplers[i], rotx * roty * rotz * corrected_R, mip_level).rgb;
                  }
              }
          }
          filtered /= 8.0;
        } else {
          filtered = textureLod(environment_samplers[i], corrected_R, mip_level).rgb;
        }


        const vec2 brdf  = texture(brdf_lut, vec2(NdotV, roughness)).rg;

        const float refractive_index = 1.5;
        const vec3 RF = refract(V, N, 1.0 / refractive_index);
        const vec3 corrected_RF = box_correct(environments[i].extent, environments[i].position, RF, fragment.position);
        const vec3 refraction = textureLod(environment_samplers[i], corrected_RF, mip_level).rgb * material.transmission;

        float horiz = dot(corrected_R,N);
        const float horiz_fade_power = 1.0 - roughness;
        horiz = clamp( 1.0 + horiz_fade_power * horiz, 0.0, 1.0 );
        horiz *= horiz;

        vec3 specular_environment = mix(refraction, filtered, F_env * brdf.x + brdf.y) * horiz * environments[i].strength;

        vec3 irradiance = vec3(0.0, 0.0, 0.0);

        const float m = 0.5;
        for(float x = -m; x <= m; x += 1.0) {
            for(float y = -m; y <= m; y += 1.0) {
              for(float z = -m; z <= m; z += 1.0) {
                  const mat3 rotx = rotate(vec3(1.0, 0.0, 0.0), x);
                  const mat3 roty = rotate(vec3(0.0, 1.0, 0.0), y);
                  const mat3 rotz = rotate(vec3(0.0, 0.0, 1.0), z);
                  irradiance += textureLod(environment_samplers[i], rotx * roty * rotz * corrected_N, num_levels - 1.5).rgb;
                }
            }
        }
        irradiance /= 8.0;

        const vec3 diffuse_environment = irradiance * albedo * environments[i].strength;
        ambient += clamp((kD_env * diffuse_environment * (1.0 - material.transmission) + specular_environment) * ambient_occlusion, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
        break;
      }
    }
    out_color.rgb = (1.0 - material.emission) * (direct + ambient) + material.emission * albedo;
    out_color.a = clamp(material.opacity * (albedo_from_map.a + material.albedo.a), 0.0, 1.0);

    //Fog
    float fog_distance = distance(fragment.position, camera.position);
    float fog_att = fog_attenuation(fog_distance, fog.attenuation_factor);
    vec3 fog_color = mix(fog.color_far, fog.color_near, fog_att);
    out_color.rgb = mix(fog_color, out_color.rgb, clamp(fog_att, 0.45, 1.0));
}
