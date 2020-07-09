#version 430 core

const float PI = 3.14159265359;

struct Spot_light {
    vec3 position;
    vec3 color;
    float strength;
    mat4 view;
    mat4 projection;
    float angle;
    vec3 direction;
};

struct Directional_light{
  vec3 position;
  vec3 direction;
  vec3 color;
  float strength;
};


struct Camera {
    vec3 position;
};

struct Environment {
    vec3 position;
    vec3 extent;
    float strength;
    float falloff;
};

in vec3 fragment_position;
in vec3 fragment_color;
in float fragment_alpha;

layout(location = 0) out vec4 color;

uniform sampler2D texture_sampler;
uniform bool emissive;
uniform Spot_light[4] spot_lights;
uniform Directional_light directional_light;
uniform Camera camera;
uniform Environment[2] environments;
uniform samplerCube[2] environment_samplers;

bool inside_box(const vec3 point, const vec3 position, const vec3 extent);

void main() {
    vec3 direct = vec3(0.0, 0.0, 0.0);
    vec3 N = normalize(-fragment_position);

    vec3 V = normalize(camera.position - fragment_position);

    vec2 temp = gl_PointCoord - vec2(0.5);
    float f = dot(temp, temp);
    if (f>0.25) discard;

    bool has_albedo_map = textureSize(texture_sampler, 0).x != 1;
    vec4 albedo_from_map = texture(texture_sampler, gl_PointCoord);
    vec3 albedo = has_albedo_map ? albedo_from_map.rgb : fragment_color.rgb;

    float alpha = fragment_alpha * (has_albedo_map ? albedo_from_map.a : (0.5 - length(temp)));

    if (!emissive){
      //Spot light simplified
      for(int i = 0; i < spot_lights.length(); i++) {
          //TODO: Make shade_direct method
          Spot_light light = spot_lights[i];
          if (light.strength > 0.0) {
            float light_fragment_distance = distance(light.position, fragment_position);
            float attenuation = 1.0 / (light_fragment_distance * light_fragment_distance);
            vec3 radiance = light.strength * 0.2 * light.color * attenuation;

            vec3 L = normalize(light.position - fragment_position);

            vec3 kD = vec3(1.0);

            float NdotL = 0.05; // Magic?
            float cos_dir = dot(L, -light.direction);
            float spot_effect = smoothstep(cos(light.angle / 2.0), cos(light.angle / 2.0 - 0.1), cos_dir);

            direct += (kD * albedo.rgb / PI) * radiance * NdotL * spot_effect;
          }
       }

      //Directional light, simplified
      if (directional_light.strength > 0.0) {
        const vec3 L = normalize(-directional_light.direction);
        const vec3 H = normalize(V + L);

        const vec3 radiance = directional_light.strength * directional_light.color;

        vec3 kD = vec3(1.0);

        float NdotL = 0.05; // Magic?
        float cos_dir = dot(L, -directional_light.direction);

        direct += (kD * albedo.rgb / PI) * radiance * NdotL;
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
    color = vec4(alpha * (direct + ambient + albedo * float(emissive)), alpha);
}
