#version 330
in vec3 v_position;
in vec2 v_uv;
uniform sampler2D texture;
uniform float opacity;

out vec4 color;

void main() {
    color = texture2D(texture, v_uv);
    color.w = color.w * opacity;
    //color = vec4(v_uv, 1.0, 1.0);
};
