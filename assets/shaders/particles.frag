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

void main() {
    vec3 Lo = vec3(0.0, 0.0, 0.0);
    vec3 N = normalize(-fragment_position);

    vec3 V = normalize(camera.position - fragment_position);

    bool has_albedo_map = textureSize(tex, 0).x != 1;
    vec4 albedo_from_map = texture(tex, gl_PointCoord);
    vec4 albedo = has_albedo_map ? albedo_from_map.rgba : fragment_color.rgba;

    for(int i = 0; i < lights.length(); i++) {
      Light light = lights[i];
      if (light.strength > 0.0) {
        float light_fragment_distance = distance(light.position, fragment_position);
        float attenuation = 1.0 / (light_fragment_distance * light_fragment_distance);
        vec3 radiance = light.strength * 0.09 * light.color * attenuation;

        vec3 L = normalize(light.position - fragment_position);

        vec3 kD = vec3(1.0);

        float NdotL = 1.0;
        float cos_dir = dot(L, -light.direction);
        float spot_effect = smoothstep(cos(light.angle / 2.0), cos(light.angle / 2.0 - 0.1), cos_dir);

        Lo += (kD * albedo.rgb / PI) * radiance * NdotL * spot_effect;
      }
    }
    if (Lo.x < 0.9){
      discard;
    }
    color = vec4(Lo, albedo.a * fragment_opacity);
}
