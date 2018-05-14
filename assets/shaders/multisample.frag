#version 330
layout(location = 0) out vec4 color;
layout(location = 1) out vec4 bright_color;
in vec2 frag_uv;

uniform sampler2DMS color_texture;
uniform sampler2DMS depth_texture;

void main() {
    vec2 texture_size = textureSize(color_texture);
    ivec2 pixel_uv = ivec2(floor(texture_size * frag_uv));
    vec3 average_color = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < 4; ++i) {
        average_color += texelFetch(color_texture, pixel_uv, i).rgb;
    }
    color = vec4(average_color / 4.0, 1.0);
    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    bright_color = vec4(color.rgb * float(brightness > 1.0), 1.0);
}
