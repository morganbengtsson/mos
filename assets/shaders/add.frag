#version 430 core

layout(location = 0) out vec4 out_color;
in vec2 frag_uv;
uniform sampler2D color_sampler;
uniform sampler2D blurred_color_sampler;
uniform sampler2DMS depth_sampler;

void main() {
    out_color = texture(color_sampler, frag_uv);
    out_color = texture(blurred_color_sampler, frag_uv);
}
