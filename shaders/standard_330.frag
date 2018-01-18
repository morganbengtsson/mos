#version 330

const int max_decals = 10;
const float PI = 3.14159265359;

struct Material {
    vec3 albedo;
    float roughness;
    float metallic;
    float opacity;
    sampler2D albedo_map;
    sampler2D light_map;
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
    vec2 light_map_uv;
    mat3 tbn;
    vec4 proj_coords[max_decals];
    vec4 proj_shadow;
    vec3 camera_to_surface;
};

uniform Material material;
uniform Light light;
uniform Material decal_materials[max_decals];
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

float rand(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float fog_attenuation(const float dist, const Fog fog) {
    return 1.0 / exp(pow(dist * fog.attenuation_factor, 2.0));
}

vec3 parallax_correct(const vec3 box_extent, const vec3 box_pos, const vec3 dir){
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

float linstep(float low, float high, float v){
    return clamp((v-low)/(high-low), 0.0, 1.0);
}

float sample_variance_shadow_map(sampler2D shadow_map, vec2 uv, float compare){
    vec2 moments = vec2(0.0, 0.0);
    vec2 texelSize = 1.0 / textureSize(light.shadow_map, 0);
    for(float x = -1.5; x <= 1.5; ++x) {
        for(float y = -1.5; y <= 1.5; ++y) {
            moments += textureLod(light.shadow_map, uv + vec2(x, y) * texelSize, 0).xy;
        }
    }
    moments /= 16.0;

    float p = step(compare, moments.x);
    float variance = max(moments.y - moments.x * moments.x, 0.000002);

    float d = compare - moments.x;
    float p_max = variance / (variance + d*d);

    return clamp(max(p, p_max), 0.0, 1.0);
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

    vec3 static_light = texture(material.light_map, fragment.light_map_uv).rgb;

    vec3 normal = fragment.normal;

    vec3 normal_from_map = texture(material.normal_map, fragment.uv).rgb * 2.0 - vec3(1.0);
    normal_from_map = normalize(fragment.tbn * normal_from_map);

    float amount = texture(material.normal_map, fragment.uv).a;
    if (amount > 0.0f){
        normal = normalize(mix(normal, normal_from_map, amount));
    }

    /*
    vec3 tangentNormal = texture(material.normal_map, fragment.uv).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(fragment.position);
    vec3 Q2  = dFdy(fragment.position);
    vec2 st1 = dFdx(fragment.uv);
    vec2 st2 = dFdy(fragment.uv);

    vec3 N2   = normalize(fragment.normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N2, T));
    mat3 TBN = mat3(T, B, N2);
    */

    //normal = normalize(TBN * tangentNormal);

    vec4 albedo_from_map = texture(material.albedo_map, fragment.uv);
    vec3 albedo = mix(material.albedo * material.opacity, albedo_from_map.rgb, albedo_from_map.a);

    vec4 metallic_from_map = texture(material.metallic_map, fragment.uv);
    float metallic = mix(material.metallic, metallic_from_map.r, metallic_from_map.a);

    vec4 roughnesss_from_map = texture(material.roughness_map, fragment.uv);
    float roughness = mix(material.roughness, roughnesss_from_map.r, roughnesss_from_map.a);

    float ambient_occlusion = texture(material.ambient_occlusion_map, fragment.uv).r;

    //TODO: Function

    for (int i = 0; i < max_decals; i++){
        if (fragment.proj_coords[i].w > 0.0){
            vec2 d_uv = fragment.proj_coords[i].xy / fragment.proj_coords[i].w;

            //TODO: Do nicer
            if (d_uv.x < 1.0 && d_uv.x > 0.0 && d_uv.y < 1.0 && d_uv.y > 0.0) {
                vec4 decal = texture(decal_materials[i].albedo_map, d_uv);
                albedo.rgb = mix(albedo.rgb, decal.rgb, decal.a);

                vec3 decal_normal = normalize(texture(decal_materials[i].normal_map, d_uv).rgb * 2.0 - vec3(1.0));
                decal_normal = normalize(fragment.tbn * decal_normal);
                float amount = texture(decal_materials[i].normal_map, fragment.uv).a;
                if (amount > 0.0f){
                    normal = normalize(mix(normal, decal_normal, amount));
                }
            }
        }
    }
    vec3 diffuse_static = static_light * albedo;

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

    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

    float cos_dir = dot(L, -light.direction);
    float spot_effect = smoothstep(cos(light.angle / 2.0), cos(light.angle / 2.0 - 0.1), cos_dir);
    Lo.rgb *= spot_effect;

    vec3 shadow_map_uv = fragment.proj_shadow.xyz / fragment.proj_shadow.w;
    float shadow = sample_variance_shadow_map(light.shadow_map, shadow_map_uv.xy, shadow_map_uv.z);

    Lo.rgb *= shadow;

    vec2 environment_texture_size = textureSize(environment.texture, 0);
    vec3 corrected_normal = parallax_correct(environment.extent, environment.position,normal);
    vec3 r = -reflect(fragment.camera_to_surface, normal);
    vec3 corrected_r = parallax_correct(environment.extent, environment.position, r);

    vec3 cube_r = vec3(corrected_r.x, corrected_r.z, corrected_r.y);
    vec3 cube_n = vec3(corrected_normal.x, corrected_normal.z, corrected_normal.y);

    float maxsize = max(environment_texture_size.x, environment_texture_size.x);
    float num_levels = log2(maxsize) + 1;
    float mip_level = roughness * num_levels * 3.0;

    vec3 F_env = fresnel_schlick_roughness(max(dot(N, V), 0.0), F0, roughness);
    vec3 kS_env = F_env;
    vec3 kD_env = 1.0 - kS_env;
    kD_env *= 1.0 - metallic;

    vec3 filtered = textureLod(environment.texture, cube_r, mip_level).rgb;
    vec2 brdf  = texture(brdf_lut, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular_environment = filtered * (F_env * brdf.x + brdf.y) * environment.strength;

    vec3 irradiance = textureLod(environment.texture, cube_n, 20.0).rgb;
    vec3 diffuse_environment = irradiance * albedo * environment.strength;

    vec3 ambient = (kD_env * diffuse_environment + specular_environment) * ambient_occlusion;

    color.rgb = Lo + diffuse_static + ambient;
    color.a = material.opacity + albedo_from_map.a;

    //Fog
    float distance = distance(fragment.position, camera.position);
    float fog_att = fog_attenuation(distance, fog);
    vec3 fog_color = mix(fog.color_far, fog.color_near, fog_att);
    color.rgb = mix(fog_color, color.rgb, fog_att);
}
