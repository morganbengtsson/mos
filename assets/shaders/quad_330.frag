#version 330
layout(location = 0) out vec4 color;
layout(location = 1) out vec4 bright_color;
in vec2 frag_uv;

uniform sampler2DMS quad_texture;
uniform sampler2DMS quad_depth_texture;

void main() {

    vec2 texture_size = textureSize(quad_texture);
    vec2 tmp = floor(texture_size * frag_uv);
    vec3 average_color = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < 4; ++i) {
        average_color += texelFetch(quad_texture, ivec2(tmp), i).rgb;
    }
    color = vec4(average_color / 4.0, 1.0);
    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
        if(brightness > 1.0)
            bright_color = vec4(color.rgb, 1.0);
        else
            bright_color = vec4(0.0, 0.0, 0.0, 1.0);

}
