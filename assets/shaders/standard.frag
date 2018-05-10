#version 330

const float PI = 3.14159265359;

struct Material {
    vec3 albedo;
    float roughness;
    float metallic;
    float opacity;
    vec3 emission;
    float ambient_occlusion;
    sampler2D albedo_map;
    sampler2D emission_map;
    sampler2D normal_map;
    sampler2D metallic_map;
    sampler2D roughness_map;
    sampler2D ambient_occlusion_map;
};

struct Light {
    vec3 position;
    vec3 color;
    mat4 view;
    mat4 projection;
    float angle;
    vec3 direction;
    sampler2D shadow_map;
};

struct Camera {
    vec3 position;
    vec2 resolution;
};

struct Environment {
    vec3 position;
    vec3 extent;
    float strength;
    samplerCube texture;
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
    vec4 proj_shadow;
    vec3 camera_to_surface;
    float ao;
};

uniform Material material;
uniform Light light;
uniform Environment environment;
uniform Camera camera;
uniform Fog fog;
uniform mat4 model;
uniform mat4 model_view;
uniform mat4 view;
uniform mat4 depth_bias_model_view_projection;
uniform sampler2D brdf_lut;

in Fragment fragment;
layout(location = 0) out vec4 color;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float fog_attenuation(const float dist, const Fog fog) {
    return 1.0 / exp(pow(dist * fog.attenuation_factor, 2.0));
}

vec3 box_correct(const vec3 box_extent, const vec3 box_pos, const vec3 dir){
    vec3 box_min = box_pos - box_extent;
    vec3 box_max = box_pos + box_extent;
    vec3 nrdir = normalize(dir);
    vec3 rbmax = (box_max - fragment.position)/nrdir;
    vec3 rbmin = (box_min - fragment.position)/nrdir;

    vec3 rbminmax = max(rbmax, rbmin);
    float fa = min(min(rbminmax.x, rbminmax.y), rbminmax.z);

    vec3 posonbox = fragment.position + nrdir*fa;
    vec3 rdir = normalize(posonbox - box_pos);
    return rdir;
}

float sample_variance_shadow_map(sampler2D shadow_map, vec2 uv, float compare){
    vec2 moments = texture(shadow_map, uv).xy;

    float p = step(compare, moments.x);
    float variance = max(moments.y - moments.x * moments.x, 0.000002);

    float d = compare - moments.x;
    float p_max = variance / (variance + d*d);

    return clamp((max(p, p_max) - 0.8) * 5, 0.0, 1.0);
}

float sample_shadow_map(sampler2D shadow_map, const vec2 uv, const float compare) {
    return float(texture(shadow_map, uv).x > (compare - 0.005));
}

float distribution_GGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.001);
}

float geometry_schlick_GGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = geometry_schlick_GGX(NdotV, roughness);
    float ggx1 = geometry_schlick_GGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnel_schlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnel_schlick_roughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

void main() {
    vec3 normal = fragment.normal;

    vec3 normal_from_map = texture(material.normal_map, fragment.uv).rgb * 2.0 - vec3(1.0);
    normal_from_map = normalize(fragment.tbn * normal_from_map);

    float amount = texture(material.normal_map, fragment.uv).a;
    if (amount > 0.0f){
        normal = normalize(mix(normal, normal_from_map, amount));
    }

    vec4 albedo_from_map = texture(material.albedo_map, fragment.uv);
    vec3 albedo = mix(material.albedo, albedo_from_map.rgb, albedo_from_map.a);

    vec4 metallic_from_map = texture(material.metallic_map, fragment.uv);
    float metallic = mix(material.metallic, metallic_from_map.r, metallic_from_map.a);

    vec4 roughnesss_from_map = texture(material.roughness_map, fragment.uv);
    float roughness = mix(material.roughness, roughnesss_from_map.r, roughnesss_from_map.a);

    float ambient_occlusion_from_map = texture(material.ambient_occlusion_map, fragment.uv).r;
    float ambient_occlusion = material.ambient_occlusion * ambient_occlusion_from_map;

    vec4 emission_from_map = texture(material.emission_map, fragment.uv);
    vec3 emission = mix(material.emission, emission_from_map.rgb, emission_from_map.a);

    float light_fragment_distance = distance(light.position, fragment.position);
    float attenuation = 1.0 / (light_fragment_distance * light_fragment_distance);
    vec3 radiance = light.color * attenuation;

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 N = normalize(normal);
    vec3 V = normalize(camera.position - fragment.position);
    vec3 L = normalize(light.position - fragment.position);
    vec3 H = normalize(V + L);

    // Cook-Torrance BRDF
    float NDF = distribution_GGX(N, H, material.roughness);
    float G = geometry_smith(N, V, L, material.roughness);
    vec3 F = fresnel_schlick(clamp(dot(H, V), 0.0, 1.0), F0);

    vec3 nominator    = NDF * G * F;
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    vec3 specular = nominator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);
    float cos_dir = dot(L, -light.direction);
    float spot_effect = smoothstep(cos(light.angle / 2.0), cos(light.angle / 2.0 - 0.1), cos_dir);

    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL * spot_effect;

    vec3 shadow_map_uv = fragment.proj_shadow.xyz / fragment.proj_shadow.w;

    float shadow = 0.0f;
    vec2 texelSize = 1.0 / textureSize(light.shadow_map, 0);
    for(float x = -3; x <= 3; x += 2) {
        for(float y = -3; y <= 3; y += 2) {
            shadow += sample_variance_shadow_map(light.shadow_map, shadow_map_uv.xy + vec2(x, y) * texelSize, shadow_map_uv.z);
        }
    }
    shadow /= 16;
    Lo.rgb *= shadow;

    vec3 corrected_normal = box_correct(environment.extent, environment.position,normal);
    vec3 r = -reflect(fragment.camera_to_surface, normal);
    vec3 corrected_r = box_correct(environment.extent, environment.position, r);

    vec2 environment_texture_size = textureSize(environment.texture, 0);
    float maxsize = max(environment_texture_size.x, environment_texture_size.x);
    float num_levels = 1 + floor(log2(maxsize));
    float mip_level = roughness * num_levels * 3.0;

    vec3 F_env = fresnel_schlick_roughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kS_env = F_env;
    vec3 kD_env = 1.0 - kS_env;
    kD_env *= 1.0 - metallic;

    vec3 filtered = textureLod(environment.texture, corrected_r, mip_level).rgb;
    vec2 brdf  = texture(brdf_lut, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular_environment = filtered * (F_env * brdf.x + brdf.y) * environment.strength;

    vec3 irradiance = textureLod(environment.texture, corrected_normal, num_levels - 2).rgb;
    irradiance += textureLod(environment.texture, corrected_normal, num_levels - 1).rgb;
    irradiance += textureLod(environment.texture, corrected_normal, num_levels).rgb;
    irradiance /= 3.0f;

    vec3 diffuse_environment = irradiance * albedo * environment.strength;

    float fragment_environment_distance_x = 1.0 - smoothstep(0.5, 1.0, distance(fragment.position.x, environment.position.x) / environment.extent.x);
    float fragment_environment_distance_y = 1.0 - smoothstep(0.5, 1.0, distance(fragment.position.y, environment.position.y) / environment.extent.y);
    float fragment_environment_distance_z = 1.0 - smoothstep(0.5, 1.0, distance(fragment.position.z, environment.position.z) / environment.extent.z);

    float environment_attenuation = min(fragment_environment_distance_x, min(fragment_environment_distance_y, fragment_environment_distance_z));

    vec3 ambient = (kD_env * diffuse_environment + specular_environment) * ambient_occlusion * environment_attenuation;

    color.rgb = (Lo + ambient + emission) * fragment.ao;
    color.a = clamp(material.opacity + albedo_from_map.a, 0.0, 1.0);

    //Fog
    float distance = distance(fragment.position, camera.position);
    float fog_att = fog_attenuation(distance, fog);
    vec3 fog_color = mix(fog.color_far, fog.color_near, fog_att);
    color.rgb = mix(fog_color, color.rgb, fog_att);
}
