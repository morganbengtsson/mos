#version 430 core

layout(location = 0) out vec4 color;
in vec2 frag_uv;

uniform sampler2DMS ambient_sampler;
uniform sampler2DMS depth_sampler;
uniform sampler2DMS normals_sampler;

float rand(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 normal_from_depth(vec2 texcoords) {

  vec2 texture_size = textureSize(depth_sampler);
  ivec2 pixel_uv = ivec2(floor(texture_size * texcoords));

  float depth1 = texelFetch(depth_sampler, pixel_uv + ivec2(0, 1), 0).r;
  float depth2 = texelFetch(depth_sampler, pixel_uv + ivec2(1, 0), 1).r;

  const float depth = texelFetch(depth_sampler, pixel_uv, 0).r;
  vec3 p1 = vec3(0, 0.001, depth1 - depth);
  vec3 p2 = vec3(0.001, 0, depth2 - depth);

  vec3 normal = cross(p1, p2);
  normal.z = -normal.z;

  return normalize(normal);
}

void main() {
    color = vec4(1.0, 0.0, 0.0, 1.0);
    vec3 normal = normal_from_depth(frag_uv);

    const float total_strength = 1.0;
    const float base = 0.2;

    const float area = 0.075;
    const float falloff = 0.000001;

    //const float radius = 0.0002;
    const float radius = 0.1;

    const int samples = 16;

    vec3 sample_sphere[samples] = {
         vec3( 0.5381, 0.1856,-0.4319), vec3( 0.1379, 0.2486, 0.4430),
         vec3( 0.3371, 0.5679,-0.0057), vec3(-0.6999,-0.0451,-0.0019),
         vec3( 0.0689,-0.1598,-0.8547), vec3( 0.0560, 0.0069,-0.1843),
         vec3(-0.0146, 0.1402, 0.0762), vec3( 0.0100,-0.1924,-0.0344),
         vec3(-0.3577,-0.5301,-0.4358), vec3(-0.3169, 0.1063, 0.0158),
         vec3( 0.0103,-0.5869, 0.0046), vec3(-0.0897,-0.4940, 0.3287),
         vec3( 0.7119,-0.0154,-0.0918), vec3(-0.0533, 0.0596,-0.5411),
         vec3( 0.0352,-0.0631, 0.5460), vec3(-0.4776, 0.2847,-0.0271)
     };

     //float3 random = normalize( tex2D(RandomTextureSampler, In.Tex0 * 4.0).rgb );

     vec3 random = vec3(rand(frag_uv), rand(frag_uv + 1.0), rand(frag_uv * 2));
     random = normalize(random);

     vec2 texture_size = textureSize(depth_sampler);
     ivec2 pixel_uv = ivec2(floor(texture_size * frag_uv));
     float depth = texelFetch(depth_sampler, pixel_uv, 0).r;

     vec3 position = vec3(frag_uv, depth);

     float radius_depth = radius/depth;
     float occlusion = 0.0;
     for(int i=0; i < samples; i++) {

       vec3 ray = radius_depth * reflect(sample_sphere[i], random);
       vec3 hemi_ray = position + sign(dot(ray,normal)) * ray;

       vec2 t  = clamp(hemi_ray.xy, vec2(0.0,0.0), vec2(1.0, 1.0));
       ivec2 p_uv = ivec2(floor(texture_size * t));

       float occ_depth = texelFetch(depth_sampler, p_uv , 0).r;
       float difference = depth - occ_depth;

       occlusion += step(falloff, difference) * (1.0-smoothstep(falloff, area, difference));
     }

     float ao = 1.0 - total_strength * occlusion * (1.0 / samples);
     color.rgb = texelFetch(ambient_sampler, pixel_uv, 0).rgb * ao;
}
