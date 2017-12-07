#version 330

const int max_decals = 20;
const float PI = 3.14159265359;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emission;
    float specular_exponent;
    float opacity;
    sampler2D diffuse_map;
    sampler2D emission_map;
    sampler2D light_map;
    sampler2D normal_map;
};

struct Light {
    vec3 position;
    vec3 diffuse;
    vec3 specular;
    mat4 view;
    mat4 projection;
    float linear_attenuation_factor;
    float quadratic_attenuation_factor;
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
    samplerCube texture;
};

struct Fog {
    vec3 color_near;
    vec3 color_far;
    float near;
    float far;
    float linear_factor;
    float exponential_factor;
    float exponential_attenuation_factor;
    float exponential_power;
};

struct Fragment {
    vec3 position;
    vec3 normal;
    vec2 uv;
    vec2 light_map_uv;
    vec4 proj_coords[max_decals];
    vec4 proj_shadow;
    vec3 camera_to_surface;
    mat3 tbn;
};

uniform Material material;
uniform Light light;
uniform Material decal_materials[max_decals];
uniform Environment environment;
uniform Camera camera;
uniform Fog fog;
//uniform sampler2D shadow_map;
uniform mat4 model;
uniform mat4 model_view;
uniform mat4 view;
uniform mat4 depth_bias_model_view_projection;

in Fragment fragment;
layout(location = 0) out vec4 color;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float fog_attenuation(const float dist, const Fog fog) {
    float linear = clamp((fog.far - dist) / (fog.far - fog.near), 0.0, 1.0) ;
    float exponential = 1.0 / exp(pow(dist * fog.exponential_attenuation_factor, fog.exponential_power));
    return linear * fog.linear_factor + exponential * fog.exponential_factor;
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
    float variance = max(moments.y - moments.x * moments.x, 0.00002);

    float d = compare - moments.x;
    float p_max = variance / (variance + d*d);

    return clamp(max(p, p_max), 0.0, 1.0);
}

void main() {

    vec4 static_light = texture(material.light_map, fragment.light_map_uv);

    vec3 normal = fragment.normal;

    vec3 tex_normal = normalize(texture(material.normal_map, fragment.uv).rgb * 2.0 - vec3(1.0));
    tex_normal = normalize(fragment.tbn * tex_normal);
    float amount = texture(material.normal_map, fragment.uv).a;
    if (amount > 0.0f){
        normal = normalize(mix(normal, tex_normal, amount));
    }

   vec4 tex_color = texture(material.diffuse_map, fragment.uv);
    vec4 diffuse_color = vec4(1.0, 0.0, 1.0, 1.0); // Rename to albedo?
    //TODO: Shouldnt it be tex_color.a * material.opacity?
    diffuse_color = vec4(mix(material.diffuse * material.opacity, tex_color.rgb, tex_color.a), 1.0);

    for (int i = 0; i < max_decals; i++){
        if (fragment.proj_coords[i].w > 0.0){
            vec2 d_uv = fragment.proj_coords[i].xy / fragment.proj_coords[i].w;
            vec4 decal = texture(decal_materials[i].diffuse_map, d_uv);
            diffuse_color.rgb = mix(diffuse_color.rgb, decal.rgb, decal.a);

            vec3 decal_normal = normalize(texture(decal_materials[i].normal_map, d_uv).rgb * 2.0 - vec3(1.0));
            decal_normal = normalize(fragment.tbn * decal_normal);
            float amount = texture(decal_materials[i].normal_map, fragment.uv).a;
            if (amount > 0.0f){
                normal = normalize(mix(normal, decal_normal, amount));
            }
        }
    }

    vec3 surface_to_light = normalize(light.position - fragment.position); // TODO: Do in vertex shader ?
    float diffuse_contribution = max(dot(normal, surface_to_light), 0.0); // NdotL
    diffuse_contribution = clamp(diffuse_contribution, 0.0, 1.0);

    float cosDir = dot(surface_to_light, -light.direction);
    float spotEffect = smoothstep(cos(light.angle / 2.0), cos(light.angle / 2.0 - 0.1), cosDir);

    float light_fragment_distance = distance(light.position, fragment.position);
    float attenuation = 1.0 / (light_fragment_distance * light_fragment_distance);
    vec3 radiance = light.diffuse * attenuation;

    //vec4 diffuse = vec4(attenuation * diffuse_contribution * light.diffuse, 1.0) * diffuse_color;
    vec4 diffuse = vec4((diffuse_color.rgb / PI) * radiance * diffuse_contribution, 1.0f);

    vec3 corrected_normal = parallax_correct(environment.extent, environment.position,normal);

    vec2 t_size = textureSize(environment.texture, 0);
    vec4 diffuse_environment = textureLod(environment.texture, corrected_normal, int(t_size.x / 20.0));
    diffuse_environment.rgb *= diffuse_color.rgb * (1.0f / 3.14);

    vec3 r = -reflect(fragment.camera_to_surface, normal);
    vec3 corrected_r = parallax_correct(environment.extent, environment.position, r);

    vec4 specular_environment = texture(environment.texture, corrected_r, (1.0 - (material.specular_exponent / 512)) * 10.0);
    specular_environment.rgb *= material.specular;

    vec4 specular = vec4(0.0, 0.0, 0.0, 0.0);
    vec3 halfway = normalize(surface_to_light + fragment.camera_to_surface);
    specular = vec4(pow(max(dot(normal, halfway), 0.0), material.specular_exponent) * light.specular * material.specular, 1.0);

    vec4 diffuse_static = static_light * diffuse_color;
    vec3 environment = diffuse_environment.rgb + specular_environment.rgb;

    diffuse.rgb *= spotEffect;
    specular.rgb *= spotEffect;

    vec3 shadow_map_uv = fragment.proj_shadow.xyz / fragment.proj_shadow.w;
    float current_depth = shadow_map_uv.z;
    float shadow = sample_variance_shadow_map(light.shadow_map, shadow_map_uv.xy, current_depth);
    diffuse.rgb *= shadow;
    specular.rgb*= shadow;

    vec4 emission_tex = texture(material.emission_map, fragment.uv);
    vec3 emission = mix(material.emission, emission_tex.rgb, emission_tex.a);

    color = vec4(diffuse.rgb + diffuse_static.rgb + environment.rgb + specular.rgb + material.ambient + emission, material.opacity);
    color.a = material.opacity + tex_color.a + emission_tex.a;

    //Fog
    float distance = distance(fragment.position, camera.position);
    float fog_att = fog_attenuation(distance, fog);
    vec3 fog_color = mix(fog.color_far, fog.color_near, fog_att);
    color.rgb = mix(fog_color, color.rgb, fog_att);

}
