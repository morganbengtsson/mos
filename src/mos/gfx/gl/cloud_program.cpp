#include <mos/gfx/gl/renderer.hpp>
#include <mos/util.hpp>

namespace mos::gfx::gl {

Renderer::Cloud_program::Cloud_program(const std::string &name,
                                       const Shader &functions_shader) {
  std::string vert_source = text("assets/shaders/" + name + ".vert");
  std::string frag_source = text("assets/shaders/" + name + ".frag");
  const auto vertex_shader = Shader(vert_source, GL_VERTEX_SHADER, name);
  const auto fragment_shader = Shader(frag_source, GL_FRAGMENT_SHADER, name);

  glAttachShader(program, vertex_shader.id);
  glAttachShader(program, fragment_shader.id);
  glAttachShader(program, functions_shader.id);
  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "color");
  glBindAttribLocation(program, 2, "size");
  glBindAttribLocation(program, 3, "alpha");

  link(name);
  check(name);

  glDetachShader(program, vertex_shader.id);
  glDetachShader(program, fragment_shader.id);
  glDetachShader(program, functions_shader.id);

  model_view_projection =
      glGetUniformLocation(program, "model_view_projection");
  model_view = glGetUniformLocation(program, "model_view");
  projection = glGetUniformLocation(program, "projection");
  albedo_sampler = glGetUniformLocation(program, "texture_sampler");
  emissive_sampler = glGetUniformLocation(program, "emissive");
  resolution = glGetUniformLocation(program, "resolution");

  camera.position = glGetUniformLocation(program, "camera.position");

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
  }
  for (size_t i = 0; i < environment_samplers.size(); i++) {
    environment_samplers.at(i).map = glGetUniformLocation(
        program,
        std::string("environment_samplers[" + std::to_string(i) + "]").c_str());
    environment_samplers.at(i).position = glGetUniformLocation(
        program, std::string("environments[" + std::to_string(i) + "].position")
                     .c_str());
    environment_samplers.at(i).extent = glGetUniformLocation(
        program,
        std::string("environments[" + std::to_string(i) + "].extent").c_str());
    environment_samplers.at(i).strength = glGetUniformLocation(
        program, std::string("environments[" + std::to_string(i) + "].strength")
                     .c_str());
    environment_samplers.at(i).falloff = glGetUniformLocation(
        program,
        std::string("environments[" + std::to_string(i) + "].falloff").c_str());
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

}
} // namespace mos::gfx
