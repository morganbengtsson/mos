#version 430 core

layout(location = 0) out vec4 out_color;
in vec2 frag_uv;
uniform sampler2D color_sampler;

void main() {
    vec3 color = texture(color_sampler, frag_uv).rgb;
    float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    out_color = vec4(clamp(color.rgb * float(brightness > 1.0), vec3(0.0), vec3(10.0)), 1.0);
}
