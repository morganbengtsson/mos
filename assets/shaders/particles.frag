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

struct Environment {
    vec3 position;
    vec3 extent;
    float strength;
    float falloff;
};

in vec3 fragment_position;
in vec4 fragment_color;
in float fragment_opacity;
layout(location = 0) out vec4 color;

uniform sampler2D tex;
uniform Light[4] lights;
uniform Camera camera;
uniform Environment[2] environments;
uniform samplerCube[2] environment_samplers;

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
    vec3 direct = vec3(0.0, 0.0, 0.0);
    vec3 N = normalize(-fragment_position);

    vec3 V = normalize(camera.position - fragment_position);

    vec2 temp = gl_PointCoord - vec2(0.5);
    float f = dot(temp, temp);
    if (f>0.25) discard;

    bool has_albedo_map = textureSize(tex, 0).x != 1;
    vec4 albedo_from_map = texture(tex, gl_PointCoord);
    vec4 albedo = has_albedo_map ? albedo_from_map.rgba : fragment_color.rgba;

    float alpha = fragment_opacity * (has_albedo_map ? albedo_from_map.a : (0.5 - length(temp)));

    for(int i = 0; i < lights.length(); i++) {
      Light light = lights[i];
      if (light.strength > 0.0) {
        float light_fragment_distance = distance(light.position, fragment_position);
        float attenuation = 1.0 / (light_fragment_distance * light_fragment_distance);
        vec3 radiance = light.strength * 0.09 * light.color * attenuation;

        vec3 L = normalize(light.position - fragment_position);

        vec3 kD = vec3(1.0);

        float NdotL = 0.05; // Magic?
        float cos_dir = dot(L, -light.direction);
        float spot_effect = smoothstep(cos(light.angle / 2.0), cos(light.angle / 2.0 - 0.1), cos_dir);

       direct += (kD * albedo.rgb / PI) * radiance * NdotL * spot_effect;
      }
    }
    vec3 ambient = vec3(0.0, 0.0, 0.0);
    float attenuation = 0.0f;

    for (int i = 0; i < environments.length(); i++) {
      if (environments[i].strength > 0.0 && inside_box(fragment_position, environments[i].position, environments[i].extent)) {

        const float num_levels = textureQueryLevels(environment_samplers[i]);
        vec3 irradiance = textureLod(environment_samplers[i], vec3(1.0, 0.0, 0.0), num_levels).rgb;
        irradiance += textureLod(environment_samplers[i], vec3(-1.0, 0.0, 0.0), num_levels).rgb;
        irradiance += textureLod(environment_samplers[i], vec3(0.0, 1.0, 0.0), num_levels).rgb;
        irradiance += textureLod(environment_samplers[i], vec3(0.0, -1.0, 0.0), num_levels).rgb;
        irradiance += textureLod(environment_samplers[i], vec3(0.0, 0.0, 1.0), num_levels).rgb;
        irradiance += textureLod(environment_samplers[i], vec3(0.0, 0.0, -1.0), num_levels).rgb;
        irradiance /= 6.0;

        const vec3 diffuse_environment = irradiance * albedo.rgb * environments[i].strength;
        ambient += clamp(diffuse_environment, vec3(0.0, 0.0, 0.0), vec3(1.0, 1.0, 1.0));
      }
    }
    color = vec4(alpha * (direct + ambient), alpha);
}
