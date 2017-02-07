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
    float linear_attenuation_factor;
    float quadratic_attenuation_factor;
};

struct Camera {
    vec3 position;
};

struct Fog {
    vec3 color;
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
    //vec2 decal_uvs[5];
    vec3 shadow;
    vec3 camera_to_surface;
    mat3 tbn;
};

uniform bool receives_light;
uniform vec3 multiply = vec3(1.0, 1.0, 1.0);
uniform Material material;
uniform Light light;
uniform Camera camera;
uniform Fog fog = Fog(vec3(1.0, 1.0, 1.0), 200.0, 300.0, 1.0, 0.0, 0.0, 1.0);
uniform sampler2D diffuse_map;
uniform sampler2D light_map;
uniform sampler2D normal_map;
uniform sampler2D shadow_map;
uniform samplerCube environment_map;
//uniform sampler2D decal_maps[5];
uniform mat4 model;
uniform mat4 model_view;
uniform mat4 view;
uniform vec4 overlay = vec4(0.0, 0.0, 0.0, 0.0);
in Fragment fragment;
layout(location = 0) out vec4 color;

float fog_attenuation(const float dist, const Fog fog) {
    float linear = clamp((fog.far - dist) / (fog.far - fog.near), 0.0, 1.0) ;
    float exponential = 1.0 / exp(pow(dist * fog.exponential_attenuation_factor, fog.exponential_power));
    return linear * fog.linear_factor + exponential * fog.exponential_factor;
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
    vec4 diffuse_color = vec4(1.0, 0.0, 1.0, 1.0); // Rename to albedo?
    diffuse_color = vec4(mix(material.diffuse * material.opacity, tex_color.rgb, tex_color.a), 1.0);

    /*
    for (int i = 0; i < 5; i++) {
        //diffuse_color.rgb += texture(decal_maps[i], fragment.decal_uvs[i]).rgb;
        vec4 decal = texture(decal_maps[i], fragment.decal_uvs[i]);
        diffuse_color.rgb = mix(diffuse_color.rgb, decal.rgb, decal.a);
    }*/

    float dist = distance(light.position, fragment.position);
    float linear_attenuation_factor = light.linear_attenuation_factor;
    float quadratic_attenuation_factor = light.quadratic_attenuation_factor;
    float att = 1.0 / (1.0 + linear_attenuation_factor*dist + quadratic_attenuation_factor*dist*dist);

    vec4 diffuse = vec4(att * diffuse_contribution * light.diffuse, 1.0) * diffuse_color;

    vec4 diffuse_environment = textureLod(environment_map, normal, 9) * 0.5f;
    diffuse_environment.rgb *= diffuse_color.rgb;

    vec3 r = reflect(fragment.camera_to_surface, normal);

    vec3 nrdir = normalize(r);
    vec3 rbmax = ((vec3(2.5, 2.5, 1.25) + vec3(0.0, 0.0, 1.25)) - fragment.position)/nrdir;
    vec3 rbmin = ((vec3(-2.5, -2.5, -1.25) + vec3(0.0, 0.0, 1.25)) - fragment.position)/nrdir;

    vec3 rbminmax = max(rbmax, rbmin);
    float fa = min(min(rbminmax.x, rbminmax.y), rbminmax.z);

    vec3 posonbox = fragment.position+ nrdir*fa;
    vec3 env_box_pos = vec3(0.0, 0.0, 1.25);
    vec3 rdir = posonbox - env_box_pos;

    vec4 specular_environment = texture(environment_map, -rdir, (1.0 - (material.specular_exponent / 512)) * 10.0) * 1.0;
    specular_environment.rgb *= material.specular;

    vec4 specular = vec4(0.0, 0.0, 0.0, 0.0);
    vec3 halfway = normalize(surface_to_light + fragment.camera_to_surface);
    specular = vec4(pow(max(dot(normal, halfway), 0.0), material.specular_exponent) * light.specular * material.specular, 1.0);

    vec4 diffuse_static = static_light * diffuse_color;

    //Ambient
    //vec3 ambient = material.ambient * light.ambient;
    vec3 ambient = light.ambient * diffuse_color.rgb;

    if(receives_light == true) {
        color = vec4(diffuse.rgb + diffuse_static.rgb + diffuse_environment.rgb + specular.rgb + ambient  + specular_environment.rgb, material.opacity);
    }
    else {
        color = vec4(diffuse_color.rgb, material.opacity);
    }
    color.a = material.opacity + tex_color.a;

    //Multiply
    color.rgb *= multiply;

    //Fog
    float distance = distance(fragment.position, camera.position);
    //color.rgb = mix(fog.color, color.rgb, fog_attenuation(distance, fog));
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
