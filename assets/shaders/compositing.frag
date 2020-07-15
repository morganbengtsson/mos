#version 430 core

layout(location = 0) out vec4 out_color;
in vec2 frag_uv;

uniform sampler2D color_sampler;
uniform sampler2D bloom_sampler;
uniform float strength;

float rand(vec2 co) {
  return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 uncharted2_tonemap(vec3 x) {
  float A = 0.15;
  float B = 0.50;
  float C = 0.10;
  float D = 0.20;
  float E = 0.02;
  float F = 0.30;
  return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

vec3 uncharted2(vec3 color) {
  const float W = 11.2;
  float exposure_bias = 2.0;
  vec3 curr = uncharted2_tonemap(exposure_bias * color);
  vec3 white_scale = 1.0 / uncharted2_tonemap(vec3(W));
  return curr * white_scale;
}

const float rcount = 50.0;
const float gcount = 50.0;
const float bcount = 50.0;
const float acount = 1.0;

int bayer[8 * 8] = int[](
    0,32, 8,40, 2,34,10,42,
     48,16,56,24,50,18,58,26,
     12,44, 4,36,14,46, 6,38,
     60,28,52,20,62,30,54,22,
     3,35,11,43, 1,33, 9,41,
     51,19,59,27,49,17,57,25,
     15,47, 7,39,13,45, 5,37,
     63,31,55,23,61,29,53,21
     );
float bayerSize = 8.0;

/*
int bayer[12 * 12] = int[](
0  ,96 ,64 ,8  ,104,72 ,2  ,98 ,66 ,10 ,106,74 , // 12x12 bayer ordered dithering
112,80 ,16 ,120,88 ,24 ,114,82 ,18 ,122,90 ,26 , // pattern. Each input pixel
48 ,32 ,128,56 ,40 ,136,50 ,34 ,130,58 ,42 ,138, // is scaled to the 0..143 range
12 ,108,76 ,4  ,100,68 ,14 ,110,78 ,6  ,102,70 , // before looking in this table
124,92 ,28 ,116,84 ,20 ,126,94 ,30 ,118,86 ,22 , // to determine the action
60 ,44 ,140,52 ,36 ,132,62 ,46 ,142,54 ,38 ,134,
3  ,99 ,67 ,11 ,107,75 ,1  ,97 ,65 ,9  ,105,73 ,
115,83 ,19 ,123,91 ,27 ,113,81 ,17 ,121,89 ,25 ,
51 ,35 ,131,59 ,43 ,139,49 ,33 ,129,57 ,41 ,137,
15 ,111,79 ,7  ,103,71 ,13 ,109,77 ,5  ,101,69 ,
127,95 ,31 ,119,87 ,23 ,125,93 ,29 ,117,85 ,21 ,
63 ,47 ,143,55 ,39 ,135,61 ,45 ,141,53 ,37 ,133
);
float bayerSize = 12.0;
*/

/*int bayer[7 * 7] = int[](
32,42,10,27,37,5,15,
1,18,28,45,13,23,40,
26,36,4,14,31,48,9,
44,12,22,39,0,17,34,
20,30,47,8,25,35,3,
38,6,16,33,43,11,21,
7,24,41,2,19,29,46
);

float bayerSize = 7.0;*/


float bayerDivider = bayerSize * bayerSize;

vec4 nearestColour(vec4 incolor) {
  vec4 rgbaCounts = vec4(rcount, gcount, bcount, acount);


  vec4 color = incolor;

  color.r = floor((rgbaCounts.r - 1.0) * color.r + 0.5) / (rgbaCounts.r - 1.0);
  color.g = floor((rgbaCounts.g - 1.0) * color.g + 0.5) / (rgbaCounts.g - 1.0);
  color.b = floor((rgbaCounts.b - 1.0) * color.b + 0.5) / (rgbaCounts.b - 1.0);

  if (rgbaCounts.a >= 2.0) {
    color.a = floor((rgbaCounts.a - 1.0) * color.a + 0.5) / (rgbaCounts.a - 1.0);
  }
  else {
    color.a = 1.0;
  }

  return color;
}

void main() {
  const vec3 bloom = texture(bloom_sampler, frag_uv).rgb;
  vec3 color = texture(color_sampler, frag_uv).rgb + bloom * strength;

  vec2 uv = frag_uv;
  uv *=  1.0 - uv.yx;
  float vig = uv.x*uv.y * 15.0;
  vig = pow(vig, 0.25);

  float offset = 0.004 * (1.0 - vig);
  color.r = texture(color_sampler, frag_uv + vec2(offset, offset)).r;
  color.g = texture(color_sampler, frag_uv + vec2(-offset, -offset)).g;
  color.b = texture(color_sampler, frag_uv + vec2(offset, -offset)).b;

  float r = rand(frag_uv);
  color.rgb *= (1.0 - r * 0.15);

  float spread = 1.0 / (0.299 * (rcount - 1.0) + 0.587 * (gcount - 1.0) + 0.114 * (bcount - 1.0));  // this spread value is optimised one -- try your own values for various effects!

  vec2 entry = mod(gl_FragCoord.xy, vec2(bayerSize, bayerSize));

  color.rgb = nearestColour(vec4(color.rgb + spread * (bayer[int(entry.y) * int(bayerSize) + int(entry.x)] / bayerDivider - 0.5), 1.0)).xyz;

  float exposure = 0.0;
  out_color = vec4(uncharted2(color * pow(2.0, exposure)), 1.0);
  out_color.rgb *= vig;


}
