#version 330
layout(location = 0) out float depth;
//layout(location = 0) out vec4 color;

void main() {
    depth = pow(gl_FragCoord.z, 100.0f);
    //float d = pow(gl_FragCoord.z, 100.0f);
    //color = vec4(d,d,d, 1.0);
}
