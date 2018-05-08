#version 330
layout(location = 0) out vec4 color;
in vec2 frag_uv;

uniform sampler2DMS quad_texture;

void main() {
    //color = vec4(texture(quad_texture, frag_uv).rgb, 1.0);
    vec2 texture_size = textureSize(quad_texture);
    vec2 tmp = floor(texture_size * frag_uv);
    vec3 average_color = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < 4; ++i) {
        average_color += texelFetch(quad_texture, ivec2(tmp), i).rgb;
    }
    color = vec4(average_color / 4.0, 1.0);
}
