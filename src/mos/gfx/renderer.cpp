#include <cstdlib>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/color_space.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <mos/gfx/box.hpp>
#include <mos/gfx/camera.hpp>
#include <mos/gfx/cloud.hpp>
#include <mos/gfx/environment_light.hpp>
#include <mos/gfx/fog.hpp>
#include <mos/gfx/mesh.hpp>
#include <mos/gfx/model.hpp>
#include <mos/gfx/models.hpp>
#include <mos/gfx/renderer.hpp>
#include <mos/gfx/scene.hpp>
#include <mos/gfx/scenes.hpp>
#include <mos/gfx/spot_light.hpp>
#include <mos/gfx/spot_lights.hpp>
#include <mos/gfx/target.hpp>
#include <mos/gfx/text.hpp>
#include <mos/gfx/texture_2d.hpp>
#include <mos/util.hpp>

namespace mos::gfx {

auto Renderer::generate(const std::function<void(GLsizei, GLuint *)> &f)
    -> GLuint {
  GLuint id;
  f(1, &id);
  return id;
}

auto Renderer::wrap_convert(const Texture::Wrap &w) -> GLuint {
  static const std::map<Texture::Wrap, GLint> wrap_map{
      {Texture::Wrap::Clamp, GL_CLAMP_TO_EDGE},
      {Texture::Wrap::Repeat, GL_REPEAT}};
  return wrap_map.at(w);
}

auto Renderer::filter_convert(const Texture::Filter &f) -> GLuint {
  static const std::map<Texture::Filter, GLint> filter_map{
      {Texture::Filter::Linear, GL_LINEAR},
      {Texture::Filter::Closest, GL_NEAREST}};
  return filter_map.at(f);
}

auto Renderer::filter_convert_mip(const Texture::Filter &f) -> GLuint {
  static const std::map<Texture::Filter, GLint> filter_map_mip{
      {Texture::Filter::Linear, GL_LINEAR_MIPMAP_LINEAR},
      {Texture::Filter::Closest, GL_NEAREST_MIPMAP_LINEAR}};
  return filter_map_mip.at(f);
}

auto Renderer::format_convert(const Texture::Format &f) -> FormatPair {
  static const std::map<Texture::Format, FormatPair> format_map{
      {Texture::Format::R, {GL_RED, GL_RED}},
      {Texture::Format::RG, {GL_RG, GL_RG}},
      {Texture::Format::SRGB, {GL_SRGB, GL_RGB}},
      {Texture::Format::SRGBA, {GL_SRGB_ALPHA, GL_RGBA}},
      {Texture::Format::RGB, {GL_RGB, GL_RGB}},
      {Texture::Format::RGBA, {GL_RGBA, GL_RGBA}}};
  return format_map.at(f);
}

void APIENTRY message_callback(GLenum source, GLenum type, GLuint id,
                               GLenum severity, GLsizei length,
                               const GLchar *message, const void *userParam) {
  if (severity == GL_DEBUG_SEVERITY_HIGH ||
      severity == GL_DEBUG_SEVERITY_MEDIUM) {
    fprintf(stderr,
            "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type,
            severity, message);
  }
}

Renderer::Renderer(const glm::ivec2 &resolution, const int samples)
    : context_(gladLoadGL()),
      functions_shader_(text("assets/shaders/functions.frag"),
                        GL_FRAGMENT_SHADER, "functions"),
      standard_program_(functions_shader_),
      point_cloud_program_("points", functions_shader_),
      line_cloud_program_("lines", functions_shader_),
      standard_target_(resolution, samples),
      multisample_target_(resolution, GL_RGBA16F),
      post_target0_(resolution / 4, GL_R11F_G11F_B10F),
      post_target1_(resolution / 4, GL_R11F_G11F_B10F), quad_(),
      black_texture_(Texture_buffer_2D(
          GL_RGBA, GL_RGBA, 1, 1, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT,
          std::array<unsigned char, 4>{0, 0, 0, 0}.data())),
      white_texture_(Texture_buffer_2D(
          GL_RGBA, GL_RGBA, 1, 1, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT,
          std::array<unsigned char, 4>{255, 255, 255, 255}.data())),
      brdf_lut_texture_(Texture_buffer_2D(
          Texture_2D("assets/brdfLUT.png", false, false,
                     Texture_2D::Filter::Linear, Texture_2D::Wrap::Clamp))),
      cube_camera_index_({0, 0}), shadow_maps_render_buffer_(resolution.y),
      shadow_maps_{Shadow_map_target(shadow_maps_render_buffer_),
                   Shadow_map_target(shadow_maps_render_buffer_),
                   Shadow_map_target(shadow_maps_render_buffer_),
                   Shadow_map_target(shadow_maps_render_buffer_)},
      cascaded_shadow_maps_{Shadow_map_target(shadow_maps_render_buffer_),
                            Shadow_map_target(shadow_maps_render_buffer_),
                            Shadow_map_target(shadow_maps_render_buffer_),
                            Shadow_map_target(shadow_maps_render_buffer_)},
      shadow_map_blur_target_(
          Post_target(shadow_maps_render_buffer_.resolution(), GL_RG32F)),
      shadow_map_blur_targets_{
          Post_target(shadow_maps_render_buffer_.resolution(), GL_RG32F),
          Post_target(shadow_maps_render_buffer_.resolution(), GL_RG32F),
          Post_target(shadow_maps_render_buffer_.resolution(), GL_RG32F),
          Post_target(shadow_maps_render_buffer_.resolution(), GL_RG32F)},
      cascaded_shadow_map_blur_targets_{
          Post_target(shadow_maps_render_buffer_.resolution(), GL_RG32F),
          Post_target(shadow_maps_render_buffer_.resolution(), GL_RG32F),
          Post_target(shadow_maps_render_buffer_.resolution(), GL_RG32F),
          Post_target(shadow_maps_render_buffer_.resolution(), GL_RG32F)},
      environment_render_buffer_(128),
      environment_maps_targets_{
          Environment_map_target(environment_render_buffer_),
          Environment_map_target(environment_render_buffer_)},
      propagate_target_(environment_render_buffer_) {

  if (!context_) {
    printf("No valid OpenGL context.\n");
    exit(-1);
  }

  fprintf(stdout, "Status: OpenGL version: %s\n", glGetString(GL_VERSION));

#ifndef NDEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(message_callback, nullptr);
#endif

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
  glEnable(GL_FRAMEBUFFER_SRGB);
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  glDepthFunc(GL_LEQUAL);
  glDepthMask(GL_TRUE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::load(const Model &model) {
  load(model.mesh);
  load(model.material.albedo.texture);
  load(model.material.normal.texture);
  load(model.material.metallic.texture);
  load(model.material.emission.texture);
  load(model.material.roughness.texture);
  load(model.material.ambient_occlusion.texture);
  for (auto &m : model.models) {
    load(m);
  }
}

void Renderer::unload(const Model &model) {
  unload(model.mesh);
  unload(model.material.albedo.texture);
  unload(model.material.normal.texture);
  unload(model.material.emission.texture);
  unload(model.material.metallic.texture);
  unload(model.material.roughness.texture);
  unload(model.material.ambient_occlusion.texture);
  for (auto &m : model.models) {
    unload(m);
  }
}

void Renderer::load_or_update(const Texture_2D &texture) {
  if (textures_.find(texture.id()) == textures_.end()) {
    textures_.insert({texture.id(), Texture_buffer_2D(texture)});
  } else {
    auto &buffer = textures_.at(texture.id());
    if (texture.layers.modified() > buffer.modified) {
      glBindTexture(GL_TEXTURE_2D, buffer.texture);
      glTexImage2D(GL_TEXTURE_2D, 0,
                   format_convert(texture.format).internal_format,
                   texture.width(), texture.height(), 0,
                   format_convert(texture.format).format, GL_UNSIGNED_BYTE,
                   texture.layers[0].data());
      if (texture.mipmaps) {
        glGenerateMipmap(GL_TEXTURE_2D);
      }
      glBindTexture(GL_TEXTURE_2D, 0);
      buffer.modified = texture.layers.modified();
    }
  }
}

void Renderer::load(const Shared_texture_2D &texture) {
  if (texture) {
    load_or_update(*texture);
  }
}

void Renderer::unload(const Shared_texture_2D &texture) {
  if (texture) {
    if (textures_.find(texture->id()) != textures_.end()) {
      textures_.erase(texture->id());
    }
  }
}

void Renderer::clear_buffers() {
  textures_.clear();
  array_buffers_.clear();
  element_array_buffers_.clear();
}

void Renderer::render_scene(const Camera &camera, const Scene &scene,
                            const glm::ivec2 &resolution) {
  glViewport(0, 0, resolution.x, resolution.y);
  glUseProgram(standard_program_.program);
  glUniform1i(standard_program_.brdf_lut_sampler, 0);

  glUniform1i(standard_program_.shadow_samplers[0], 1);
  glUniform1i(standard_program_.shadow_samplers[1], 2);
  glUniform1i(standard_program_.shadow_samplers[2], 3);
  glUniform1i(standard_program_.shadow_samplers[3], 4);

  glUniform1i(standard_program_.environment_maps[0].sampler, 5);
  glUniform1i(standard_program_.environment_maps[1].sampler, 6);

  glUniform1i(standard_program_.material.albedo_sampler, 7);
  glUniform1i(standard_program_.material.emission_sampler, 8);
  glUniform1i(standard_program_.material.normal_sampler, 9);
  glUniform1i(standard_program_.material.metallic_sampler, 10);
  glUniform1i(standard_program_.material.roughness_sampler, 11);
  glUniform1i(standard_program_.material.ambient_occlusion_sampler, 12);

  glUniform1i(standard_program_.cascaded_shadow_samplers[0], 13);
  glUniform1i(standard_program_.cascaded_shadow_samplers[1], 14);
  glUniform1i(standard_program_.cascaded_shadow_samplers[2], 15);
  glUniform1i(standard_program_.cascaded_shadow_samplers[3], 16);

  // Cascaded

  glActiveTexture(GL_TEXTURE13);
  // glBindTexture(GL_TEXTURE_2D, cascaded_shadow_maps_[0].texture);
  glBindTexture(GL_TEXTURE_2D, cascaded_shadow_map_blur_targets_[0].texture);

  glActiveTexture(GL_TEXTURE14);
  // glBindTexture(GL_TEXTURE_2D, cascaded_shadow_maps_[1].texture);
  glBindTexture(GL_TEXTURE_2D, cascaded_shadow_map_blur_targets_[1].texture);

  glActiveTexture(GL_TEXTURE15);
  // glBindTexture(GL_TEXTURE_2D, cascaded_shadow_maps_[2].texture);
  glBindTexture(GL_TEXTURE_2D, cascaded_shadow_map_blur_targets_[2].texture);

  glActiveTexture(GL_TEXTURE16);
  // glBindTexture(GL_TEXTURE_2D, cascaded_shadow_maps_[3].texture);
  glBindTexture(GL_TEXTURE_2D, cascaded_shadow_map_blur_targets_[3].texture);

  // Cascaded Splits
  glUniform4fv(standard_program_.cascade_splits, 1,
               glm::value_ptr(cascade_splits));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, brdf_lut_texture_.texture);

  glActiveTexture(GL_TEXTURE1);
  // glBindTexture(GL_TEXTURE_2D, shadow_maps_[0].texture);
  glBindTexture(GL_TEXTURE_2D, shadow_map_blur_targets_[0].texture);

  glActiveTexture(GL_TEXTURE2);
  // glBindTexture(GL_TEXTURE_2D, shadow_maps_[1].texture);
  glBindTexture(GL_TEXTURE_2D, shadow_map_blur_targets_[1].texture);

  glActiveTexture(GL_TEXTURE3);
  // glBindTexture(GL_TEXTURE_2D, shadow_maps_[0].texture);
  glBindTexture(GL_TEXTURE_2D, shadow_map_blur_targets_[2].texture);

  glActiveTexture(GL_TEXTURE4);
  // glBindTexture(GL_TEXTURE_2D, shadow_maps_[1].texture);
  glBindTexture(GL_TEXTURE_2D, shadow_map_blur_targets_[3].texture);

  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_CUBE_MAP, propagate_target_.texture);

  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_CUBE_MAP, environment_maps_targets_[1].texture);

  for (size_t i = 0; i < scene.environment_lights.size(); i++) {
    auto position = scene.environment_lights.at(i).position();
    glUniform3fv(standard_program_.environment_maps.at(i).position, 1,
                 glm::value_ptr(position));
    auto extent = scene.environment_lights.at(i).extent();
    glUniform3fv(standard_program_.environment_maps.at(i).extent, 1,
                 glm::value_ptr(extent));
    glUniform1fv(standard_program_.environment_maps.at(i).strength, 1,
                 &scene.environment_lights.at(i).strength);
    glUniform1fv(standard_program_.environment_maps.at(i).falloff, 1,
                 &scene.environment_lights.at(i).falloff);
  }

  // Camera in world space
  auto position = camera.position();
  glUniform3fv(standard_program_.camera.position, 1, glm::value_ptr(position));

  // Spotlights
  for (size_t i = 0; i < scene.spot_lights.size(); i++) {
    auto light_position = scene.spot_lights.at(i).position();
    glUniform3fv(standard_program_.spot_lights.at(i).position, 1,
                 glm::value_ptr(light_position));

    glUniform3fv(standard_program_.spot_lights.at(i).color, 1,
                 glm::value_ptr(scene.spot_lights.at(i).color));
    glUniform1fv(standard_program_.spot_lights.at(i).strength, 1,
                 &scene.spot_lights.at(i).strength);

    auto view = scene.spot_lights.at(i).camera.view();
    glUniformMatrix4fv(standard_program_.spot_lights.at(i).view, 1, GL_FALSE,
                       glm::value_ptr(view));

    auto projection = scene.spot_lights.at(i).camera.projection();
    glUniformMatrix4fv(standard_program_.spot_lights.at(i).projection, 1,
                       GL_FALSE, glm::value_ptr(projection));

    auto light_angle = scene.spot_lights.at(i).angle();
    glUniform1fv(standard_program_.spot_lights.at(i).angle, 1, &light_angle);
    auto light_direction = scene.spot_lights.at(i).direction();
    glUniform3fv(standard_program_.spot_lights.at(i).direction, 1,
                 glm::value_ptr(light_direction));

    auto light_blend = scene.spot_lights.at(i).blend();
    glUniform1fv(standard_program_.spot_lights.at(i).blend, 1, &light_blend);
  }

  // Directional light
  glUniform3fv(standard_program_.directional_light.position, 1,
               glm::value_ptr(scene.directional_light.position));

  glUniform3fv(standard_program_.directional_light.direction, 1,
               glm::value_ptr(scene.directional_light.direction));

  glUniform3fv(standard_program_.directional_light.color, 1,
               glm::value_ptr(scene.directional_light.color));

  glUniform1fv(standard_program_.directional_light.strength, 1,
               &scene.directional_light.strength);

  // ---
  glUniform2iv(standard_program_.camera.resolution, 1,
               glm::value_ptr(resolution));

  glUniform3fv(standard_program_.fog.color_near, 1,
               glm::value_ptr(scene.fog.color_near));
  glUniform3fv(standard_program_.fog.color_far, 1,
               glm::value_ptr(scene.fog.color_far));
  glUniform1fv(standard_program_.fog.attenuation_factor, 1,
               &scene.fog.attenuation_factor);
  glUniform1fv(standard_program_.fog.min, 1, &scene.fog.min);
  glUniform1fv(standard_program_.fog.max, 1, &scene.fog.max);

  render_sky(scene.sky, camera, scene.fog, resolution, standard_program_);

  for (auto &model : scene.models) {
    render_model(model, glm::mat4(1.0f), camera, scene.spot_lights,
                 scene.environment_lights, scene.fog, resolution,
                 standard_program_);
  }
  render_boxes(scene.boxes, camera);
}

void Renderer::render_boxes(const Boxes &boxes,
                            const mos::gfx::Camera &camera) {

  glUseProgram(box_program_.program);
  glBindVertexArray(box.vertex_array);

  for (auto &box : boxes) {
    glm::vec3 size = box.size();
    glm::mat4 transform = box.transform;
    glm::mat4 mv =
        camera.view() * transform * glm::scale(glm::mat4(1.0f), size);
    glm::mat4 mvp = camera.projection() * camera.view() * transform *
                    glm::scale(glm::mat4(1.0f), size);

    glUniformMatrix4fv(box_program_.model_view_projection, 1, GL_FALSE,
                       &mvp[0][0]);

    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, nullptr);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT,
                   (GLvoid *)(4 * sizeof(GLuint)));
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT,
                   (GLvoid *)(8 * sizeof(GLuint)));
  }
  glBindVertexArray(0);
}

void Renderer::render_sky(const Model &model, const Camera &camera,
                          const Fog &fog, const glm::vec2 &resolution,
                          const Standard_program &program) {
  glUseProgram(program.program);

  auto res = glm::ivec2(resolution);
  glUniform2iv(program.camera.resolution, 1, glm::value_ptr(res));

  glUniform3fv(program.fog.color_near, 1, glm::value_ptr(fog.color_near));
  glUniform3fv(program.fog.color_far, 1, glm::value_ptr(fog.color_far));
  glUniform1fv(program.fog.attenuation_factor, 1, &fog.attenuation_factor);
  glUniform1fv(program.fog.min, 1, &fog.min);
  glUniform1fv(program.fog.max, 1, &fog.max);

  auto sky_camera = camera;
  auto view = sky_camera.view();
  view[3] = glm::vec4(0.0f, 0.0, 0.0f, 1.0f);
  sky_camera.view(view);
  load(model);
  render_model(model, glm::mat4(1.0f), sky_camera, Spot_lights(),
               Environment_lights(), fog, resolution, program);

  glEnable(GL_DEPTH_TEST);
  glDepthMask(true);
}

void Renderer::render_clouds(const Clouds &clouds,
                             const Directional_light &directional_light,
                             const Spot_lights &spot_lights,
                             const Environment_lights &environment_lights,
                             const mos::gfx::Camera &camera,
                             const glm::ivec2 &resolution,
                             const Cloud_program &program,
                             const GLenum &draw_mode) {
  glDepthMask(GL_FALSE);
  for (auto &particles : clouds) {
    if (particles.blending == Cloud::Blending::Additive) {
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    } else {
      glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    }
    if (vertex_arrays_.find(particles.id()) == vertex_arrays_.end()) {
      unsigned int vertex_array;
      glGenVertexArrays(1, &vertex_array);
      glBindVertexArray(vertex_array);
      if (array_buffers_.find(particles.id()) == array_buffers_.end()) {
        array_buffers_.insert(
            {particles.id(),
             Buffer(GL_ARRAY_BUFFER, particles.points.size() * sizeof(Point),
                    particles.points.data(), GL_STREAM_DRAW,
                    particles.points.modified())});
      }
      vertex_arrays_.insert(
          {particles.id(), Vertex_array(particles, array_buffers_)});
    }
    glBindBuffer(GL_ARRAY_BUFFER, array_buffers_.at(particles.id()).id);
    glBufferData(GL_ARRAY_BUFFER, particles.points.size() * sizeof(Point),
                 particles.points.data(), GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glm::mat4 mv = camera.view();
    glm::mat4 mvp = camera.projection() * camera.view();

    glUseProgram(program.program);

    glBindVertexArray(vertex_arrays_.at(particles.id()).id);

    load(particles.texture);
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D,
                  particles.texture
                      ? textures_.at(particles.texture->id()).texture
                      : black_texture_.texture);
    glUniform1i(program.albedo_sampler, 10);

    // TODO: Check
    glBindTexture(GL_TEXTURE_CUBE_MAP, propagate_target_.texture);
    glUniform1i(program.environment_samplers[0].map, 5);

    // TODO: Check
    glBindTexture(GL_TEXTURE_CUBE_MAP, environment_maps_targets_[1].texture);
    glUniform1i(program.environment_samplers[1].map, 6);

    glUniformMatrix4fv(program.model_view_projection, 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(program.model_view, 1, GL_FALSE, &mv[0][0]);
    auto projection = camera.projection();
    glUniformMatrix4fv(program.projection, 1, GL_FALSE,
                       glm::value_ptr(projection));

    auto emissive = static_cast<int>(particles.emissive);
    glUniform1iv(program.emissive_sampler, 1, &emissive);

    auto position = camera.position();
    glUniform3fv(program.camera.position, 1, glm::value_ptr(position));
    glm::vec2 res = resolution;
    glUniform2fv(program.resolution, 1, glm::value_ptr(res));

    for (size_t i = 0; i < environment_lights.size(); i++) {
      auto position = environment_lights.at(i).position();
      glUniform3fv(program.environment_samplers.at(i).position, 1,
                   glm::value_ptr(position));
      auto extent = environment_lights.at(i).extent();
      glUniform3fv(program.environment_samplers.at(i).extent, 1,
                   glm::value_ptr(extent));
      glUniform1fv(program.environment_samplers.at(i).strength, 1,
                   &environment_lights.at(i).strength);
      glUniform1fv(program.environment_samplers.at(i).falloff, 1,
                   &environment_lights.at(i).falloff);
    }
    for (size_t i = 0; i < spot_lights.size(); i++) {
      auto light_position = spot_lights.at(i).position();
      glUniform3fv(program.spot_lights.at(i).position, 1,
                   glm::value_ptr(light_position));

      glUniform3fv(program.spot_lights.at(i).color, 1,
                   glm::value_ptr(spot_lights.at(i).color));
      glUniform1fv(program.spot_lights.at(i).strength, 1,
                   &spot_lights.at(i).strength);

      auto view = spot_lights.at(i).camera.view();
      glUniformMatrix4fv(program.spot_lights.at(i).view, 1, GL_FALSE,
                         glm::value_ptr(view));
      auto projection = spot_lights.at(i).camera.projection();
      glUniformMatrix4fv(program.spot_lights.at(i).projection, 1, GL_FALSE,
                         glm::value_ptr(projection));

      auto light_angle = spot_lights.at(i).angle();
      glUniform1fv(program.spot_lights.at(i).angle, 1, &light_angle);
      auto light_direction = spot_lights.at(i).direction();
      glUniform3fv(program.spot_lights.at(i).direction, 1,
                   glm::value_ptr(light_direction));
    }

    // Directional light
    glUniform3fv(program.directional_light.position, 1,
                 glm::value_ptr(directional_light.position));

    glUniform3fv(program.directional_light.direction, 1,
                 glm::value_ptr(directional_light.direction));

    glUniform3fv(program.directional_light.color, 1,
                 glm::value_ptr(directional_light.color));

    glUniform1fv(program.directional_light.strength, 1,
                 &directional_light.strength);

    glDrawArrays(draw_mode, 0, particles.points.size());
  }
  glDepthMask(GL_TRUE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::render_model(const Model &model,
                            const glm::mat4 &parent_transform,
                            const Camera &camera, const Spot_lights &lights,
                            const Environment_lights &environment_lights,
                            const Fog &fog, const glm::vec2 &resolution,
                            const Environment_program &program) {

  if (camera.in_frustum(glm::vec3(parent_transform[3]) + model.centroid(),
                        model.radius())) {

    const glm::mat4 mvp = camera.projection() * camera.view() *
                          parent_transform * model.transform;

    if (model.mesh && model.material.alpha > 0) {
      glBindVertexArray(vertex_arrays_.at(model.mesh->id()).id);

      const auto &uniforms = program;

      glActiveTexture(GL_TEXTURE7);
      glBindTexture(
          GL_TEXTURE_2D,
          model.material.albedo.texture
              ? textures_.at(model.material.albedo.texture->id()).texture
              : black_texture_.texture);

      glActiveTexture(GL_TEXTURE8);
      glBindTexture(
          GL_TEXTURE_2D,
          model.material.emission.texture
              ? textures_.at(model.material.emission.texture->id()).texture
              : black_texture_.texture);

      static const glm::mat4 bias(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0,
                                  0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0);

      for (size_t i = 0; i < lights.size(); i++) {
        const glm::mat4 depth_bias_mvp =
            bias * lights.at(i).camera.projection() *
            lights.at(i).camera.view() * parent_transform * model.transform;
        glUniformMatrix4fv(uniforms.depth_bias_mvps.at(i), 1, GL_FALSE,
                           &depth_bias_mvp[0][0]);
      }

      // TODO: Cascaded

      glUniformMatrix4fv(uniforms.model_view_projection, 1, GL_FALSE,
                         &mvp[0][0]);
      auto model_matrix = parent_transform * model.transform;
      glUniformMatrix4fv(uniforms.model_matrix, 1, GL_FALSE,
                         &model_matrix[0][0]);

      const auto normal_matrix =
          glm::inverseTranspose(glm::mat3(parent_transform * model.transform));
      glUniformMatrix3fv(uniforms.normal_matrix, 1, GL_FALSE,
                         &normal_matrix[0][0]);

      glUniform3fv(uniforms.material.albedo, 1,
                   glm::value_ptr(model.material.albedo.value));
      glUniform3fv(uniforms.material.emission, 1,
                   glm::value_ptr(model.material.emission.value));
      glUniform1fv(uniforms.material.roughness, 1,
                   &model.material.roughness.value);
      glUniform1fv(uniforms.material.metallic, 1,
                   &model.material.metallic.value);
      glUniform1fv(uniforms.material.index_of_refraction, 1,
                   &model.material.index_of_refraction);
      glUniform1fv(uniforms.material.alpha, 1, &model.material.alpha);
      glUniform1fv(uniforms.material.transmission, 1,
                   &model.material.transmission);
      glUniform1fv(uniforms.material.ambient_occlusion, 1,
                   &model.material.ambient_occlusion.value);

      glDrawElements(GL_TRIANGLES, model.mesh->triangles.size() * 3,
                     GL_UNSIGNED_INT, nullptr);
    }
  }
  for (const auto &child : model.models) {
    render_model(child, parent_transform * model.transform, camera, lights,
                 environment_lights, fog, resolution, program);
  }
}

void Renderer::render_model(const Model &model,
                            const glm::mat4 &parent_transform,
                            const Camera &camera, const Spot_lights &lights,
                            const Environment_lights &environment_lights,
                            const Fog &fog, const glm::vec2 &resolution,
                            const Standard_program &program) {
  if (camera.in_frustum(glm::vec3(parent_transform[3]) + model.centroid(),
                        model.radius())) {
    const glm::mat4 mvp = camera.projection() * camera.view() *
                          parent_transform * model.transform;

    if (model.mesh) {
      glBindVertexArray(vertex_arrays_.at(model.mesh->id()).id);

      const auto &uniforms = program;

      glActiveTexture(GL_TEXTURE7);
      glBindTexture(
          GL_TEXTURE_2D,
          model.material.albedo.texture
              ? textures_.at(model.material.albedo.texture->id()).texture
              : black_texture_.texture);

      glActiveTexture(GL_TEXTURE8);
      glBindTexture(
          GL_TEXTURE_2D,
          model.material.emission.texture
              ? textures_.at(model.material.emission.texture->id()).texture
              : black_texture_.texture);

      glActiveTexture(GL_TEXTURE9);
      glBindTexture(
          GL_TEXTURE_2D,
          model.material.normal.texture
              ? textures_.at(model.material.normal.texture->id()).texture
              : black_texture_.texture);

      glActiveTexture(GL_TEXTURE10);
      glBindTexture(
          GL_TEXTURE_2D,
          model.material.metallic.texture
              ? textures_.at(model.material.metallic.texture->id()).texture
              : black_texture_.texture);

      glActiveTexture(GL_TEXTURE11);
      glBindTexture(
          GL_TEXTURE_2D,
          model.material.roughness.texture
              ? textures_.at(model.material.roughness.texture->id()).texture
              : black_texture_.texture);

      glActiveTexture(GL_TEXTURE12);
      glBindTexture(
          GL_TEXTURE_2D,
          model.material.ambient_occlusion.texture
              ? textures_.at(model.material.ambient_occlusion.texture->id())
                    .texture
              : white_texture_.texture);

      static const glm::mat4 bias(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0,
                                  0.0, 0.5, 0.0, 0.5, 0.5, 0.5, 1.0);

      for (size_t i = 0; i < lights.size(); i++) {
        auto projection = lights.at(i).camera.projection();
        auto view = lights.at(i).camera.view();
        const glm::mat4 depth_bias_mvp =
            bias * projection * view * parent_transform * model.transform;
        glUniformMatrix4fv(uniforms.depth_bias_mvps.at(i), 1, GL_FALSE,
                           &depth_bias_mvp[0][0]);
      }

      // Cascaded
      for (size_t i = 0; i < directional_light_ortho_matrices.size(); i++) {
        auto light_view = light_view_matrix[i];
        const glm::mat4 cascaded_depth_bias_mvp =
            bias * directional_light_ortho_matrices[i] * light_view *
            parent_transform * model.transform;
        glUniformMatrix4fv(uniforms.cascaded_depth_bias_mvps.at(i), 1, GL_FALSE,
                           &cascaded_depth_bias_mvp[0][0]);
      }

      glUniformMatrix4fv(uniforms.model_view_projection, 1, GL_FALSE,
                         &mvp[0][0]);
      auto model_matrix = parent_transform * model.transform;
      glUniformMatrix4fv(uniforms.model_matrix, 1, GL_FALSE,
                         &model_matrix[0][0]);

      glm::mat3 normal_matrix = glm::inverseTranspose(
          glm::mat3(parent_transform) * glm::mat3(model.transform));
      normal_matrix =
          glm::inverseTranspose(glm::mat3(parent_transform * model.transform));
      glUniformMatrix3fv(uniforms.normal_matrix, 1, GL_FALSE,
                         &normal_matrix[0][0]);

      glUniform3fv(uniforms.material.albedo, 1,
                   glm::value_ptr(model.material.albedo.value));
      glUniform3fv(uniforms.material.emission, 1,
                   glm::value_ptr(model.material.emission.value));
      glUniform1fv(uniforms.material.roughness, 1,
                   &model.material.roughness.value);
      glUniform1fv(uniforms.material.metallic, 1,
                   &model.material.metallic.value);
      glUniform1fv(uniforms.material.index_of_refraction, 1,
                   &model.material.index_of_refraction);
      glUniform1fv(uniforms.material.alpha, 1, &model.material.alpha);
      glUniform1fv(uniforms.material.transmission, 1,
                   &model.material.transmission);
      glUniform1fv(uniforms.material.ambient_occlusion, 1,
                   &model.material.ambient_occlusion.value);

      glDrawElements(GL_TRIANGLES, model.mesh->triangles.size() * 3,
                     GL_UNSIGNED_INT, nullptr);
    }
  }
  for (const auto &child : model.models) {
    render_model(child, parent_transform * model.transform, camera, lights,
                 environment_lights, fog, resolution, program);
  }
}

void Renderer::clear(const glm::vec4 &color) {
  glClearDepthf(1.0f);
  glClearColor(color.r, color.g, color.b, color.a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::clear_depth() {
  glClearDepthf(1.0f);
  glClear(GL_DEPTH_BUFFER_BIT);
}

void Renderer::clear_color(const glm::vec4 &color) {
  glClearColor(color.r, color.g, color.b, color.a);
  glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::blur(const GLuint input_texture,
                    const Post_target &buffer_target,
                    const Post_target &output_target, const float iterations) {
  glViewport(0, 0, GLsizei(output_target.resolution.x),
             GLsizei(output_target.resolution.y));
  for (int i = 0; i < iterations; i++) {
    GLint horizontal = (i % 2 == 1);
    glBindFramebuffer(GL_FRAMEBUFFER, horizontal ? output_target.frame_buffer
                                                 : buffer_target.frame_buffer);
    glUseProgram(blur_program_.program);
    clear(glm::vec4(0.0f));
    glBindVertexArray(quad_.vertex_array);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, i == 0 ? input_texture
                                        : horizontal ? buffer_target.texture
                                                     : output_target.texture);
    glUniform1i(blur_program_.color_sampler, 0);
    glUniform1iv(blur_program_.horizontal, 1, &horizontal);

    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
}

void Renderer::render_shadow_maps(const Models &models,
                                  const Spot_lights &lights) {
  for (size_t i = 0; i < shadow_maps_.size(); i++) {
    if (lights.at(i).strength > 0.0f) {
      auto frame_buffer = shadow_maps_.at(i).frame_buffer;
      glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
      glClear(GL_DEPTH_BUFFER_BIT);
      auto resolution = shadow_maps_render_buffer_.resolution();
      glUseProgram(depth_program_.program);
      glViewport(0, 0, resolution.x, resolution.y);

      glUniform1i(depth_program_.albedo_sampler, 0);

      for (auto &model : models) {
        render_model_depth(model, glm::mat4(1.0f), lights.at(i).camera,
                           resolution, depth_program_);
      }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      glViewport(0, 0, shadow_maps_render_buffer_.resolution().x,
                 shadow_maps_render_buffer_.resolution().y);

      blur(shadow_maps_.at(i).texture, shadow_map_blur_target_,
           shadow_map_blur_targets_.at(i), 4);
    }
  }
}

void Renderer::render_cascaded_shadow_maps(const Models &models,
                                           const Directional_light &light,
                                           const Camera &camera) {
  std::vector<mos::gfx::Model> models_to_render(models.begin(), models.end());

  const float lambda = .7f;
  const float min_distance = 0.0f;
  const float max_distance = 1.0f;

  const auto camera_projection = camera.projection();
  const auto camera_view = camera.view();

  const auto light_dir = light.direction;

  const float near_clip = camera.near_plane();
  const float far_clip = camera.far_plane();
  const float clip_range = far_clip - near_clip;

  const float min_z = near_clip + min_distance * clip_range;
  const float max_z = near_clip + max_distance * clip_range;

  const float z_range = max_z - min_z;
  const float ratio = max_z / min_z;

  for (int i = 0; i < cascade_count; i++) {
    float p = (i + 1.f) / static_cast<float>(cascade_count);
    float log = min_z * std::pow(ratio, p);
    float uniform = min_z + z_range * p;
    float d = lambda * (log - uniform) + uniform;
    cascade_splits[i] = (d - near_clip) / clip_range;
  }

  for (unsigned int cascade_idx = 0; cascade_idx < cascade_count;
       ++cascade_idx) {

    const float prev_split_distance =
        cascade_idx == 0 ? min_distance : cascade_splits[cascade_idx - 1];
    const float split_distance = cascade_splits[cascade_idx];

    // TODO: Incorporate into camera
    glm::vec3 frustum_corners_world[8] = {
        glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 1.0f, -1.0f),
        glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, 1.0f, 1.0f),  glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, -1.0f, 1.0f),  glm::vec3(-1.0f, -1.0f, 1.0f),
    };

    const glm::mat4 inv_view_proj =
        glm::inverse(camera_projection * camera_view);
    for (auto &corner : frustum_corners_world) {
      glm::vec4 inverse_point = inv_view_proj * glm::vec4(corner, 1.0f);
      corner = glm::vec3(inverse_point / inverse_point.w);
    }

    for (int i = 0; i < 4; i++) {
      const glm::vec3 corner_ray =
          frustum_corners_world[i + 4] - frustum_corners_world[i];
      const glm::vec3 near_corner_ray = corner_ray * prev_split_distance;
      const glm::vec3 far_corner_ray = corner_ray * split_distance;
      frustum_corners_world[i + 4] = frustum_corners_world[i] + far_corner_ray;
      frustum_corners_world[i] = frustum_corners_world[i] + near_corner_ray;
    }

    auto frustum_center = glm::vec3(0.0f);
    for (auto corner : frustum_corners_world) {
      frustum_center += corner;
    }
    frustum_center /= 8.0f;

    float radius = 0.0f;
    for (auto corner : frustum_corners_world) {
      float distance = glm::length(corner - frustum_center);
      radius = glm::max(radius, distance);
    }
    radius = std::ceil(radius * 16.0f) / 16.0f;

    const glm::vec3 max_extents = glm::vec3(radius, radius, radius);
    const glm::vec3 min_extents = -max_extents;

    const glm::vec3 light_position =
        frustum_center - glm::normalize(light_dir) * -min_extents.z;
    light_view_matrix[cascade_idx] = glm::mat4(1.0f);
    light_view_matrix[cascade_idx] = glm::lookAt(light_position, frustum_center,
                                                 glm::vec3(0.0f, 1.0f, 0.0f));

    const glm::vec3 cascade_extents = max_extents - min_extents;

    directional_light_ortho_matrices[cascade_idx] =
        glm::ortho(min_extents.x, max_extents.x, min_extents.y, max_extents.y,
                   0.0f, cascade_extents.z * 2); // TODO: hack times two?

    // TOOD: Do Same thing on standard shadow maps
    const auto shadow_matrix = directional_light_ortho_matrices[cascade_idx] *
                               light_view_matrix[cascade_idx];
    auto shadow_origin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    shadow_origin = shadow_matrix * shadow_origin;
    shadow_origin =
        shadow_origin *
        static_cast<float>(shadow_maps_render_buffer_.resolution().x) / 2.0f;

    const glm::vec4 rounded_origin = glm::round(shadow_origin);
    glm::vec4 round_offset = rounded_origin - shadow_origin;
    round_offset =
        round_offset * 2.0f /
        static_cast<float>(shadow_maps_render_buffer_.resolution().x);
    round_offset.z = 0.0f;
    round_offset.w = 0.0f;

    glm::mat4 shadow_proj = directional_light_ortho_matrices[cascade_idx];
    shadow_proj[3] += round_offset;
    directional_light_ortho_matrices[cascade_idx] = shadow_proj;

    // TEMP
    auto frame_buffer = cascaded_shadow_maps_.at(cascade_idx).frame_buffer;
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
    glClear(GL_DEPTH_BUFFER_BIT);
    auto resolution = shadow_maps_render_buffer_.resolution();
    glUseProgram(depth_program_.program);
    glViewport(0, 0, resolution.x, resolution.y);

    glUniform1i(depth_program_.albedo_sampler, 0);

    auto light_camera = Camera(light_position, frustum_center,
                               directional_light_ortho_matrices[cascade_idx],
                               glm::vec3(0.0f, 1.0f, 0.0f));

    // for (auto &model : models) {
    auto it = models_to_render.begin();
    while (it != models_to_render.end()) {
      if (light_camera.in_frustum(it->position(), it->radius())) {
        render_model_depth(*it, glm::mat4(1.0f), light_camera, resolution,
                           depth_program_);
        if (it->radius() > radius) {
          it++;
        } else {
          it = models_to_render.erase(it);
        }
      } else {
        it++;
      }
    }

    blur(cascaded_shadow_maps_.at(cascade_idx).texture, shadow_map_blur_target_,
         cascaded_shadow_map_blur_targets_.at(cascade_idx), 2);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::render_environment(const Scene &scene,
                                  const glm::vec4 &clear_color) {
  for (size_t i = 0; i < environment_maps_targets_.size(); i++) {
    if (scene.environment_lights.at(i).strength > 0.0f) {
      GLuint frame_buffer_id = environment_maps_targets_.at(i).frame_buffer;
      glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);

      auto texture_id = environment_maps_targets_.at(i).texture;

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             GL_TEXTURE_CUBE_MAP_POSITIVE_X +
                                 cube_camera_index_.at(i),
                             texture_id, 0);

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                             GL_TEXTURE_CUBE_MAP_POSITIVE_X +
                                 cube_camera_index_.at(i),
                             environment_maps_targets_.at(i).albedo, 0);

      clear(clear_color);
      auto resolution = environment_render_buffer_.resolution();
      auto cube_camera =
          scene.environment_lights.at(i).camera(cube_camera_index_.at(i));

      render_scene(cube_camera, scene, resolution);

      cube_camera_index_.at(i) =
          cube_camera_index_.at(i) >= 5
              ? 0
              : ++cube_camera_index_.at(i); // TODO PROBLEM

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
      if (cube_camera_index_.at(i) == 0) {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
      }
      glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
  }

  int i = cube_camera_index_[0];

  glBindFramebuffer(GL_FRAMEBUFFER, propagate_target_.frame_buffer);
  glUseProgram(propagate_program_.program);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         propagate_target_.texture, 0);

  clear(glm::vec4(0.0, 1.0, 0.0, 1.0));
  auto resolution = environment_render_buffer_.resolution();

  glViewport(0, 0, resolution.x, resolution.y);

  glBindVertexArray(quad_.vertex_array);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, environment_maps_targets_[0].texture);
  glUniform1i(propagate_program_.environment_sampler, 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_CUBE_MAP, environment_maps_targets_[0].albedo);
  glUniform1i(propagate_program_.environment_albedo_sampler, 1);

  glUniform1iv(propagate_program_.side, 1, &i);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, propagate_target_.texture);
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
void Renderer::load(const Mesh &mesh) {
  if (vertex_arrays_.find(mesh.id()) == vertex_arrays_.end()) {
    vertex_arrays_.insert({mesh.id(), Vertex_array(mesh, array_buffers_,
                                                   element_array_buffers_)});
  }

  if (mesh.vertices.size() > 0 &&
      mesh.vertices.modified() > array_buffers_.at(mesh.id()).modified) {
    glBindBuffer(GL_ARRAY_BUFFER, array_buffers_.at(mesh.id()).id);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex),
                 mesh.vertices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    array_buffers_.at(mesh.id()).modified = mesh.vertices.modified();
  }
  if (mesh.triangles.size() > 0 &&
      mesh.triangles.modified() >
          element_array_buffers_.at(mesh.id()).modified) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                 element_array_buffers_.at(mesh.id()).id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 mesh.triangles.size() * 3 * sizeof(unsigned int),
                 mesh.triangles.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    element_array_buffers_.at(mesh.id()).modified = mesh.triangles.modified();
  }
}

void Renderer::unload(const Mesh &mesh) {
  if (vertex_arrays_.find(mesh.id()) != vertex_arrays_.end()) {
    vertex_arrays_.erase(mesh.id());

    if (array_buffers_.find(mesh.id()) != array_buffers_.end()) {
      array_buffers_.erase(mesh.id());
    }
    if (element_array_buffers_.find(mesh.id()) !=
        element_array_buffers_.end()) {
      element_array_buffers_.erase(mesh.id());
    }
  }
}

void Renderer::load(const Shared_mesh &mesh) {
  if (mesh) {
    load(*mesh);
  }
}

void Renderer::unload(const Shared_mesh &mesh) {
  if (mesh) {
    unload(*mesh);
  }
}
void Renderer::load(const Models &models) {
  for (auto &model : models) {
    load(model);
  }
}

void Renderer::render_texture_targets(const Scene &scene) {
  for (auto &target : scene.texture_targets) {
    if (frame_buffers_.find(target.target.id()) == frame_buffers_.end()) {
      frame_buffers_.insert({target.target.id(),
                             Frame_buffer(target, textures_, render_buffers_)});
    }
    const auto &fb = frame_buffers_.at(target.target.id());
    glBindFramebuffer(GL_FRAMEBUFFER, fb.id);

    auto texture_id = textures_.at(target.texture->id()).texture;

    clear(glm::vec4(0.0f));

    render_scene(target.camera, scene,
                 glm::ivec2(target.texture->width(), target.texture->height()));
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}

void Renderer::render_model_depth(const Model &model,
                                  const glm::mat4 &transform,
                                  const Camera &camera,
                                  const glm::vec2 &resolution,
                                  const Depth_program &program) {
  if (camera.in_frustum(glm::vec3(transform[3]) + model.centroid(),
                        model.radius())) {
    const glm::mat4 mvp =
        camera.projection() * camera.view() * transform * model.transform;

    if (model.mesh) {
      glBindVertexArray(vertex_arrays_.at(model.mesh->id()).id);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(
          GL_TEXTURE_2D,
          model.material.albedo.texture
              ? textures_.at(model.material.albedo.texture->id()).texture
              : black_texture_.texture);

      glUniformMatrix4fv(program.model_view_projection, 1, GL_FALSE,
                         &mvp[0][0]);

      glUniform3fv(program.albedo, 1,
                   glm::value_ptr(model.material.albedo.value));
      glUniform3fv(program.emission, 1,
                   glm::value_ptr(model.material.emission.value));
      const int num_elements =
          model.mesh ? model.mesh->triangles.size() * 3 : 0;
      glDrawElements(GL_TRIANGLES, num_elements, GL_UNSIGNED_INT, nullptr);
    }
  }
  for (const auto &child : model.models) {
    render_model_depth(child, transform * model.transform, camera, resolution,
                       program);
  }
}

void Renderer::render(const Scenes &scenes, const glm::vec4 &color,
                      const glm::ivec2 &resolution) {
  for (auto &scene : scenes) {
    load(scene.models);
  }
  render_shadow_maps(scenes[0].models, scenes[0].spot_lights);
  render_cascaded_shadow_maps(scenes[0].models, scenes[0].directional_light,
                              scenes[0].camera);
  render_environment(scenes[0], color);
  render_texture_targets(scenes[0]);

  glBindFramebuffer(GL_FRAMEBUFFER, standard_target_.frame_buffer);
  clear(glm::vec4(0.0f));

  for (auto &scene : scenes) {

    render_scene(scene.camera, scene, resolution);
    render_clouds(scene.point_clouds, scene.directional_light,
                  scene.spot_lights, scene.environment_lights, scene.camera,
                  resolution, point_cloud_program_, GL_POINTS);

    render_clouds(scene.line_clouds, scene.directional_light, scene.spot_lights,
                  scene.environment_lights, scene.camera, resolution,
                  line_cloud_program_, GL_LINES);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, standard_target_.frame_buffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, multisample_target_.frame_buffer);
    glBlitFramebuffer(0, 0, resolution.x, resolution.y, 0, 0, resolution.x,
                      resolution.y, GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT,
                      GL_NEAREST);
  }

  // Bloom
  glViewport(0, 0, post_target0_.resolution.x, post_target0_.resolution.y);
  glBindFramebuffer(GL_FRAMEBUFFER, post_target0_.frame_buffer);
  clear(glm::vec4(0.0f));

  glUseProgram(bloom_program_.program);
  glBindVertexArray(quad_.vertex_array);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, multisample_target_.texture);
  glUniform1i(bloom_program_.color_sampler, 0);

  glDrawArrays(GL_TRIANGLES, 0, 6);

  blur(post_target0_.texture, post_target1_, post_target0_);

  // Render to screen
  glViewport(0, 0, resolution.x, resolution.y);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  clear(glm::vec4(0.0f));

  // Compositing
  glUseProgram(compositing_program_.program);

  glBindVertexArray(quad_.vertex_array);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, multisample_target_.texture);
  glUniform1i(compositing_program_.color_sampler, 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, post_target0_.texture);
  glUniform1i(compositing_program_.bloom_sampler, 1);

  float strength = 0.1f;
  glUniform1fv(compositing_program_.bloom_strength, 1, &strength);

  glDrawArrays(GL_TRIANGLES, 0, 6);
}

} // namespace mos::gfx
