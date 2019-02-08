#version 430 core

const float PI = 3.14159265359;

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
};

in vec3 fragment_position;
in vec4 fragment_color;
in float fragment_opacity;
layout(location = 0) out vec4 color;

uniform sampler2D tex;
uniform Light[4] lights;
uniform Camera camera;

float distribution_GGX(vec3 N, vec3 H, float roughness);
float geometry_schlick_GGX(float NdotV, float roughness);
float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnel_schlick(float cosTheta, vec3 F0);
vec3 fresnel_schlick_roughness(float cosTheta, vec3 F0, float roughness);

void main() {
    vec3 Lo = vec3(0.0, 0.0, 0.0);
    vec3 N = vec3(0.0, -1.0, 0.0);

    vec3 frag_pos = fragment_position;
    vec3 V = normalize(camera.position - frag_pos);

    vec4 albedo = texture(tex, gl_PointCoord);

    for(int i = 0; i < lights.length(); i++) {
      Light light = lights[i];
      if (light.strength > 0.0) {
        float light_fragment_distance = distance(light.position, frag_pos);
        float attenuation = 1.0 / (light_fragment_distance * light_fragment_distance);
        vec3 radiance = light.strength * 0.09 * light.color * attenuation;

        vec3 L = normalize(light.position - frag_pos);
        vec3 H = normalize(V + L);

        // Cook-Torrance BRDF
        float NDF = distribution_GGX(N, H, 1.0);
        float G = geometry_smith(N, V, L, 1.0);
        vec3 F = fresnel_schlick(clamp(dot(H, V), 0.0, 1.0), vec3(0.0, 0.0, 0.0));

        vec3 nominator    = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
        vec3 specular = nominator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;

        float NdotL = max(dot(N, L), 0.0);
        float cos_dir = dot(L, -light.direction);
        float spot_effect = smoothstep(cos(light.angle / 2.0), cos(light.angle / 2.0 - 0.1), cos_dir);

        Lo += (kD * albedo.rgb / PI) * radiance * NdotL * spot_effect;
      }
    }
    //color = vec4(tex_color.r, tex_color.g, tex_color.b, tex_color.a * fragment_color.a * fragment_opacity);
    color = vec4(Lo, albedo.a * fragment_color.a * fragment_opacity);
}
