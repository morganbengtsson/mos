#version 330
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float specular_exponent;
    float opacity;
};

struct Light {
    vec3 position;
    vec3 diffuse;
    vec3 specular;
    vec3 ambient;
    mat4 view;
    mat4 projection;
};

struct Fog {
    vec3 color;
    float near;
    float far;
};

struct Fragment {
    vec3 position;
    vec3 normal;
    vec2 uv;
    vec2 light_map_uv;
    vec3 shadow;
    vec3 camera_to_surface;
    mat3 tbn;
};

uniform bool receives_light;
uniform vec3 multiply = vec3(1.0, 1.0, 1.0);
uniform Material material;
uniform Light light;
uniform Fog fog = Fog(vec3(1.0, 1.0, 1.0), 200.0, 300.0);
uniform sampler2D diffuse_map;
uniform sampler2D light_map;
uniform sampler2D normal_map;
uniform sampler2D shadow_map;
uniform sampler2D diffuse_environment_map;
uniform sampler2D specular_environment_map;
uniform mat4 model;
uniform mat4 model_view;
uniform mat4 view;
uniform vec4 overlay = vec4(0.0, 0.0, 0.0, 0.0);
in Fragment fragment;
layout(location = 0) out vec4 color;

float fog_linear(
  const float dist,
  const float start,
  const float end
) {
  return 1.0 - clamp((end - dist) / (end - start), 0.0, 1.0);
}

vec4 textureEquirectangular(const sampler2D tex, const vec3 direction){
    vec3 r = direction;
    vec2 tc;
    tc.y = r.z;
    r.z = 0.0;
    tc.x = normalize(r).x * 0.5;

    float s = sign(r.y) * 0.5;

    tc.s = 0.75 - s * (0.5 - tc.s);
    tc.t = 0.5 + 0.5 * tc.t;

    // Sample from scaled and biased texture coordinate
    color = texture(tex, tc);
    return color;
}

void main() {

    vec4 static_light = texture(light_map, fragment.light_map_uv);

    vec3 normal = fragment.normal;

    vec3 tex_normal = normalize(texture(normal_map, fragment.uv).rgb * 2.0 - vec3(1.0));
    tex_normal = normalize(fragment.tbn * tex_normal);

    normal = normalize(mix(normal, tex_normal, texture(normal_map, fragment.uv).a));

    vec3 surface_to_light = normalize(light.position - fragment.position); // TODO: Do in vertex shader ?
    float diffuse_contribution = max(dot(normal, surface_to_light), 0.0);
    diffuse_contribution = clamp(diffuse_contribution, 0.0, 1.0);

    vec4 tex_color = texture(diffuse_map, fragment.uv);
    vec4 diffuse_color = vec4(1.0, 0.0, 1.0, 1.0);
    diffuse_color = vec4(mix(material.diffuse * material.opacity, tex_color.rgb, tex_color.a), 1.0);

    float dist = distance(light.position, fragment.position);
    float a = 0.0; //TODO: set in light
    float b = 0.0; // TODO: set in light
    float att = 1.0 / (1.0 + a*dist + b*dist*dist);

    vec4 diffuse = vec4(att * diffuse_contribution * light.diffuse, 1.0) * diffuse_color;

    vec4 diffuse_environment = textureEquirectangular(diffuse_environment_map, normal);
    diffuse_environment.rgb *= material.diffuse;

    vec3 r = reflect(fragment.camera_to_surface, normalize(normal));
    vec4 specular_environment = textureEquirectangular(specular_environment_map, r);
    specular_environment.rgb *= material.specular;

    vec4 specular = vec4(0.0, 0.0, 0.0, 0.0);
    vec3 halfway = normalize(surface_to_light + fragment.camera_to_surface);
    specular = vec4(pow(max(dot(normal, halfway), 0.0), material.specular_exponent) * light.specular * material.specular, 1.0);

    vec4 diffuse_static = static_light * diffuse_color;

    //Ambient
    //vec3 ambient = material.ambient * light.ambient;
    vec3 ambient = light.ambient * diffuse_color.rgb;

    if(receives_light == true) {
        color = vec4(diffuse.rgb + diffuse_static.rgb + specular.rgb + ambient + diffuse_environment.rgb + specular_environment.rgb, material.opacity);
    }
    else {
        color = vec4(diffuse_color.rgb, material.opacity);
    }
    color.a = material.opacity + tex_color.a;

    //Multiply
    color.rgb *= multiply;

    //Fog
    float distance = length(fragment.position.xyz);
    color.rgb = mix(color.rgb, fog.color, fog_linear(distance, fog.near, fog.far));
    color.rgb = mix(color.rgb, overlay.rgb, overlay.a);

     //Shadow test, not that great yet.
#ifdef SHADOWMAPS
    float closest_depth = texture(shadowmap, fragment.shadow.xy).x;
    float depth = fragment.shadow.z;
    float bias = 0.005;
    float shadow = closest_depth < depth - bias  ? 0.0 : 1.0;
    color.rgb *= shadow;
#endif
}
