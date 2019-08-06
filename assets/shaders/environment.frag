#version 430 core

const float PI = 3.14159265359;

struct Material {
    vec4 albedo;
    vec3 emission;
    float roughness;
    float metallic;
    float index_of_refraction;
    float transmission;
    float alpha;
    float ambient_occlusion;
    sampler2D albedo_map;
    sampler2D emission_map;
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
    float near;
    float far;
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
    vec4[4] proj_shadow;
    vec3 camera_to_surface;
    float weight;
};

uniform Material material;
uniform Light[4] lights;
uniform sampler2D[4] shadow_maps;

uniform Camera camera;
uniform Fog fog;
uniform mat4 model;
uniform mat4 model_view;
uniform mat4 view;
uniform sampler2D brdf_lut;

in Fragment fragment;
layout(location = 0) out vec4 color;
layout(location = 1) out vec4 albedo_out;

// Defined in functions.frag
float rand(vec2 co);
vec3 box_correct(const vec3 box_extent, const vec3 box_pos, const vec3 dir, const vec3 fragment_position);
float sample_variance_shadow_map(sampler2D shadow_map, vec2 uv, float compare);
float sample_shadow_map(sampler2D shadow_map, const vec2 uv, const float compare);
float distribution_GGX(vec3 N, vec3 H, float roughness);
float geometry_schlick_GGX(float NdotV, float roughness);
float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnel_schlick(float cosTheta, vec3 F0);
vec3 fresnel_schlick_roughness(float cosTheta, vec3 F0, float roughness);
float fog_attenuation(const float dist, const float factor);

void main() {
    vec3 N = fragment.normal;

    vec4 albedo_from_map = texture(material.albedo_map, fragment.uv);
    vec3 albedo = mix(material.albedo.rgb, albedo_from_map.rgb, albedo_from_map.a);

    vec4 emission_from_map = texture(material.emission_map, fragment.uv);
    vec3 emission = mix(material.emission.rgb, emission_from_map.rgb, emission_from_map.a);

    float metallic = material.metallic;

    float roughness = material.roughness;

    float ambient_occlusion = material.ambient_occlusion;

    vec3 V = normalize(camera.position - fragment.position);
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
            const float cos_dir = dot(L, -light.direction);
            const float spot_effect = smoothstep(cos(light.angle / 2.0), cos(light.angle / 2.0 - 0.1), cos_dir);

            const vec3 shadow_map_uv = fragment.proj_shadow[i].xyz / fragment.proj_shadow[i].w;
            const float shadow = sample_shadow_map(shadow_maps[i], shadow_map_uv.xy, shadow_map_uv.z);

            const float light_fragment_distance = distance(light.position, fragment.position);
            const float attenuation = 1.0 / (light_fragment_distance * light_fragment_distance);
            const vec3 radiance = light.strength * 0.09 * light.color * attenuation;

            const float NDF = distribution_GGX(N, H, roughness);
            const float G = geometry_smith(N, V, L, roughness);
            const vec3 F = fresnel_schlick(clamp(dot(H, V), 0.0, 1.0), F0);

            const vec3 nominator    = NDF * G * F;
            const float denominator = 4 * NdotV * NdotL + 0.001;
            const vec3 specular = nominator / denominator;

            const vec3 kS = F;
            const vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);

            direct += (kD * albedo / PI + specular) * radiance * NdotL * spot_effect * shadow;
        }
    }

    vec3 ambient = vec3(0.0, 0.0, 0.0);

    color.rgb = (direct + ambient) + material.emission;
    color.a = clamp(material.alpha * (albedo_from_map.a + material.albedo.a), 0.0, 1.0);

    //Fog
    float distance = distance(fragment.position, camera.position);
    float fog_att = fog_attenuation(distance, fog.attenuation_factor);
    vec3 fog_color = mix(fog.color_far, fog.color_near, fog_att);
    color.rgb = mix(fog_color, color.rgb, clamp(fog_att, 0.45, 1.0));
    albedo_out = vec4(albedo, 1.0);
}
