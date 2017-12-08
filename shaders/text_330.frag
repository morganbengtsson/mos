#version 330
struct Material {
    vec3 albedo;
    float roughness;
    float opacity;
};

in vec3 v_position;
in vec2 v_uv;
uniform Material material;
uniform sampler2D texturemap;

out vec4 color;

void main() {
    color = texture(texturemap, v_uv) * vec4(material.albedo, 1.0);
    color = texture(texturemap, v_uv) * vec4(1.0, 0.0, 0.0, 1.0);
    color.w = color.w * material.opacity;
    //color = vec4(v_uv, 1.0, 1.0);
}
