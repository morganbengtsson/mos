#version 330
uniform sampler2D texture;
uniform float opacity;
uniform float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
uniform float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );
in vec3 v_position;
in vec2 v_uv;

void main() {
    float offset2 = 2.0;
    vec4 sum;
    vec4 color = texture2D(texture, v_uv);
    sum = color * weight[0];
    for (int i=1; i<3; i++) {
        sum += texture2D( texture, v_uv + vec2(0.0, offset[i] * offset2/ 800.0)) * weight[i];
        sum += texture2D( texture, v_uv - vec2(0.0, offset[i] * offset2/ 800.0)) * weight[i];
    }
    //gl_FragColor = sum * pow(length(sum.xyz), 2) + color;
    gl_FragColor = vec4(color.rgb, opacity);
    //gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
};

