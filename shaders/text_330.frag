#version 330
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float specular_exponent;
    float opacity;
};

in vec3 v_position;
in vec2 v_uv;
uniform Material material;
uniform sampler2D texture;

out vec4 color;

void main() {
    color = texture2D(texture, v_uv) * vec4(material.diffuse, 1.0);
    color.w = color.w * material.opacity;
    //color = vec4(v_uv, 1.0, 1.0);
}
