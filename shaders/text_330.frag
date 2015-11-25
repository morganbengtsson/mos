#version 330
in vec3 v_position;
in vec2 v_uv;
uniform vec3 material_ambient_color;
uniform vec3 material_diffuse_color;
uniform vec3 material_specular_color;
uniform sampler2D texture;
uniform float opacity;

out vec4 color;

void main() {
    color = texture2D(texture, v_uv) * vec4(material_diffuse_color, 1.0);
    color.w = color.w * opacity;
    //color = vec4(v_uv, 1.0, 1.0);
};
