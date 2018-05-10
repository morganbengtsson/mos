#version 330
layout(location = 0) out vec4 color;
layout(location = 1) out vec4 bright_color;
in vec2 frag_uv;

uniform sampler2D color_texture;

void main() {
    color = vec4(texture(color_texture, frag_uv).rgb, 1.0);
    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        bright_color = vec4(color.rgb, 1.0);
    else
        bright_color = vec4(0.0, 0.0, 0.0, 1.0);
}
