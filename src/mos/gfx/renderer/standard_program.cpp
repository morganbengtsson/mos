#include <mos/gfx/renderer.hpp>
#include <mos/util.hpp>

namespace mos::gfx {

Renderer::Standard_program::Standard_program(const Shader &functions_shader) {
  std::string name = "standard";
  std::string vert_source = text("assets/shaders/" + name + ".vert");
  std::string frag_source = text("assets/shaders/" + name + ".frag");

  const auto vertex_shader = Shader(vert_source, GL_VERTEX_SHADER, name);
  const auto fragment_shader = Shader(frag_source, GL_FRAGMENT_SHADER, name);

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

  material_albedo_sampler =
      glGetUniformLocation(program, "material.albedo_sampler");
  material_normal_sampler =
      glGetUniformLocation(program, "material.normal_sampler");
  material_metallic_sampler =
      glGetUniformLocation(program, "material.metallic_sampler");
  material_roughness_sampler =
      glGetUniformLocation(program, "material.roughness_sampler");
  material_emission_sampler =
      glGetUniformLocation(program, "material.emission_sampler");
  material_ambient_occlusion_sampler =
      glGetUniformLocation(program, "material.ambient_occlusion_sampler");
  material_albedo = glGetUniformLocation(program, "material.albedo");
  material_roughness = glGetUniformLocation(program, "material.roughness");
  material_metallic = glGetUniformLocation(program, "material.metallic");
  material_index_of_refraction =
      glGetUniformLocation(program, "material.index_of_refraction");
  material_alpha = glGetUniformLocation(program, "material.alpha");
  material_transmission =
      glGetUniformLocation(program, "material.transmission");
  material_emission = glGetUniformLocation(program, "material.emission");
  material_ambient_occlusion =
      glGetUniformLocation(program, "material.ambient_occlusion");

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
    lights.at(i).blend = glGetUniformLocation(
        program,
        std::string("lights[" + std::to_string(i) + "].blend").c_str());
    lights.at(i).direction = glGetUniformLocation(
        program,
        std::string("lights[" + std::to_string(i) + "].direction").c_str());

    shadow_maps.at(i) = glGetUniformLocation(
        program, std::string("shadow_maps[" + std::to_string(i) + "]").c_str());
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


  fog_color_near = glGetUniformLocation(program, "fog.color_near");
  fog_color_far = glGetUniformLocation(program, "fog.color_far");
  fog_attenuation_factor =
      glGetUniformLocation(program, "fog.attenuation_factor");
  fog_min = glGetUniformLocation(program, "fog.min");
  fog_max = glGetUniformLocation(program, "fog.max");
  brdf_lut = glGetUniformLocation(program, "brdf_lut");
}
} // namespace mos::gfx
