#version 430 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 bright_color;
in vec2 frag_uv;

uniform sampler2DMS color_sampler;

void main() {
    vec2 texture_size = textureSize(color_sampler);
    ivec2 pixel_uv = ivec2(floor(texture_size * frag_uv));
    vec3 average_color = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < 4; ++i) {
        average_color += texelFetch(color_sampler, pixel_uv, i).rgb;
    }
    color = vec4(average_color / 4.0, 1.0);
    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    bright_color = vec4(clamp(color.rgb * float(brightness > 1.0), vec3(0.0), vec3(10.0)), 1.0);
}
