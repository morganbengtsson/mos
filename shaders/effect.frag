#ifdef GL_ES
precision mediump float;
precision mediump int;
#else
#version 120
#endif
varying vec3 v_position;
varying vec2 v_uv;
uniform sampler2D texture;
uniform float opacity;

const float blurSize = 1.0/200.0;
const float intensity = 0.35;

const float R_fxaaReduceMin  =  (1.0/ 128.0);
const float  R_fxaaReduceMul =   (1.0 / 8.0);
const float R_fxaaSpanMax  =   16.0;

uniform float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
uniform float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );

void main() {
    float offset2 = 2.0;
    vec4 sum;
    vec4 color = texture2D(texture, v_uv);
    sum = color * weight[0];
    for (int i=1; i<3; i++) {
        sum += texture2D( texture, v_uv + vec2(0.0, offset[i] * offset2/ 800.0)) * weight[i];
        sum += texture2D( texture, v_uv - vec2(0.0, offset[i] * offset2/ 800.0)) * weight[i];
    }
    gl_FragColor = sum * pow(length(sum.xyz), 2) + color;

};

