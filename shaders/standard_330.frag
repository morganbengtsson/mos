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

struct Lightmaps {
    sampler2D first;
    sampler2D second;
    float mix;
};

struct Textures {
    sampler2D first;
    sampler2D second;
};

struct Fog {
    vec3 color;
    float density;
};

struct Fragment {
    vec3 position;
    vec3 normal;
    vec2 uv;
    vec2 lightmap_uv;
    vec3 shadow;
};

uniform bool receives_light;
uniform float multiply = 1.0;
uniform Material material;
uniform Textures textures;
uniform Lightmaps lightmaps;
uniform Light light;
uniform Fog fog = Fog(vec3(1.0, 1.0f, 1.0), 0.0);
uniform sampler2D normalmap;
uniform sampler2D shadowmap;
uniform vec3 overlay = vec3(0.0, 0.0, 0.0);
in Fragment fragment;
layout(location = 0) out vec4 color;

float calculate_fog(float distance, float density) {
    float result = exp(-pow(density * distance, 2.0));
    result = 1.0 - clamp(result, 0.0, 1.0);
    return result;
}

float linear_depth(float value)
{
  float n = 1.0; // camera z near
  float f = 60.0; // camera z far
  float z = value;
  return (2.0 * n) / (f + n - z * (f - n));
}

void main() {

    vec4 static_light = texture2D(lightmaps.first, fragment.lightmap_uv);

    static_light += texture2D(lightmaps.second, fragment.uv);

    vec3 normal = normalize(fragment.normal);

    //TODO: Not correct!
    vec4 tex_normal = texture2D(normalmap, fragment.uv);
    normal = mix(normal, tex_normal.xyz, tex_normal.z);

    vec3 surface_to_light = normalize(light.position - fragment.position);
    float diffuse_contribution = max(dot(normal, surface_to_light), 0.0);
    diffuse_contribution = clamp(diffuse_contribution, 0.0, 1.0);

    vec4 tex_color = texture2D(textures.first, fragment.uv);
    vec4 tex2_color = texture2D(textures.second, fragment.uv);
    tex_color.rgb = mix(tex2_color.rgb, tex_color.rgb, 1.0 - tex2_color.a);

    vec4 diffuse_color = vec4(1.0, 0.0, 1.0, 1.0);
    diffuse_color = vec4(mix(tex_color.rgb, material.diffuse.rgb, 1.0 - tex_color.a), 1.0);
    //diffuse_color.rgb = mix(tex_color.rgb, tex2_color.rgb, tex2_color.a);

    float dist = distance(light.position, fragment.position);
    float a = 1.0;
    float b = 1.0;
    float att = 1.0 / (1.0 + a*dist + b*dist*dist);

    vec4 diffuse = vec4(att * diffuse_contribution* light.diffuse, 1.0) * diffuse_color;

    vec4 specular = vec4(0.0, 0.0, 0.0, 0.0);
    vec3 surface_to_view = normalize(fragment.position);
    vec3 reflection = reflect(normal, -surface_to_light);
    float specular_contribution = pow(max(0.0, dot(surface_to_view, reflection)), material.specular_exponent);
    specular = vec4(specular_contribution * light.specular * material.specular, 1.0);

    vec4 diffuse_static = static_light * diffuse_color;

    //Ambient
    vec3 ambient = material.ambient * light.ambient;

    if(receives_light == true) {
        color = vec4(diffuse.xyz + diffuse_static.xyz + specular.xyz + ambient, material.opacity);
    }
    else {
        color = vec4(diffuse_color.rgb, material.opacity);
    }
    color.a = material.opacity + tex_color.a + tex2_color.a;

    //Multiply
    color.rgb = color.rgb * multiply;
    //Fog
    float distance = gl_FragCoord.z / gl_FragCoord.w;
    color = mix(color, vec4(fog.color, 1.0), calculate_fog(distance, fog.density));
    color.rgb += overlay;

    //Shadow test, not that great yet.
#ifdef SHADOWMAPS
    float closest_depth = texture(shadowmap, fragment.shadow.xy).x;
    float depth = fragment.shadow.z;
    float bias = 0.005;
    float shadow = closest_depth < depth - bias  ? 0.0 : 1.0;
    color.rgb *= shadow;
#endif
}
