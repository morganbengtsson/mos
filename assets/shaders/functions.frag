#version 430 core

const float PI = 3.14159265359;

float rand(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

mat3 rotate(const vec3 axis, const float a) {
  const float s=sin(a);
  const float c=cos(a);
  const float oc=1.0-c;
  const vec3 as=axis*s;
  const mat3 p=mat3(axis.x*axis,axis.y*axis,axis.z*axis);
  const mat3 q=mat3(c,-as.z,as.y,as.z,c,-as.x,-as.y,as.x,c);
  return p*oc+q;
}

vec3 box_correct(const vec3 box_extent, const vec3 box_pos, const vec3 dir, const vec3 fragment_position) {
    vec3 box_min = box_pos - box_extent;
    vec3 box_max = box_pos + box_extent;
    vec3 nrdir = normalize(dir);
    vec3 rbmax = (box_max - fragment_position)/nrdir;
    vec3 rbmin = (box_min - fragment_position)/nrdir;

    vec3 rbminmax = max(rbmax, rbmin);
    float fa = min(min(rbminmax.x, rbminmax.y), rbminmax.z);

    vec3 posonbox = fragment_position + nrdir*fa;
    vec3 rdir = normalize(posonbox - box_pos);
    return rdir;
}

float sample_variance_shadow_map(sampler2D shadow_map, vec2 uv, float compare) {
    vec2 moments = texture(shadow_map, uv).xy;

    float p = step(compare, moments.x);
    float variance = max(moments.y - moments.x * moments.x, 0.00002);

    float d = compare - moments.x;
    float p_max = variance / (variance + d*d);

    return clamp((max(p, p_max) - 0.5) * 2, 0.0, 1.0);
}

float sample_shadow_map(sampler2D shadow_map, const vec2 uv, const float compare) {
    return float(texture(shadow_map, uv).x > (compare - 0.005));
}

float distribution_GGX(vec3 N, vec3 H, float roughness) {
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float geometry_schlick_GGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = geometry_schlick_GGX(NdotV, roughness);
    float ggx1 = geometry_schlick_GGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnel_schlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnel_schlick_roughness(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float fog_attenuation(const float dist, const float factor) {
    return 1.0 / exp(pow(dist * factor, 2.0));
}