#version 330
layout(location = 0) out vec4 color;
in vec2 frag_uv;

uniform sampler2D color_texture;
uniform sampler2D bright_color_texture;

void main() {
    vec3 bloom = texture(bright_color_texture, frag_uv).rgb;
    color = vec4(texture(color_texture, frag_uv).rgb + bloom, 1.0);
}
