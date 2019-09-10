#include <glm/glm.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/color_space.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <mos/gfx/mesh.hpp>
#include <mos/gfx/model.hpp>
#include <mos/gfx/renderer.hpp>
#include <mos/util.hpp>
#include <functional>

namespace mos {
namespace gfx {

auto generate(const std::function<void(GLsizei, GLuint*)> & f){
  GLuint id;
  f(1, &id);
  return id;
}

auto wrap_convert(const Texture::Wrap& w) {
  static const std::map<Texture::Wrap, GLint> wrap_map{
      {Texture::Wrap::Clamp, GL_CLAMP_TO_EDGE},
      {Texture::Wrap::Repeat, GL_REPEAT}};
  return wrap_map.at(w);
}

auto filter_convert(const Texture::Filter &f) {
  static const std::map<Texture::Filter, GLint> filter_map{
      {Texture::Filter::Linear, GL_LINEAR},
      {Texture::Filter::Closest, GL_NEAREST}};
  return filter_map.at(f);
}

auto filter_convert_mip(const Texture::Filter &f) {
  static const std::map<Texture::Filter, GLint> filter_map_mip{
      {Texture::Filter::Linear, GL_LINEAR_MIPMAP_LINEAR},
      {Texture::Filter::Closest, GL_NEAREST_MIPMAP_LINEAR}};
  return filter_map_mip.at(f);
}

struct FormatPair {
  GLint internal_format;
  GLenum format;
};

auto format_convert(const Texture::Format &f){
  static const std::map<Texture::Format, FormatPair> format_map{
      {Texture::Format::R, {GL_RED, GL_RED}},
      {Texture::Format::RG, {GL_RG, GL_RG}},
      {Texture::Format::SRGB, {GL_SRGB, GL_RGB}},
      {Texture::Format::SRGBA, {GL_SRGB_ALPHA, GL_RGBA}},
      {Texture::Format::RGB, {GL_RGB, GL_RGB}},
      {Texture::Format::RGBA, {GL_RGBA, GL_RGBA}}};
  return format_map.at(f);
}

void APIENTRY
message_callback(GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar *message,
                 const void *userParam) {
  if (severity == GL_DEBUG_SEVERITY_HIGH || severity == GL_DEBUG_SEVERITY_MEDIUM) {
    fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
            type, severity, message);
  }
}

Renderer::Renderer(const glm::vec4 &color, const glm::ivec2 &resolution)
    : context_(gladLoadGL()),standard_target_(resolution), temp_target_(resolution / 4, GL_RGBA16F),
      multisample_target_(resolution, GL_RGBA16F),
      screen_target_(resolution, GL_R11F_G11F_B10F),
      bloom_target_(resolution / 4, GL_R11F_G11F_B10F),
      depth_of_field_target_(resolution / 4, GL_R11F_G11F_B10F),
      post_target_(resolution / 4, GL_RGBA16F), quad_(),
      black_texture_(Texture_buffer_2D(GL_RGBA, GL_RGBA, 1, 1, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT,
                                             std::array<unsigned char, 4>{0, 0, 0, 0}.data())),
      white_texture_(Texture_buffer_2D(GL_RGBA, GL_RGBA, 1, 1, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_REPEAT,
                                             std::array<unsigned char, 4>{255, 255, 255, 255}.data())),
      brdf_lut_texture_(Texture_buffer_2D(Texture_2D("assets/brdfLUT.png", false, false,
                                   Texture_2D::Filter::Linear,
                                   Texture_2D::Wrap::Clamp))),
      cube_camera_index_({0, 0}), shadow_maps_render_buffer_(resolution.y),
      shadow_maps_{Shadow_map_target(shadow_maps_render_buffer_),
                   Shadow_map_target(shadow_maps_render_buffer_),
                   Shadow_map_target(shadow_maps_render_buffer_),
                   Shadow_map_target(shadow_maps_render_buffer_)},
      shadow_map_blur_target_(Post_target(
          glm::ivec2(shadow_maps_render_buffer_.resolution), GL_RG32F)),
      shadow_map_blur_targets_{
          Post_target(glm::ivec2(shadow_maps_render_buffer_.resolution),
                      GL_RG32F),
          Post_target(glm::ivec2(shadow_maps_render_buffer_.resolution),
                      GL_RG32F),
          Post_target(glm::ivec2(shadow_maps_render_buffer_.resolution),
                      GL_RG32F),
          Post_target(glm::ivec2(shadow_maps_render_buffer_.resolution),
                      GL_RG32F)},
      environment_render_buffer_(128),
      environment_maps_targets{
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
  clear(color);
}

Renderer::~Renderer() {
  for (auto &fb : frame_buffers_) {
    glDeleteFramebuffers(1, &fb.second);
  }

  for (auto &rb : render_buffers) {
    glDeleteRenderbuffers(1, &rb.second);
  }

  for (auto &va : vertex_arrays_) {
    glDeleteVertexArrays(1, &va.second);
  }
}

void Renderer::load(const Model &model) {
  load(model.mesh);
  load(model.material.albedo_map);
  load(model.material.normal_map);
  load(model.material.metallic_map);
  load(model.material.emission_map);
  load(model.material.roughness_map);
  load(model.material.ambient_occlusion_map);
  for (auto &m : model.models) {
    load(m);
  }
}

void Renderer::unload(const Model &model) {
  unload(model.mesh);
  unload(model.material.albedo_map);
  unload(model.material.normal_map);
  unload(model.material.emission_map);
  unload(model.material.metallic_map);
  unload(model.material.roughness_map);
  unload(model.material.ambient_occlusion_map);
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
                   format_convert(texture.format).format,
                   GL_UNSIGNED_BYTE, texture.layers[0].data());
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

void Renderer::render_scene(const Camera &camera,
                            const Scene &scene,
                            const glm::ivec2 &resolution) {
  glViewport(0, 0, resolution.x, resolution.y);
  glUseProgram(standard_program_.program);
  glUniform1i(standard_program_.brdf_lut, 0);
  glUniform1i(standard_program_.shadow_maps[0], 1);
  glUniform1i(standard_program_.shadow_maps[1], 2);
  glUniform1i(standard_program_.shadow_maps[2], 3);
  glUniform1i(standard_program_.shadow_maps[3], 4);
  glUniform1i(standard_program_.environment_maps[0].map, 5);
  glUniform1i(standard_program_.environment_maps[1].map, 6);

  glUniform1i(standard_program_.material_albedo_sampler, 7);
  glUniform1i(standard_program_.material_emission_sampler, 8);
  glUniform1i(standard_program_.material_normal_sampler, 9);
  glUniform1i(standard_program_.material_metallic_sampler, 10);
  glUniform1i(standard_program_.material_roughness_sampler, 11);
  glUniform1i(standard_program_.material_ambient_occlusion_sampler, 12);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, brdf_lut_texture_.texture);

  glActiveTexture(GL_TEXTURE1);
  //glBindTexture(GL_TEXTURE_2D, shadow_maps_[0].texture);
  glBindTexture(GL_TEXTURE_2D, shadow_map_blur_targets_[0].texture);

  glActiveTexture(GL_TEXTURE2);
  //glBindTexture(GL_TEXTURE_2D, shadow_maps_[1].texture);
  glBindTexture(GL_TEXTURE_2D, shadow_map_blur_targets_[1].texture);

  glActiveTexture(GL_TEXTURE3);
  //glBindTexture(GL_TEXTURE_2D, shadow_maps_[0].texture);
  glBindTexture(GL_TEXTURE_2D, shadow_map_blur_targets_[2].texture);

  glActiveTexture(GL_TEXTURE4);
  //glBindTexture(GL_TEXTURE_2D, shadow_maps_[1].texture);
  glBindTexture(GL_TEXTURE_2D, shadow_map_blur_targets_[3].texture);

  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_CUBE_MAP, propagate_target_.texture);

  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_CUBE_MAP, environment_maps_targets[1].texture);

  for (size_t i = 0; i < scene.environment_lights.size(); i++) {
    glUniform3fv(standard_program_.environment_maps.at(i).position, 1,
                 glm::value_ptr(scene.environment_lights.at(i).position()));
    glUniform3fv(standard_program_.environment_maps.at(i).extent, 1,
                 glm::value_ptr(scene.environment_lights.at(i).extent()));
    glUniform1fv(standard_program_.environment_maps.at(i).strength, 1,
                 &scene.environment_lights.at(i).strength);
    glUniform1fv(standard_program_.environment_maps.at(i).falloff, 1,
                 &scene.environment_lights.at(i).falloff);
  }

  // Camera in world space
  auto position = camera.position();
  glUniform3fv(standard_program_.camera_position, 1, glm::value_ptr(position));

  for (size_t i = 0; i < scene.lights.size(); i++) {
    glUniform3fv(standard_program_.lights.at(i).position, 1,
                 glm::value_ptr(glm::vec3(glm::vec4(scene.lights.at(i).position(), 1.0f))));

    glUniform3fv(standard_program_.lights.at(i).color, 1, glm::value_ptr(scene.lights.at(i).color));
    glUniform1fv(standard_program_.lights.at(i).strength, 1, &scene.lights.at(i).strength);

    auto view = scene.lights.at(i).camera.view();
    glUniformMatrix4fv(standard_program_.lights.at(i).view, 1, GL_FALSE,
                       glm::value_ptr(view));

    auto projection = scene.lights.at(i).camera.projection();
    glUniformMatrix4fv(standard_program_.lights.at(i).projection, 1, GL_FALSE,
                       glm::value_ptr(projection));

    auto light_angle = scene.lights.at(i).angle();
    glUniform1fv(standard_program_.lights.at(i).angle, 1, &light_angle);
    glUniform3fv(standard_program_.lights.at(i).direction, 1, glm::value_ptr(scene.lights.at(i).direction()));
  }

  glUniform2iv(standard_program_.camera_resolution, 1, glm::value_ptr(resolution));

  glUniform3fv(standard_program_.fog_color_near, 1, glm::value_ptr(scene.fog.color_near));
  glUniform3fv(standard_program_.fog_color_far, 1, glm::value_ptr(scene.fog.color_far));
  glUniform1fv(standard_program_.fog_attenuation_factor, 1,
               &scene.fog.attenuation_factor);

  for (auto &model : scene.models) {
    render_model(model, glm::mat4(1.0f), camera,
                 scene.lights,
                 scene.environment_lights,
                 scene.fog,
                 resolution, standard_program_);
  }
  render_boxes(scene.boxes, camera);
}

void Renderer::render_boxes(const Boxes &boxes, const mos::gfx::Camera &camera) {

  glUseProgram(box_program_.program);
  glBindVertexArray(box.vertex_array);

  for (auto &box : boxes) {
    glm::vec3 size = box.size();
    glm::mat4 transform = box.transform;
    glm::mat4 mv = camera.view() * transform * glm::scale(glm::mat4(1.0f), size);
    glm::mat4 mvp = camera.projection() * camera.view() * transform *
        glm::scale(glm::mat4(1.0f), size);

    glUniformMatrix4fv(box_program_.model_view_projection, 1, GL_FALSE, &mvp[0][0]);

    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, nullptr);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT,
                   (GLvoid *) (4 * sizeof(GLuint)));
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT,
                   (GLvoid *) (8 * sizeof(GLuint)));
  }
  glBindVertexArray(0);
}

void Renderer::render_particles(const Particle_clouds &clouds,
                                const Lights &lights,
                                const mos::gfx::Camera &camera,
                                const glm::ivec2 &resolution) {
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  glDisable(GL_DEPTH_TEST);
  for (auto &particles : clouds) {
    if (vertex_arrays_.find(particles.id()) == vertex_arrays_.end()) {
      unsigned int vertex_array;
      glGenVertexArrays(1, &vertex_array);
      glBindVertexArray(vertex_array);
      if (array_buffers_.find(particles.id()) == array_buffers_.end()) {
        unsigned int array_buffer;
        glGenBuffers(1, &array_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
        glBufferData(GL_ARRAY_BUFFER, particles.particles.size() * sizeof(Particle),
                     particles.particles.data(), GL_STREAM_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        array_buffers_.insert({particles.id(), Buffer(GL_ARRAY_BUFFER,
                                                            particles.particles.size() * sizeof(Particle),
                                                            particles.particles.data(),
                                                            GL_STREAM_DRAW,
                                                            particles.particles.modified())});
      }
      glBindBuffer(GL_ARRAY_BUFFER, array_buffers_.at(particles.id()).id);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), nullptr);
      glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle),
                            reinterpret_cast<const void *>(sizeof(glm::vec3)));
      glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle),
                            reinterpret_cast<const void *>(sizeof(glm::vec3) +
                                sizeof(glm::vec4)));
      glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle),
                            reinterpret_cast<const void *>(sizeof(glm::vec3) +
                                sizeof(glm::vec4) + sizeof(float)));
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glEnableVertexAttribArray(2);
      glEnableVertexAttribArray(3);
      glBindVertexArray(0);
      vertex_arrays_.insert({particles.id(), vertex_array});
    }
    glBindBuffer(GL_ARRAY_BUFFER, array_buffers_.at(particles.id()).id);
    glBufferData(GL_ARRAY_BUFFER, particles.particles.size() * sizeof(Particle),
                 particles.particles.data(), GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glm::mat4 mv = camera.view();
    glm::mat4 mvp = camera.projection() * camera.view();

    glUseProgram(particle_program_.program);

    glBindVertexArray(vertex_arrays_.at(particles.id()));

    load(particles.emission_map);
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, particles.emission_map
                                 ? textures_.at(particles.emission_map->id()).texture
                                     : black_texture_.texture);
    glUniform1i(particle_program_.texture, 10);

    glUniformMatrix4fv(particle_program_.model_view_projection, 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(particle_program_.model_view, 1, GL_FALSE, &mv[0][0]);
    auto projection = camera.projection();
    glUniformMatrix4fv(particle_program_.projection, 1, GL_FALSE, glm::value_ptr(projection));
    auto r = glm::vec2(resolution); // TODO: Remove.
    glUniform2fv(particle_program_.resolution, 1, glm::value_ptr(r));

    for (size_t i = 0; i < lights.size(); i++) {
      glUniform3fv(particle_program_.lights.at(i).position, 1,
                   glm::value_ptr(glm::vec3(glm::vec4(lights.at(i).position(), 1.0f))));

      glUniform3fv(particle_program_.lights.at(i).color, 1, glm::value_ptr(lights.at(i).color));
      glUniform1fv(particle_program_.lights.at(i).strength, 1, &lights.at(i).strength);

      auto view = lights.at(i).camera.view();
      glUniformMatrix4fv(particle_program_.lights.at(i).view, 1, GL_FALSE,
                         glm::value_ptr(view));
      auto projection = lights.at(i).camera.projection();
      glUniformMatrix4fv(particle_program_.lights.at(i).projection, 1, GL_FALSE,
                         glm::value_ptr(projection));

      auto light_angle = lights.at(i).angle();
      glUniform1fv(particle_program_.lights.at(i).angle, 1, &light_angle);
      glUniform3fv(particle_program_.lights.at(i).direction, 1, glm::value_ptr(lights.at(i).direction()));
    }

    auto position = camera.position();
    glUniform3fv(particle_program_.camera_position, 1, glm::value_ptr(position));
    glUniform2iv(particle_program_.camera_resolution, 1, glm::value_ptr(resolution));

    glDrawArrays(GL_POINTS, 0, particles.particles.size());
  }
  glEnable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void Renderer::render_model(const Model &model,
                            const glm::mat4 &parent_transform,
                            const Camera &camera,
                            const Lights &lights,
                            const Environment_lights &environment_lights,
                            const Fog &fog,
                            const glm::vec2 &resolution,
                            const Environment_program &program) {

  if (camera.in_frustum(glm::vec3(parent_transform[3]) + model.centroid(), model.radius())) {

    const glm::mat4 mvp = camera.projection() * camera.view() * parent_transform * model.transform;

    if (model.mesh) {
      glBindVertexArray(vertex_arrays_.at(model.mesh->id()));

      const auto &uniforms = program;

      glActiveTexture(GL_TEXTURE3);
      glBindTexture(GL_TEXTURE_2D, model.material.albedo_map
                                   ? textures_.at(model.material.albedo_map->id()).texture
                                       : black_texture_.texture);

      glActiveTexture(GL_TEXTURE4);
      glBindTexture(GL_TEXTURE_2D, model.material.emission_map
                                       ? textures_.at(model.material.emission_map->id()).texture
                                       : black_texture_.texture);

      static const glm::mat4 bias(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0,
                                  0.5, 0.0, 0.5, 0.5, 0.5, 1.0);

      for (size_t i = 0; i < lights.size(); i++) {
        const glm::mat4 depth_bias_mvp = bias * lights.at(i).camera.projection() *
            lights.at(i).camera.view() * parent_transform *
            model.transform;
        glUniformMatrix4fv(uniforms.depth_bias_mvps.at(i), 1, GL_FALSE,
                           &depth_bias_mvp[0][0]);
      }

      glUniformMatrix4fv(uniforms.model_view_projection, 1, GL_FALSE,
                         &mvp[0][0]);
      auto model_matrix = parent_transform * model.transform;
      glUniformMatrix4fv(uniforms.model_matrix, 1, GL_FALSE, &model_matrix[0][0]);

      glm::mat3 normal_matrix = glm::inverseTranspose(glm::mat3(parent_transform) *
          glm::mat3(model.transform));
      normal_matrix =
          glm::inverseTranspose(glm::mat3(parent_transform * model.transform));
      glUniformMatrix3fv(uniforms.normal_matrix, 1, GL_FALSE, &normal_matrix[0][0]);

      glm::vec4 albedo =
          glm::vec4(model.material.albedo, model.material.albedo_map ? 0.0f : 1.0f);
      glUniform4fv(uniforms.material_albedo, 1,glm::value_ptr(albedo));
      glUniform3fv(uniforms.material_emission, 1, glm::value_ptr(model.material.emission));
      glUniform1fv(uniforms.material_roughness, 1, &model.material.roughness);
      glUniform1fv(uniforms.material_metallic, 1, &model.material.metallic);
      glUniform1fv(uniforms.material_index_of_refraction, 1, &model.material.index_of_refraction);
      glUniform1fv(uniforms.material_alpha, 1, &model.material.alpha);
      glUniform1fv(uniforms.material_transmission, 1, &model.material.transmission);
      glUniform1fv(uniforms.material_ambient_occlusion, 1, &model.material.ambient_occlusion);

      glDrawElements(GL_TRIANGLES, model.mesh->triangles.size() * 3, GL_UNSIGNED_INT, nullptr);
    }
  }

  for (const auto &child : model.models) {
    render_model(child, parent_transform * model.transform, camera, lights,
                 environment_lights, fog, resolution, program);
  }

}

void Renderer::render_model(const Model &model,
                            const glm::mat4 &parent_transform,
                            const Camera &camera,
                            const Lights &lights,
                            const Environment_lights &environment_lights,
                            const Fog &fog,
                            const glm::vec2 &resolution,
                            const Standard_program &program) {

   if (camera.in_frustum(glm::vec3(parent_transform[3]) + model.centroid(), model.radius())) {
    const glm::mat4 mvp = camera.projection() * camera.view() * parent_transform * model.transform;

    if (model.mesh) {
      glBindVertexArray(vertex_arrays_.at(model.mesh->id()));

      const auto &uniforms = program;

      glActiveTexture(GL_TEXTURE7);
      glBindTexture(GL_TEXTURE_2D, model.material.albedo_map
                                   ? textures_.at(model.material.albedo_map->id()).texture
                                       : black_texture_.texture);


      glActiveTexture(GL_TEXTURE8);
      glBindTexture(GL_TEXTURE_2D, model.material.emission_map
                                       ? textures_.at(model.material.emission_map->id()).texture
                                       : black_texture_.texture);

      glActiveTexture(GL_TEXTURE9);
      glBindTexture(GL_TEXTURE_2D, model.material.normal_map
                                   ? textures_.at(model.material.normal_map->id()).texture
                                       : black_texture_.texture);

      glActiveTexture(GL_TEXTURE10);
      glBindTexture(GL_TEXTURE_2D, model.material.metallic_map
                                   ? textures_.at(model.material.metallic_map->id()).texture
                                       : black_texture_.texture);

      glActiveTexture(GL_TEXTURE11);
      glBindTexture(GL_TEXTURE_2D, model.material.roughness_map
                                   ? textures_.at(model.material.roughness_map->id()).texture
                                       : black_texture_.texture);

      glActiveTexture(GL_TEXTURE12);
      glBindTexture(GL_TEXTURE_2D, model.material.ambient_occlusion_map
                                   ? textures_.at(model.material.ambient_occlusion_map->id()).texture
                                       : white_texture_.texture);

      static const glm::mat4 bias(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0,
                                  0.5, 0.0, 0.5, 0.5, 0.5, 1.0);

      for (size_t i = 0; i < lights.size(); i++) {
        auto projection = lights.at(i).camera.projection();
        auto view = lights.at(i).camera.view();
        const glm::mat4 depth_bias_mvp = bias * projection *
            view * parent_transform *
            model.transform;
        glUniformMatrix4fv(uniforms.depth_bias_mvps.at(i), 1, GL_FALSE,
                           &depth_bias_mvp[0][0]);
      }

      glUniformMatrix4fv(uniforms.model_view_projection, 1, GL_FALSE,
                         &mvp[0][0]);
      auto model_matrix = parent_transform * model.transform;
      glUniformMatrix4fv(uniforms.model_matrix, 1, GL_FALSE, &model_matrix[0][0]);

      glm::mat3 normal_matrix = glm::inverseTranspose(glm::mat3(parent_transform) *
          glm::mat3(model.transform));
      normal_matrix =
          glm::inverseTranspose(glm::mat3(parent_transform * model.transform));
      glUniformMatrix3fv(uniforms.normal_matrix, 1, GL_FALSE, &normal_matrix[0][0]);

      glm::vec4 albedo =
          glm::vec4(model.material.albedo, model.material.albedo_map ? 0.0f : 1.0f);
      glUniform4fv(uniforms.material_albedo, 1, glm::value_ptr(albedo));
      glUniform3fv(uniforms.material_emission, 1, glm::value_ptr(model.material.emission));
      glUniform1fv(uniforms.material_roughness, 1, &model.material.roughness);
      glUniform1fv(uniforms.material_metallic, 1, &model.material.metallic);
      glUniform1fv(uniforms.material_index_of_refraction, 1, &model.material.index_of_refraction);
      glUniform1fv(uniforms.material_alpha, 1, &model.material.alpha);
      glUniform1fv(uniforms.material_transmission, 1, &model.material.transmission);
      glUniform1fv(uniforms.material_ambient_occlusion, 1, &model.material.ambient_occlusion);

      glDrawElements(GL_TRIANGLES, model.mesh->triangles.size() * 3, GL_UNSIGNED_INT, nullptr);
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
                    const Post_target &output_target,
                    const float iterations) {
  glViewport(0, 0, GLsizei(output_target.resolution.x), GLsizei(output_target.resolution.y));
  for (int i = 0; i < iterations; i++) {
    GLint horizontal = (i % 2 == 1);
    glBindFramebuffer(GL_FRAMEBUFFER, horizontal ? output_target.frame_buffer : buffer_target.frame_buffer);
    glUseProgram(blur_program_.program);
    glBindVertexArray(quad_.vertex_array);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, i == 0 ? input_texture : horizontal ? buffer_target.texture : output_target.texture);
    glUniform1i(blur_program_.color_sampler, 0);
    glUniform1iv(blur_program_.horizontal, 1, &horizontal);

    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
}

void Renderer::render_shadow_maps(const Models &models, const Lights &lights) {
  for (size_t i = 0; i < shadow_maps_.size(); i++) {
    if (lights.at(i).strength > 0.0f) {
      auto frame_buffer = shadow_maps_.at(i).frame_buffer;
      glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
      glClear(GL_DEPTH_BUFFER_BIT);
      auto resolution = shadow_maps_render_buffer_.resolution;
      glUseProgram(depth_program_.program);
      glViewport(0, 0, resolution, resolution);

      glUniform1i(depth_program_.albedo_sampler, 0);

      for (auto &model : models) {
        render_model_depth(model, glm::mat4(1.0f), lights.at(i).camera, glm::vec2(resolution), depth_program_);
      }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);

      glViewport(0, 0, shadow_maps_render_buffer_.resolution,
                 shadow_maps_render_buffer_.resolution);

      blur(shadow_maps_.at(i).texture, shadow_map_blur_target_, shadow_map_blur_targets_.at(i), 4);
    }
  }
}

void Renderer::render_environment(const Scene &scene, const glm::vec4 &clear_color) {
  for (size_t i = 0; i < environment_maps_targets.size(); i++) {
    if (scene.environment_lights.at(i).strength > 0.0f) {
      GLuint frame_buffer_id = environment_maps_targets.at(i).frame_buffer;
      glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);

      auto texture_id = environment_maps_targets.at(i).texture;

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             GL_TEXTURE_CUBE_MAP_POSITIVE_X + cube_camera_index_.at(i), texture_id, 0);

      glFramebufferTexture2D(GL_FRAMEBUFFER,
                             GL_COLOR_ATTACHMENT1,
                             GL_TEXTURE_CUBE_MAP_POSITIVE_X + cube_camera_index_.at(i),
                             environment_maps_targets.at(i).albedo,
                             0);

      clear(clear_color);
      auto resolution = glm::ivec2(environment_render_buffer_.resolution,
                                   environment_render_buffer_.resolution);
      auto cube_camera = scene.environment_lights.at(i).camera(cube_camera_index_.at(i));

      glViewport(0, 0, resolution.x, resolution.y);

      glUseProgram(environment_program_.program);

      glUniform1i(environment_program_.brdf_lut, 0);
      glUniform1i(environment_program_.shadow_samplers[0], 1);
      glUniform1i(environment_program_.shadow_samplers[1], 2);

      glUniform1i(environment_program_.material_albedo_sampler, 3);
      glUniform1i(environment_program_.material_emission_sampler, 4);

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, brdf_lut_texture_.texture);

      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, shadow_map_blur_targets_[0].texture);

      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, shadow_map_blur_targets_[1].texture);

      // Camera in world space
      auto position = cube_camera.position();
      glUniform3fv(environment_program_.camera_position, 1, glm::value_ptr(position));

      auto near_plane = cube_camera.near_plane();
      glUniform1fv(environment_program_.camera_near, 1, &near_plane);

      auto far_plane = cube_camera.far_plane();
      glUniform1fv(environment_program_.camera_far, 1, &far_plane);

      //TODO: Loop through all lights?
      for (size_t i = 0; i < 2; i++) {
        glUniform3fv(environment_program_.lights.at(i).position, 1,
                     glm::value_ptr(glm::vec3(glm::vec4(scene.lights.at(i).position(), 1.0f))));

        glUniform3fv(environment_program_.lights.at(i).color, 1, glm::value_ptr(scene.lights.at(i).color));
        glUniform1fv(environment_program_.lights.at(i).strength, 1, &scene.lights.at(i).strength);

        auto view = scene.lights.at(i).camera.view();
        glUniformMatrix4fv(environment_program_.lights.at(i).view, 1, GL_FALSE,
                           glm::value_ptr(view));
        auto projection = scene.lights.at(i).camera.projection();
        glUniformMatrix4fv(environment_program_.lights.at(i).projection, 1, GL_FALSE,
                           glm::value_ptr(projection));

        auto light_angle = scene.lights.at(i).angle();
        glUniform1fv(environment_program_.lights.at(i).angle, 1, &light_angle);
        glUniform3fv(environment_program_.lights.at(i).direction, 1, glm::value_ptr(scene.lights.at(i).direction()));
      }

      glUniform2iv(environment_program_.camera_resolution, 1, glm::value_ptr(resolution));

      glUniform3fv(environment_program_.fog_color_near, 1, glm::value_ptr(scene.fog.color_near));
      glUniform3fv(environment_program_.fog_color_far, 1, glm::value_ptr(scene.fog.color_far));
      glUniform1fv(environment_program_.fog_attenuation_factor, 1,
                   &scene.fog.attenuation_factor);

      for (auto &model : scene.models) {
        render_model(model, glm::mat4(1.0f), cube_camera,
                     scene.lights,
                     scene.environment_lights,
                     scene.fog,
                     resolution, environment_program_);
      }

      cube_camera_index_.at(i) = cube_camera_index_.at(i) >= 5 ? 0 : ++cube_camera_index_.at(i); //TODO PROBLEM

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
      if (cube_camera_index_.at(i) == 0) {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
      }
      glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
  }

  int i = cube_camera_index_[0];

  auto cube_camera = scene.environment_lights[0].camera(i);

  glBindFramebuffer(GL_FRAMEBUFFER, propagate_target_.frame_buffer);
  glUseProgram(propagate_program_.program);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, propagate_target_.texture, 0);

  clear(glm::vec4(0.0, 1.0, 0.0, 1.0));
  auto resolution = glm::ivec2(environment_render_buffer_.resolution,
                               environment_render_buffer_.resolution);

  glViewport(0, 0, resolution.x, resolution.y);

  glBindVertexArray(quad_.vertex_array);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, environment_maps_targets[0].texture);
  glUniform1i(propagate_program_.environment_sampler, 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_CUBE_MAP, environment_maps_targets[0].albedo);
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
    unsigned int vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    if (array_buffers_.find(mesh.id()) == array_buffers_.end()) {
      array_buffers_.insert({mesh.id(), Buffer(GL_ARRAY_BUFFER,
                                               mesh.vertices.size() * sizeof(Vertex),
                                               mesh.vertices.data(),
                                               GL_STATIC_DRAW,
                                               mesh.vertices.modified())});
    }
    if (element_array_buffers_.find(mesh.id()) ==
        element_array_buffers_.end()) {
      element_array_buffers_.insert({mesh.id(), Buffer(GL_ELEMENT_ARRAY_BUFFER,
                                                             mesh.triangles.size() * 3 * sizeof(unsigned int),
                                                             mesh.triangles.data(),
                                                             GL_STATIC_DRAW,
                                                             mesh.triangles.modified())});
    }
    glBindBuffer(GL_ARRAY_BUFFER, array_buffers_.at(mesh.id()).id);
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<const void *>(sizeof(glm::vec3)));

    // Tangent
    glVertexAttribPointer(
        2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<const void *>(sizeof(glm::vec3) * 2));

    // UV
    glVertexAttribPointer(
        3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<const void *>(sizeof(glm::vec3) * 3));

    // Weight
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<const void *>(sizeof(glm::vec3) * 3 +
                              sizeof(glm::vec2)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                 element_array_buffers_.at(mesh.id()).id);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glBindVertexArray(0);
    vertex_arrays_.insert({mesh.id(), vertex_array});
  }

  if (mesh.vertices.size() > 0 && mesh.vertices.modified() > array_buffers_.at(mesh.id()).modified) {
    glBindBuffer(GL_ARRAY_BUFFER, array_buffers_.at(mesh.id()).id);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex),
                 mesh.vertices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
  if (mesh.triangles.size() > 0 && mesh.triangles.modified() > element_array_buffers_.at(mesh.id()).modified) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffers_.at(mesh.id()).id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 mesh.triangles.size() * 3 * sizeof(unsigned int),
                 mesh.triangles.data(),
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }
}

void Renderer::unload(const Mesh &mesh) {
  if (vertex_arrays_.find(mesh.id()) != vertex_arrays_.end()) {
    auto va_id = vertex_arrays_.at(mesh.id());
    glDeleteVertexArrays(1, &va_id);
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
      GLuint frame_buffer_id;
      glGenFramebuffers(1, &frame_buffer_id);
      glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);

      //auto buffer = Texture_buffer_2D(*target.texture);

      textures_.insert({target.texture->id(),
                        Texture_buffer_2D(*target.texture)});

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             GL_TEXTURE_2D, textures_.at(target.texture->id()).texture, 0);

      GLuint depthrenderbuffer_id;
      glGenRenderbuffers(1, &depthrenderbuffer_id);
      glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer_id);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                            target.texture->width(),
                            target.texture->height());
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                GL_RENDERBUFFER, depthrenderbuffer_id);
      glBindRenderbuffer(GL_RENDERBUFFER, 0);
      render_buffers.insert({target.target.id(), depthrenderbuffer_id});

      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Framebuffer incomplete.");
      }

      frame_buffers_.insert({target.target.id(), frame_buffer_id});
    }
    auto fb = frame_buffers_.at(target.target.id());
    glBindFramebuffer(GL_FRAMEBUFFER, fb);

    auto texture_id = textures_.at(target.texture->id()).texture;

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, texture_id, 0);

    clear(glm::vec4(0.0f));

    render_scene(target.camera,
                 scene,
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
  if (camera.in_frustum(glm::vec3(transform[3]) + model.centroid(), model.radius())) {
    const glm::mat4 mvp = camera.projection() * camera.view() * transform * model.transform;

    if (model.mesh) {
      glBindVertexArray(vertex_arrays_.at(model.mesh->id()));

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, model.material.albedo_map
                                   ? textures_.at(model.material.albedo_map->id()).texture
                                       : black_texture_.texture);

      glUniformMatrix4fv(program.model_view_projection, 1, GL_FALSE,
                         &mvp[0][0]);
      glm::vec4 albedo =
          glm::vec4(model.material.albedo, model.material.albedo_map ? 0.0f : 1.0f);
      glUniform4fv(program.albedo, 1, glm::value_ptr(albedo));
      glUniform3fv(program.emission, 1, glm::value_ptr(model.material.emission));
      const int num_elements = model.mesh ? model.mesh->triangles.size() * 3 : 0;
      glDrawElements(GL_TRIANGLES, num_elements, GL_UNSIGNED_INT, nullptr);
    }
  }
  for (const auto &child : model.models) {
    render_model_depth(child, transform * model.transform, camera, resolution, program);
  }
}

void Renderer::render(const Scenes &scenes, const glm::vec4 &color, const glm::ivec2 &resolution) {
  for (auto &scene : scenes) {
    load(scene.models);
  }
  render_shadow_maps(scenes[0].models, scenes[0].lights);
  render_environment(scenes[0], color);
  render_texture_targets(scenes[0]);

  glBindFramebuffer(GL_FRAMEBUFFER, screen_target_.frame_buffer);
  clear(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));

  //for (auto &scene : scenes) {
  for (auto & scene : scenes) {
    glBindFramebuffer(GL_FRAMEBUFFER, standard_target_.frame_buffer);
    clear(glm::vec4(0.0f));
    render_scene(scene.camera, scene, resolution);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, standard_target_.frame_buffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, multisample_target_.frame_buffer);
    glBlitFramebuffer(0, 0, resolution.x, resolution.y, 0, 0, resolution.x, resolution.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, standard_target_.frame_buffer);
    render_particles(scene.particle_clouds, scene.lights, scene.camera, resolution);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, standard_target_.frame_buffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, multisample_target_.frame_buffer);
    glBlitFramebuffer(0, 0, resolution.x, resolution.y, 0, 0, resolution.x, resolution.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);

     //TODO: Include clear in blur
    glBindFramebuffer(GL_FRAMEBUFFER, temp_target_.frame_buffer);
    clear(glm::vec4(0.0f));

    glBindFramebuffer(GL_FRAMEBUFFER, post_target_.frame_buffer);
    clear(glm::vec4(0.0f));
    blur(multisample_target_.texture, post_target_, temp_target_);

    glViewport(0, 0, screen_target_.resolution.x, screen_target_.resolution.y);

    glBindFramebuffer(GL_FRAMEBUFFER, screen_target_.frame_buffer);

    glUseProgram(depth_of_field_program_.program);
    glBindVertexArray(quad_.vertex_array);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, multisample_target_.texture);
    glUniform1i(depth_of_field_program_.color_sampler, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, temp_target_.texture);
    glUniform1i(depth_of_field_program_.blurred_color_sampler, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, multisample_target_.depth_texture);
    glUniform1i(depth_of_field_program_.depth_sampler, 2);

    float camera_near = scene.camera.near_plane();
    glUniform1fv(depth_of_field_program_.camera_near, 1, &camera_near);

    float camera_far = scene.camera.far_plane();
    glUniform1fv(depth_of_field_program_.camera_far, 1, &camera_far);

    float camera_focus_distance = glm::distance(scene.camera.position(), scene.camera.center());
    glUniform1fv(depth_of_field_program_.camera_focus_distance, 1, &camera_focus_distance);

    glUniform1fv(depth_of_field_program_.camera_fstop, 1, &scene.camera.fstop);

    glDrawArrays(GL_TRIANGLES, 0, 6);
 }

  // Bloom
  glViewport(0, 0, bloom_target_.resolution.x, bloom_target_.resolution.y);
  glBindFramebuffer(GL_FRAMEBUFFER, bloom_target_.frame_buffer);

  glUseProgram(bloom_program_.program);
  glBindVertexArray(quad_.vertex_array);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, multisample_target_.texture);
  glUniform1i(bloom_program_.color_sampler, 0);

  glDrawArrays(GL_TRIANGLES, 0, 6);

  blur(bloom_target_.texture, post_target_, bloom_target_);

  // Render to screen
  glViewport(0, 0, resolution.x, resolution.y);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //Compositing
  glUseProgram(compositing_program_.program);

  glBindVertexArray(quad_.vertex_array);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, screen_target_.texture);
  //glBindTexture(GL_TEXTURE_2D, multisample_target_.texture);
  glUniform1i(compositing_program_.color_sampler, 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, bloom_target_.texture);
  glUniform1i(compositing_program_.bloom_sampler, 1);

  float strength = 0.1f;
  glUniform1fv(compositing_program_.bloom_strength, 1, &strength);

  glDrawArrays(GL_TRIANGLES, 0, 6);
}

Renderer::Depth_program::Depth_program() {
  std::string name = "depth";
  auto vert_source = text("assets/shaders/" + name + ".vert");
  auto frag_source = text("assets/shaders/" + name + ".frag");

  const auto vertex_shader = Shader(vert_source, GL_VERTEX_SHADER, name);
  const auto fragment_shader = Shader(frag_source, GL_FRAGMENT_SHADER, name);

  glAttachShader(program, vertex_shader.id);
  glAttachShader(program, fragment_shader.id);
  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 3, "uv");
  link(name);
  check(name);
  glDetachShader(program, vertex_shader.id);
  glDetachShader(program, fragment_shader.id);

  model_view_projection = glGetUniformLocation(program, "model_view_projection");
  albedo_sampler = glGetUniformLocation(program, "albedo_sampler");
  albedo = glGetUniformLocation(program, "albedo");
  emission = glGetUniformLocation(program, "emission");
}

Renderer::Environment_program::Environment_program() {
  std::string name = "environment";
  std::string vert_source = text("assets/shaders/" + name + ".vert");
  std::string frag_source = text("assets/shaders/" + name + ".frag");

  std::string functions_name = "functions";
  std::string functions_frag_source = text("assets/shaders/" + functions_name + ".frag");

  const auto vertex_shader = Shader(vert_source, GL_VERTEX_SHADER, name);
  const auto fragment_shader = Shader(frag_source, GL_FRAGMENT_SHADER, name);
  const auto functions_fragment_shader = Shader(functions_frag_source, GL_FRAGMENT_SHADER, functions_name);

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

  model_view_projection = (glGetUniformLocation(program, "model_view_projection"));
  model_matrix = glGetUniformLocation(program, "model");
  normal_matrix = glGetUniformLocation(program, "normal_matrix");
  for (size_t i = 0; i < depth_bias_mvps.size(); i++) {
    depth_bias_mvps.at(i) = glGetUniformLocation(program,
                                              std::string("depth_bias_model_view_projections[" + std::to_string(i)
                                                              + "]").c_str());
  }

  material_albedo_sampler = glGetUniformLocation(program, "material.albedo_sampler");
  material_emission_sampler = glGetUniformLocation(program, "material.emission_sampler");
  material_albedo = glGetUniformLocation(program, "material.albedo");
  material_roughness = glGetUniformLocation(program, "material.roughness");
  material_metallic = glGetUniformLocation(program, "material.metallic");
  material_index_of_refraction = glGetUniformLocation(program, "material.index_of_refraction");
  material_alpha = glGetUniformLocation(program, "material.alpha");
  material_transmission = glGetUniformLocation(program, "material.transmission");
  material_emission = glGetUniformLocation(program, "material.emission");
  material_ambient_occlusion = glGetUniformLocation(program, "material.ambient_occlusion");

  camera_position = glGetUniformLocation(program, "camera.position");
  camera_resolution = glGetUniformLocation(program, "camera.resolution");
  camera_far = glGetUniformLocation(program, "camera.far");
  camera_near = glGetUniformLocation(program, "camera.near");

  for (size_t i = 0; i < lights.size(); i++) {
    lights.at(i).position =
        glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].position").c_str());
    lights.at(i).color = glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].color").c_str());
    lights.at(i).strength =
        glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].strength").c_str());
    lights.at(i).view = glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].view").c_str());
    lights.at(i).projection =
        glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].projection").c_str());
    lights.at(i).angle = glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].angle").c_str());
    lights.at(i).direction =
        glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].direction").c_str());

    shadow_samplers.at(i) = glGetUniformLocation(program, std::string("shadow_maps[" + std::to_string(i) + "]").c_str());
  }

  fog_color_near = glGetUniformLocation(program, "fog.color_near");
  fog_color_far = glGetUniformLocation(program, "fog.color_far");
  fog_attenuation_factor = glGetUniformLocation(program, "fog.attenuation_factor");
  brdf_lut = glGetUniformLocation(program, "brdf_lut");
}

Renderer::Standard_program::Standard_program() {
  std::string name = "standard";
  std::string vert_source = text("assets/shaders/" + name + ".vert");
  std::string frag_source = text("assets/shaders/" + name + ".frag");

  std::string functions_name = "functions";
  std::string functions_frag_source = text("assets/shaders/" + functions_name + ".frag");

  const auto vertex_shader = Shader(vert_source, GL_VERTEX_SHADER, name);
  const auto fragment_shader = Shader(frag_source, GL_FRAGMENT_SHADER, name);
  const auto functions_fragment_shader = Shader(functions_frag_source, GL_FRAGMENT_SHADER, functions_name);

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

  model_view_projection = (glGetUniformLocation(program, "model_view_projection"));
  model_matrix = glGetUniformLocation(program, "model");
  normal_matrix = glGetUniformLocation(program, "normal_matrix");
  for (size_t i = 0; i < depth_bias_mvps.size(); i++) {
    depth_bias_mvps.at(i) = glGetUniformLocation(program,
                                              std::string("depth_bias_model_view_projections[" + std::to_string(i)
                                                              + "]").c_str());
  }

  for (size_t i = 0; i < environment_maps.size(); i++) {
    environment_maps.at(i).map =
        glGetUniformLocation(program, std::string("environment_samplers[" + std::to_string(i) + "]").c_str());
    environment_maps.at(i).position =
        glGetUniformLocation(program, std::string("environments[" + std::to_string(i) + "].position").c_str());
    environment_maps.at(i).extent =
        glGetUniformLocation(program, std::string("environments[" + std::to_string(i) + "].extent").c_str());
    environment_maps.at(i).strength =
        glGetUniformLocation(program, std::string("environments[" + std::to_string(i) + "].strength").c_str());
    environment_maps.at(i).falloff =
        glGetUniformLocation(program, std::string("environments[" + std::to_string(i) + "].falloff").c_str());
  }

  material_albedo_sampler = glGetUniformLocation(program, "material.albedo_sampler");
  material_normal_sampler = glGetUniformLocation(program, "material.normal_sampler");
  material_metallic_sampler = glGetUniformLocation(program, "material.metallic_sampler");
  material_roughness_sampler = glGetUniformLocation(program, "material.roughness_sampler");
  material_emission_sampler = glGetUniformLocation(program, "material.emission_sampler");
  material_ambient_occlusion_sampler = glGetUniformLocation(program, "material.ambient_occlusion_sampler");
  material_albedo = glGetUniformLocation(program, "material.albedo");
  material_roughness = glGetUniformLocation(program, "material.roughness");
  material_metallic = glGetUniformLocation(program, "material.metallic");
  material_index_of_refraction = glGetUniformLocation(program, "material.index_of_refraction");
  material_alpha = glGetUniformLocation(program, "material.alpha");
  material_transmission = glGetUniformLocation(program, "material.transmission");
  material_emission = glGetUniformLocation(program, "material.emission");
  material_ambient_occlusion = glGetUniformLocation(program, "material.ambient_occlusion");

  camera_position = glGetUniformLocation(program, "camera.position");
  camera_resolution = glGetUniformLocation(program, "camera.resolution");

  for (size_t i = 0; i < lights.size(); i++) {
    lights.at(i).position =
        glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].position").c_str());
    lights.at(i).color = glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].color").c_str());
    lights.at(i).strength =
        glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].strength").c_str());
    lights.at(i).view = glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].view").c_str());
    lights.at(i).projection =
        glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].projection").c_str());
    lights.at(i).angle = glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].angle").c_str());
    lights.at(i).direction =
        glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].direction").c_str());

    shadow_maps.at(i) = glGetUniformLocation(program, std::string("shadow_maps[" + std::to_string(i) + "]").c_str());
  }

  fog_color_near = glGetUniformLocation(program, "fog.color_near");
  fog_color_far = glGetUniformLocation(program, "fog.color_far");
  fog_attenuation_factor = glGetUniformLocation(program, "fog.attenuation_factor");
  brdf_lut = glGetUniformLocation(program, "brdf_lut");
}

Renderer::Particle_program::Particle_program() {
  std::string name = "particles";
  std::string vert_source = text("assets/shaders/" + name + ".vert");
  std::string frag_source = text("assets/shaders/" + name + ".frag");
  const auto vertex_shader = Shader(vert_source, GL_VERTEX_SHADER, name);
  const auto fragment_shader = Shader(frag_source, GL_FRAGMENT_SHADER, name);

  glAttachShader(program, vertex_shader.id);
  glAttachShader(program, fragment_shader.id);
  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "color");

  link(name);
  check(name);

  glDetachShader(program, vertex_shader.id);
  glDetachShader(program, fragment_shader.id);

  model_view_projection = glGetUniformLocation(program, "model_view_projection");
  model_view = glGetUniformLocation(program, "model_view");
  projection = glGetUniformLocation(program, "projection");
  texture = glGetUniformLocation(program, "tex");
  resolution = glGetUniformLocation(program, "resolution");
  camera_position = glGetUniformLocation(program, "camera.position");
  camera_resolution = glGetUniformLocation(program, "camera.resolution");

  for (size_t i = 0; i < lights.size(); i++) {
    lights.at(i).position =
        glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].position").c_str());
    lights.at(i).color = glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].color").c_str());
    lights.at(i).strength =
        glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].strength").c_str());
    lights.at(i).view = glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].view").c_str());
    lights.at(i).projection =
        glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].projection").c_str());
    lights.at(i).angle = glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].angle").c_str());
    lights.at(i).direction =
        glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].direction").c_str());

  }
}

Renderer::Box_program::Box_program() {
  std::string name = "box";
  std::string vert_source = text("assets/shaders/" + name + ".vert");
  std::string frag_source = text("assets/shaders/" + name + ".frag");

  const auto vertex_shader = Shader(vert_source, GL_VERTEX_SHADER, name);
  const auto fragment_shader = Shader(frag_source, GL_FRAGMENT_SHADER, name);

  glAttachShader(program, vertex_shader.id);
  glAttachShader(program, fragment_shader.id);
  glBindAttribLocation(program, 0, "position");

  link(name);
  check(name);

  glDetachShader(program, vertex_shader.id);
  glDetachShader(program, fragment_shader.id);

  model_view_projection = glGetUniformLocation(program, "model_view_projection");
}

Renderer::Depth_of_field_program::Depth_of_field_program() {
  std::string name = "depth_of_field";
  auto vert_source = text("assets/shaders/" + name + ".vert");
  auto frag_source = text("assets/shaders/" + name + ".frag");

  const auto vertex_shader = Shader(vert_source, GL_VERTEX_SHADER, name);
  const auto fragment_shader = Shader(frag_source, GL_FRAGMENT_SHADER, name);

  glAttachShader(program, vertex_shader.id);
  glAttachShader(program, fragment_shader.id);
  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "uv");
  link(name);
  check(name);

  glDetachShader(program, vertex_shader.id);
  glDetachShader(program, fragment_shader.id);

  color_sampler = glGetUniformLocation(program, "color_sampler");
  blurred_color_sampler = glGetUniformLocation(program, "blurred_color_sampler");
  depth_sampler = glGetUniformLocation(program, "depth_sampler");

  camera_far = glGetUniformLocation(program, "camera_far");
  camera_near = glGetUniformLocation(program, "camera_near");
  camera_focus_distance = glGetUniformLocation(program, "camera_focus_distance");
  camera_fstop = glGetUniformLocation(program, "camera_fstop");
}

Renderer::Bloom_program::Bloom_program() {
  std::string name = "bloom";
  auto vert_source = text("assets/shaders/" + name + ".vert");
  auto frag_source = text("assets/shaders/" + name + ".frag");

  const auto vertex_shader = Shader(vert_source, GL_VERTEX_SHADER, name);
  const auto fragment_shader = Shader(frag_source, GL_FRAGMENT_SHADER, name);

  glAttachShader(program, vertex_shader.id);
  glAttachShader(program, fragment_shader.id);
  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "uv");
  link(name);
  check(name);

  glDetachShader(program, vertex_shader.id);
  glDetachShader(program, fragment_shader.id);

  color_sampler = glGetUniformLocation(program, "color_sampler");
}

Renderer::Compositing_program::Compositing_program() {
  std::string name = "compositing";
  auto vert_source = text("assets/shaders/" + name + ".vert");
  auto frag_source = text("assets/shaders/" + name + ".frag");

  const auto vertex_shader = Shader(vert_source, GL_VERTEX_SHADER, name);
  const auto fragment_shader = Shader(frag_source, GL_FRAGMENT_SHADER, name);

  glAttachShader(program, vertex_shader.id);
  glAttachShader(program, fragment_shader.id);
  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "uv");
  link(name);
  check(name);

  glDetachShader(program, vertex_shader.id);
  glDetachShader(program, fragment_shader.id);

  color_sampler = glGetUniformLocation(program, "color_sampler");
  bloom_sampler = glGetUniformLocation(program, "bloom_sampler");
  bloom_strength = glGetUniformLocation(program, "strength");

}

Renderer::Blur_program::Blur_program() {
  std::string name = "blur";
  auto vert_source = text("assets/shaders/" + name + ".vert");
  auto frag_source = text("assets/shaders/" + name + ".frag");

  const auto vertex_shader = Shader(vert_source, GL_VERTEX_SHADER, name);
  const auto fragment_shader = Shader(frag_source, GL_FRAGMENT_SHADER, name);

  glAttachShader(program, vertex_shader.id);
  glAttachShader(program, fragment_shader.id);
  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "uv");
  link(name);
  check(name);

  glDetachShader(program, vertex_shader.id);
  glDetachShader(program, fragment_shader.id);
  color_sampler = glGetUniformLocation(program, "color_sampler");
  horizontal = glGetUniformLocation(program, "horizontal");
}

Renderer::Propagate_program::Propagate_program() {
  std::string name = "propagate";
  auto vert_source = text("assets/shaders/" + name + ".vert");
  auto frag_source = text("assets/shaders/" + name + ".frag");

  std::string functions_name = "functions";
  std::string functions_frag_source = text("assets/shaders/" + functions_name + ".frag");

  const auto functions_fragment_shader = Shader(functions_frag_source, GL_FRAGMENT_SHADER, functions_name);
  const auto vertex_shader = Shader(vert_source, GL_VERTEX_SHADER, name);
  const auto fragment_shader = Shader(frag_source, GL_FRAGMENT_SHADER, name);

  glAttachShader(program, functions_fragment_shader.id);
  glAttachShader(program, vertex_shader.id);
  glAttachShader(program, fragment_shader.id);
  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "uv");
  link(name);
  check(name);

  glDetachShader(program, vertex_shader.id);
  glDetachShader(program, fragment_shader.id);
  glDetachShader(program, functions_fragment_shader.id);

  environment_sampler = glGetUniformLocation(program, "environment_map");
  environment_albedo_sampler = glGetUniformLocation(program, "environment_albedo_map");
  side = glGetUniformLocation(program, "side");
}

Renderer::Shadow_map_target::Shadow_map_target(
    const Render_buffer &render_buffer)
    : frame_buffer(generate(glGenFramebuffers)),
      texture(generate(glGenTextures)) {
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexImage2D(GL_TEXTURE_2D, 0,
               GL_RG32F,
               render_buffer.resolution,
               render_buffer.resolution,
               0,
               GL_RG,
               GL_UNSIGNED_BYTE,
               nullptr);

  glBindTexture(GL_TEXTURE_2D, 0);

  glFramebufferTexture2D(GL_FRAMEBUFFER,
                         GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, texture, 0);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                            GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, render_buffer.render_buffer);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("Framebuffer incomplete.");
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
Renderer::Shadow_map_target::~Shadow_map_target() {
  glDeleteFramebuffers(1, &frame_buffer);
  glDeleteTextures(1, &texture);
}

Renderer::Render_buffer::Render_buffer(const int resolution) : resolution(resolution), render_buffer(generate(glGenRenderbuffers)){
  glBindRenderbuffer(GL_RENDERBUFFER, render_buffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                        resolution,
                        resolution);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
}
Renderer::Render_buffer::~Render_buffer() {
  glDeleteRenderbuffers(1, &render_buffer);
}
Renderer::Environment_map_target::Environment_map_target(
    const Renderer::Render_buffer &render_buffer)
    : frame_buffer(generate(glGenFramebuffers)),
      texture(generate(glGenTextures)), albedo(generate(glGenTextures)) {
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

  glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
                  GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
                  GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
                  GL_CLAMP_TO_EDGE);

  for (int i = 0; i < 6; i++) {
    std::vector<unsigned char> data;
    for (int i = 0; i < render_buffer.resolution * render_buffer.resolution; i++) {
      data.push_back(0);
      data.push_back(0);
      data.push_back(0);
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                 GL_RGB16F,
                 render_buffer.resolution, render_buffer.resolution, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data.data());
  }
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_CUBE_MAP_POSITIVE_X, texture, 0);

  //Albedo target
  glBindTexture(GL_TEXTURE_CUBE_MAP, albedo);
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
                  GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
                  GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
                  GL_CLAMP_TO_EDGE);

  for (int i = 0; i < 6; i++) {
    std::vector<unsigned char> data;
    for (int i = 0; i < render_buffer.resolution * render_buffer.resolution; i++) {
      data.push_back(0);
      data.push_back(0);
      data.push_back(0);
    }
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                 GL_RGB,
                 render_buffer.resolution, render_buffer.resolution, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data.data());
  }
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                         GL_TEXTURE_CUBE_MAP_POSITIVE_X, albedo, 0);

  std::array<GLuint, 2> attachments{GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
  glDrawBuffers(2, attachments.data());

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, render_buffer.render_buffer);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("Framebuffer incomplete.");
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
Renderer::Environment_map_target::~Environment_map_target() {
  glDeleteTextures(1, &texture);
  glDeleteFramebuffers(1, &frame_buffer);
}

Renderer::Standard_target::Standard_target(const glm::ivec2 &resolution)
    : frame_buffer(generate(glGenFramebuffers)),
      color_texture(generate(glGenTextures)),
      depth_texture(generate(glGenTextures)) {
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, color_texture);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, resolution.x, resolution.y, GL_TRUE);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, color_texture, 0);

  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depth_texture);
  glTexStorage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_DEPTH_COMPONENT24, resolution.x, resolution.y, true);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER,
                         GL_DEPTH_ATTACHMENT,
                         GL_TEXTURE_2D_MULTISAMPLE,
                         depth_texture,
                         0);


  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("Framebuffer incomplete.");
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
Renderer::Standard_target::~Standard_target() {
  glDeleteFramebuffers(1, &frame_buffer);
  glDeleteTextures(1, &color_texture);
  glDeleteTextures(1, &depth_texture);
}

Renderer::Post_target::Post_target(const glm::ivec2 &resolution,
                                   const GLint precision)
    : resolution(resolution),
      frame_buffer(generate(glGenFramebuffers)),
      texture(generate(glGenTextures)) {
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, precision, resolution.x, resolution.y, 0, GL_RGBA, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_2D, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("Framebuffer incomplete");
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
Renderer::Post_target::~Post_target() {
  glDeleteFramebuffers(1, &frame_buffer);
  glDeleteTextures(1, &texture);
}

Renderer::Blit_target::Blit_target(const glm::ivec2 &resolution,
                                   const GLint precision)
    : resolution(resolution), frame_buffer(generate(glGenFramebuffers)),
      texture(generate(glGenTextures)), depth_texture(generate(glGenTextures)) {
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, resolution.x, resolution.y, 0, GL_RGBA, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_2D, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

  glBindTexture(GL_TEXTURE_2D, depth_texture);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT24, resolution.x, resolution.y);
  glBindTexture(GL_TEXTURE_2D, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER,
                         GL_DEPTH_ATTACHMENT,
                         GL_TEXTURE_2D,
                         depth_texture,
                         0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("Framebuffer incomplete");
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
Renderer::Blit_target::~Blit_target() {
  glDeleteFramebuffers(1, &frame_buffer);
  glDeleteTextures(1, &texture);
  glDeleteTextures(1, &depth_texture);
}

Renderer::Quad::Quad()
    : vertex_array(generate(glGenVertexArrays)),
      buffer(generate(glGenBuffers)) {
  static const float quad_vertices[] = {
      -1.0f, 1.0f, 0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f, 0.0f,
      1.0f, -1.0f, 1.0f, 0.0f,
      -1.0f, 1.0f, 0.0f, 1.0f,
      1.0f, -1.0f, 1.0f, 0.0f,
      1.0f, 1.0f, 1.0f, 1.0f
  };
  glBindVertexArray(vertex_array);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) nullptr);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
Renderer::Quad::~Quad() {
  glDeleteBuffers(1, &buffer);
  glDeleteVertexArrays(1, &vertex_array);
}
Renderer::Box::Box()
    : buffer(generate(glGenBuffers)), element_buffer(generate(glGenBuffers)),
      vertex_array(generate(glGenVertexArrays)) {
  // Render boxes
  float vertices[] = {
      -0.5, -0.5, -0.5, 1.0, 0.5, -0.5, -0.5, 1.0, 0.5, 0.5, -0.5,
      1.0, -0.5, 0.5, -0.5, 1.0, -0.5, -0.5, 0.5, 1.0, 0.5, -0.5,
      0.5, 1.0, 0.5, 0.5, 0.5, 1.0, -0.5, 0.5, 0.5, 1.0,
  };

  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  unsigned int elements[] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 4, 1, 5, 2, 6, 3, 7};

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glBindVertexArray(vertex_array);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, // attribute
                        4, // number of elements per vertex, here (x,y,z,w)
                        GL_FLOAT, // the type of each element
                        GL_FALSE, // take our values as-is
                        0,        // no extra data between each position
                        nullptr         // offset of first element
  );
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
  glBindVertexArray(0);
}

Renderer::Box::~Box(){
  glDeleteBuffers(1, &buffer);
  glDeleteVertexArrays(1, &vertex_array);
  glDeleteBuffers(1, &element_buffer);
}

Renderer::Buffer::Buffer(GLenum type, GLsizeiptr size, const void *data,
                         GLenum hint, Time_point modified)
    : id(generate(glGenBuffers)), modified(modified) {
  glBindBuffer(type, id);
  glBufferData(type, size,
              data, hint);
  glBindBuffer(type, 0);
}

Renderer::Buffer::Buffer(Renderer::Buffer &&buffer): id(buffer.id){
  buffer.id = 0;
}

Renderer::Buffer &Renderer::Buffer::operator=(Renderer::Buffer &&buffer) {
  if(this != &buffer){
    release();
    std::swap(id, buffer.id);
  }
  return *this;
}

Renderer::Buffer::~Buffer() {
  release();
}

void Renderer::Buffer::release() {
  glDeleteBuffers(1, &id);
  id = 0;
}


Renderer::Texture_buffer_2D::Texture_buffer_2D(
    const GLint internal_format, const GLenum external_format, const int width,
    const int height, const GLint filter_min, const GLint filter_mag,
    const GLint wrap, const void *data, const Time_point &modified)
    : texture(generate(glGenTextures)), modified(modified) {
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_min);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_mag);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

  glTexImage2D(GL_TEXTURE_2D, 0,
               internal_format,
               width, height, 0,
               external_format, GL_UNSIGNED_BYTE, data);
  if (filter_min == GL_LINEAR_MIPMAP_LINEAR || filter_min == GL_NEAREST_MIPMAP_LINEAR) {
    glGenerateMipmap(GL_TEXTURE_2D);
  };
  glBindTexture(GL_TEXTURE_2D, 0);
}

Renderer::Texture_buffer_2D::~Texture_buffer_2D() {
  glDeleteTextures(1, &texture);
}

Renderer::Texture_buffer_2D::Texture_buffer_2D(Renderer::Texture_buffer_2D &&buffer) : texture(buffer.texture) {
  buffer.texture = 0;
}

Renderer::Texture_buffer_2D &Renderer::Texture_buffer_2D::operator=(Renderer::Texture_buffer_2D &&buffer) {
  if (this != &buffer){
    release();
    std::swap(texture, buffer.texture);
  }
  return  *this;
}

void Renderer::Texture_buffer_2D::release(){
  glDeleteTextures(1, &texture);
  texture = 0;
}

Renderer::Texture_buffer_2D::Texture_buffer_2D(const Texture_2D &texture_2d)
    : Texture_buffer_2D(format_convert(texture_2d.format).internal_format,
                        format_convert(texture_2d.format).format,
                        texture_2d.width(), texture_2d.height(),
                        texture_2d.mipmaps ? filter_convert_mip(texture_2d.filter) : filter_convert(texture_2d.filter),
                        filter_convert(texture_2d.filter),
                        wrap_convert(texture_2d.wrap),
                        texture_2d.layers[0].data()) {}

Renderer::Shader::Shader(const std::string &source,
                         const GLuint type,
                         const std::string &name): id(glCreateShader(type)) {

  static const std::map<const unsigned int, std::string> shader_types{
      {GL_VERTEX_SHADER, "vertex shader"},
      {GL_FRAGMENT_SHADER, "fragment shader"},
      {GL_GEOMETRY_SHADER, "geometry shader"}};

  auto const *chars = source.c_str();
  std::cout << "Compiling: " << (!name.empty() ? name + " " : "") << shader_types.at(type) << std::endl;
  glShaderSource(id, 1, &chars, nullptr);
  glCompileShader(id);

  assert(id);

  GLint status;
  glGetShaderiv(id, GL_COMPILE_STATUS, &status);

  if (status == GL_FALSE) {
    int length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    std::vector<char> buffer(length);
    glGetShaderInfoLog(id, length, nullptr, &buffer[0]);
    std::cerr << "Compile failure in:  " << (!name.empty() ? name + " " : "") << shader_types.at(type) << std::endl;
    std::cerr << std::string(buffer.begin(), buffer.end()) << std::endl;
  }
  assert(status);
}
Renderer::Shader::~Shader() {
  glDeleteShader(id);
}
Renderer::Program::Program() : program(glCreateProgram()) {
  assert(program);
}
Renderer::Program::~Program() {
  glDeleteProgram(program);
}
void Renderer::Program::check(const std::string &name) {
  GLint status = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    GLint length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    std::vector<char> buffer(length);
    glGetProgramInfoLog(program, length, nullptr, &buffer[0]);
    std::cout << "Link failure in: " << name << std::endl;
    std::cout << std::string(buffer.begin(), buffer.end()) << std::endl;
  }
  assert(status);
}
void Renderer::Program::link(const std::string &name) {
  std::cout << "Linking: " + name + " program." << std::endl;
  glLinkProgram(program);
}
}
}
