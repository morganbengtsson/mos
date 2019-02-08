#version 430 core

struct Light {
    vec3 position;
    vec3 color;
    float strength;
    mat4 view;
    mat4 projection;
    float angle;
    vec3 direction;
};

in vec3 fragment_position;
in vec4 fragment_color;
in float fragment_opacity;
layout(location = 0) out vec4 color;

uniform sampler2D tex;
uniform Light[4] lights;

void main() {
    vec3 Lo = vec3(0.0, 0.0, 0.0);
    vec4 tex_color = texture(tex, gl_PointCoord);
    color = vec4(tex_color.r, tex_color.g, tex_color.b, tex_color.a * fragment_color.a * fragment_opacity);
    color.rgb = lights[0].color;
}
