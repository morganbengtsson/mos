#version 330 core
in vec3 fragment_position;
in vec4 fragment_color;
layout(location = 0) out vec4 color;

uniform sampler2D tex;

void main() {
    color = vec4(fragment_color);
    color = vec4(gl_PointCoord, 1.0, 1.0);
    color = texture(tex, gl_PointCoord);
}
