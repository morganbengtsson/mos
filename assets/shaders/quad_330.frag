#version 330
layout(location = 0) out vec4 color;
in vec2 frag_uv;

uniform sampler2D quad_texture;

void main() {
    color = vec4(texture(quad_texture, frag_uv).rgb, 1.0);
}
