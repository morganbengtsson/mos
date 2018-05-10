#version 330 core
in vec3 fragment_position;
in vec4 fragment_color;
in float fragment_opacity;
layout(location = 0) out vec4 color;

uniform sampler2D tex;

void main() {
    color = vec4(fragment_color);
    color = vec4(gl_PointCoord, 1.0, 1.0);
    vec4 tex_color = texture(tex, gl_PointCoord);
    color = vec4(tex_color.r, tex_color.g, tex_color.b, tex_color.a * fragment_color.a * fragment_opacity);
    color.rgb = vec3(fragment_opacity, fragment_opacity, fragment_opacity);
}
