#include <mos/gfx/renderer.hpp>
#include <mos/util.hpp>

namespace mos::gfx {

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
  texture = glGetUniformLocation(program, "texture_sampler");
  emissive = glGetUniformLocation(program, "emissive");
  resolution = glGetUniformLocation(program, "resolution");
  camera_position = glGetUniformLocation(program, "camera.position");
  camera_resolution = glGetUniformLocation(program, "camera.resolution");

  for (size_t i = 0; i < lights.size(); i++) {
    lights.at(i).position = glGetUniformLocation(
        program,
        std::string("lights[" + std::to_string(i) + "].position").c_str());
    lights.at(i).color = glGetUniformLocation(
        program,
        std::string("lights[" + std::to_string(i) + "].color").c_str());
    lights.at(i).strength = glGetUniformLocation(
        program,
        std::string("lights[" + std::to_string(i) + "].strength").c_str());
    lights.at(i).view = glGetUniformLocation(
        program, std::string("lights[" + std::to_string(i) + "].view").c_str());
    lights.at(i).projection = glGetUniformLocation(
        program,
        std::string("lights[" + std::to_string(i) + "].projection").c_str());
    lights.at(i).angle = glGetUniformLocation(
        program,
        std::string("lights[" + std::to_string(i) + "].angle").c_str());
    lights.at(i).direction = glGetUniformLocation(
        program,
        std::string("lights[" + std::to_string(i) + "].direction").c_str());
  }
  for (size_t i = 0; i < environment_maps.size(); i++) {
    environment_maps.at(i).map = glGetUniformLocation(
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
}
} // namespace mos::gfx
