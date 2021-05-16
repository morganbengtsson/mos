#version 430 core

const float PI = 3.14159265359;

float rand(const in vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

mat3 rotate(const in vec3 axis, const in float a) {
  const float s=sin(a);
  const float c=cos(a);
  const float oc=1.0-c;
  const vec3 as=axis*s;
  const mat3 p=mat3(axis.x*axis,axis.y*axis,axis.z*axis);
  const mat3 q=mat3(c,-as.z,as.y,as.z,c,-as.x,-as.y,as.x,c);
  return p*oc+q;
}

vec3 box_correct(const in vec3 box_extent, const in vec3 box_pos, const in vec3 dir, const in vec3 fragment_position) {
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

float sample_variance_shadow_map(const in sampler2D shadow_map, const in vec2 uv, const in float compare) {
    vec2 moments = texture(shadow_map, uv).xy;

    float p = step(compare, moments.x);
    float variance = max(moments.y - moments.x * moments.x, 0.000002);

    float d = compare - moments.x;
    float p_max = variance / (variance + d*d);

    return clamp((max(p, p_max) - 0.5) * 2, 0.0, 1.0);
}

float sample_shadow_map(const in sampler2D shadow_map, const in vec2 uv, const in float compare) {
    return float(texture(shadow_map, uv).x > (compare - 0.005));
}

float distribution_GGX(const in vec3 N, const in vec3 H, const in float roughness) {
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float geometry_schlick_GGX(const in float NdotV, const in float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float geometry_smith(const in vec3 N, const in vec3 V, const in vec3 L, const in float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = geometry_schlick_GGX(NdotV, roughness);
    float ggx1 = geometry_schlick_GGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnel_schlick(const in float cosTheta, const in vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnel_schlick_roughness(const in float cosTheta, const in vec3 F0, const in float roughness) {
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float fog_attenuation(const in float dist, const in float factor) {
    return 1.0 / exp(pow(dist * factor, 2.0));
}

bool inside_box(const in vec3 point, const in vec3 position, const in vec3 extent) {
  vec3 mi = position - extent;
  vec3 ma = position + extent;
  return (mi.x <= point.x
      && point.x <= ma.x
      && mi.y <= point.y
      && point.y <= ma.y
      && mi.z <= point.z
      && point.z <= ma.z);
}

vec3 sample_environment(const in samplerCube sampler, const in vec3 direction,
                        const in float mip_level, const in float angle, const in float step) {
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

float environment_attenuation(const in vec3 point, const in vec3 position,
                              const in vec3 extent, const in float falloff) {
  const float distance_x = smoothstep(abs(position.x) + extent.x,abs(position.x) + extent.x * (1.0 - falloff), abs(point.x));
  const float distance_y = smoothstep(abs(position.y) + extent.y,abs(position.y) + extent.y * (1.0 - falloff), abs(point.y));
  const float distance_z = smoothstep(abs(position.z) + extent.z,abs(position.z) + extent.z * (1.0 - falloff), abs(point.z));
  return min(distance_x, min(distance_y, distance_z));
}

vec3 sample_normal(const in vec3 normal, const in sampler2D normal_sampler,
                   const in mat3 tbn, const in vec2 uv) {
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

vec4 sample_albedo_alpha(const in vec3 albedo, const in float alpha,
                         const in sampler2D sampler, const in vec2 uv) {
  bool has_albedo_map = textureSize(sampler, 0).x != 1;
  vec4 albedo_from_map = texture(sampler, uv);

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
