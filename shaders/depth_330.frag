#version 330
layout(location = 0) out float depth;

void main() {
    depth = gl_FragCoord.z;
}
