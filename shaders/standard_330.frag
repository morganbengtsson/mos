#version 330
uniform bool selected;
uniform float time;
uniform vec3 material_ambient_color;
uniform vec3 material_diffuse_color;
uniform vec3 material_specular_color;
uniform float material_specular_exponent;
uniform float opacity;
uniform sampler2D texture;
uniform sampler2D lightmap;
uniform sampler2D normalmap;
uniform vec3 light_position;
uniform vec3 light_diffuse_color;
uniform vec3 light_specular_color;
uniform bool has_texture;
uniform bool has_lightmap;
uniform bool has_normalmap;
in vec3 fragment_position;
in vec3 fragment_normal;
in vec2 fragment_uv;
in vec2 fragment_lightmap_uv;

layout(location = 0) out vec4 color;

void main() {

    vec4 static_light = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    if (has_lightmap == true){
        static_light = texture2D(lightmap, fragment_lightmap_uv);
    }

    vec3 normal = normalize(fragment_normal);
    if (has_normalmap == true){
        normal = texture2D(normalmap, fragment_uv).xyz;
    }

    vec3 surface_to_light = normalize(light_position - fragment_position);
    float diffuse_contribution = max(dot(normal, surface_to_light), 0.0);
    diffuse_contribution = clamp(diffuse_contribution, 0.0, 1.0);

    vec4 tex_color = vec4(1.0, 1.0, 1.0, 0.0);
    if (has_texture == true){
        tex_color = texture2D(texture, fragment_uv);
    }
    vec4 diffuse_color = vec4(mix(tex_color.rgb, material_diffuse_color.rgb, 1.0 - tex_color.a), 1.0);

    float dist = distance(light_position, fragment_position);
    float a = 1.0;
    float b = 1.0;
    float att = 1.0 / (1.0 + a*dist + b*dist*dist);

    vec4 diffuse = vec4(att * diffuse_contribution* light_diffuse_color, 1.0) * diffuse_color;

    vec3 surface_to_view = normalize(fragment_position);
    vec3 reflection = reflect(normal, -surface_to_light);
    float secular_contribution = pow(max(0.0, dot(surface_to_view, reflection)), material_specular_exponent);
    vec4 specular = vec4(secular_contribution * light_specular_color * material_specular_color, 1.0);

    vec4 diffuse_static = static_light * diffuse_color;

    if (selected == true){
        diffuse.xyz = diffuse.xyz + vec3(0.2, 0.1, 0.1);
    }

    //gl_FragColor = vec4(diffuse.xyz + diffuse_static.xyz + specular.xyz, 1.0);
    color = vec4(diffuse.xyz + diffuse_static.xyz + specular.xyz, 1.0);
};
