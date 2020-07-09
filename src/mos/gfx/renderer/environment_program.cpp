#include <mos/gfx/renderer.hpp>
#include <mos/util.hpp>

namespace mos::gfx {
Renderer::Environment_program::Environment_program() {
  std::string name = "environment";
  std::string vert_source = text("assets/shaders/" + name + ".vert");
  std::string frag_source = text("assets/shaders/" + name + ".frag");

  std::string functions_name = "functions";
  std::string functions_frag_source =
      text("assets/shaders/" + functions_name + ".frag");

  const auto vertex_shader = Shader(vert_source, GL_VERTEX_SHADER, name);
  const auto fragment_shader = Shader(frag_source, GL_FRAGMENT_SHADER, name);
  const auto functions_fragment_shader =
      Shader(functions_frag_source, GL_FRAGMENT_SHADER, functions_name);

  glAttachShader(program, vertex_shader.id);
  glAttachShader(program, functions_fragment_shader.id);
  glAttachShader(program, fragment_shader.id);

  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "normal");
  glBindAttribLocation(program, 2, "tangent");
  glBindAttribLocation(program, 3, "uv");

  link(name);
  check(name);

  glDetachShader(program, vertex_shader.id);
  glDetachShader(program, fragment_shader.id);
  glDetachShader(program, functions_fragment_shader.id);

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

  material.albedo_sampler =
      glGetUniformLocation(program, "material.albedo_sampler");
  material.emission_sampler =
      glGetUniformLocation(program, "material.emission_sampler");
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
  camera.far = glGetUniformLocation(program, "camera.far");
  camera.near = glGetUniformLocation(program, "camera.near");

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
    spot_lights.at(i).direction = glGetUniformLocation(
        program,
        std::string("spot_lights[" + std::to_string(i) + "].direction").c_str());

    shadow_samplers.at(i) = glGetUniformLocation(
        program, std::string("shadow_samplers[" + std::to_string(i) + "]").c_str());
  }

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
