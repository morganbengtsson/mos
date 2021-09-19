#include <mos/gl/renderer.hpp>
#include <mos/util.hpp>

namespace mos::gl {

Standard_program::Standard_program(const Shader &functions_shader) {
  std::string name = "standard";

  const auto vertex_shader = Shader(name, GL_VERTEX_SHADER);
  const auto fragment_shader = Shader(name, GL_FRAGMENT_SHADER);

  glAttachShader(program, vertex_shader.id);
  glAttachShader(program, functions_shader.id);
  glAttachShader(program, fragment_shader.id);

  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "normal");
  glBindAttribLocation(program, 2, "tangent");
  glBindAttribLocation(program, 3, "uv");

  link(name);
  check(name);

  glDetachShader(program, vertex_shader.id);
  glDetachShader(program, fragment_shader.id);
  glDetachShader(program, functions_shader.id);

  model_view_projection =
      (glGetUniformLocation(program, "model_view_projection"));
  model_matrix = glGetUniformLocation(program, "model");
  normal_matrix = glGetUniformLocation(program, "normal_matrix");
  for (size_t i = 0; i < depth_bias_mvps.size(); i++) {
    depth_bias_mvps.at(i) = glGetUniformLocation(
        program, std::string("depth_bias_model_view_projections[" +
                             std::to_string(i) + "]")
                     .c_str());
  }

  for (size_t i = 0; i < environment_maps.size(); i++) {
    environment_maps.at(i).sampler = glGetUniformLocation(
        program,
        std::string("environment_samplers[" + std::to_string(i) + "]").c_str());
    environment_maps.at(i).position = glGetUniformLocation(
        program, std::string("environments[" + std::to_string(i) + "].position")
                     .c_str());
    environment_maps.at(i).extent = glGetUniformLocation(
        program,
        std::string("environments[" + std::to_string(i) + "].extent").c_str());
    environment_maps.at(i).strength = glGetUniformLocation(
        program, std::string("environments[" + std::to_string(i) + "].strength")
                     .c_str());
    environment_maps.at(i).falloff = glGetUniformLocation(
        program,
        std::string("environments[" + std::to_string(i) + "].falloff").c_str());
  }

  material.albedo_sampler =
      glGetUniformLocation(program, "material.albedo_sampler");
  material.normal_sampler =
      glGetUniformLocation(program, "material.normal_sampler");
  material.metallic_sampler =
      glGetUniformLocation(program, "material.metallic_sampler");
  material.roughness_sampler =
      glGetUniformLocation(program, "material.roughness_sampler");
  material.emission_sampler =
      glGetUniformLocation(program, "material.emission_sampler");
  material.ambient_occlusion_sampler =
      glGetUniformLocation(program, "material.ambient_occlusion_sampler");
  material.albedo = glGetUniformLocation(program, "material.albedo");
  material.roughness = glGetUniformLocation(program, "material.roughness");
  material.metallic = glGetUniformLocation(program, "material.metallic");
  material.index_of_refraction =
      glGetUniformLocation(program, "material.index_of_refraction");
  material.alpha = glGetUniformLocation(program, "material.alpha");
  material.transmission =
      glGetUniformLocation(program, "material.transmission");
  material.emission = glGetUniformLocation(program, "material.emission");
  material.ambient_occlusion =
      glGetUniformLocation(program, "material.ambient_occlusion");

  camera.position = glGetUniformLocation(program, "camera.position");
  camera.resolution = glGetUniformLocation(program, "camera.resolution");

  for (size_t i = 0; i < spot_lights.size(); i++) {
    spot_lights.at(i).position = glGetUniformLocation(
        program,
        std::string("spot_lights[" + std::to_string(i) + "].position").c_str());
    spot_lights.at(i).color = glGetUniformLocation(
        program,
        std::string("spot_lights[" + std::to_string(i) + "].color").c_str());
    spot_lights.at(i).strength = glGetUniformLocation(
        program,
        std::string("spot_lights[" + std::to_string(i) + "].strength").c_str());
    spot_lights.at(i).view = glGetUniformLocation(
        program, std::string("spot_lights[" + std::to_string(i) + "].view").c_str());
    spot_lights.at(i).projection = glGetUniformLocation(
        program,
        std::string("spot_lights[" + std::to_string(i) + "].projection").c_str());
    spot_lights.at(i).angle = glGetUniformLocation(
        program,
        std::string("spot_lights[" + std::to_string(i) + "].angle").c_str());
    spot_lights.at(i).blend = glGetUniformLocation(
        program,
        std::string("spot_lights[" + std::to_string(i) + "].blend").c_str());
    spot_lights.at(i).direction = glGetUniformLocation(
        program,
        std::string("spot_lights[" + std::to_string(i) + "].direction").c_str());

    shadow_samplers.at(i) = glGetUniformLocation(
        program, std::string("shadow_samplers[" + std::to_string(i) + "]").c_str());
  }

  //Cascaded
  for (int i = 0; i < 4; i++){
    cascaded_shadow_samplers.at(i) = glGetUniformLocation(
        program, std::string("cascaded_shadow_samplers[" + std::to_string(i) + "]").c_str());
    cascaded_depth_bias_mvps.at(i) = glGetUniformLocation(
        program, std::string("cascaded_depth_bias_model_view_projections[" +
                             std::to_string(i) + "]")
                     .c_str());
  }
  cascade_splits = glGetUniformLocation(
      program,
      std::string("cascade_splits").c_str());

  //Directional light
  directional_light.position = glGetUniformLocation(
      program,
      std::string("directional_light.position").c_str());
  directional_light.direction = glGetUniformLocation(
      program,
      std::string("directional_light.direction").c_str());
  directional_light.color = glGetUniformLocation(
      program,
      std::string("directional_light.color").c_str());
  directional_light.strength = glGetUniformLocation(
      program, std::string("directional_light.strength").c_str());


  fog.color_near = glGetUniformLocation(program, "fog.color_near");
  fog.color_far = glGetUniformLocation(program, "fog.color_far");
  fog.attenuation_factor =
      glGetUniformLocation(program, "fog.attenuation_factor");
  fog.min = glGetUniformLocation(program, "fog.min");
  fog.max = glGetUniformLocation(program, "fog.max");
  brdf_lut_sampler = glGetUniformLocation(program, "brdf_lut_sampler");
}
} // namespace mos::gfx
