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

namespace mos {
namespace gfx {

const std::map<const unsigned int, std::string> Renderer::shader_types_{
    {GL_VERTEX_SHADER, "vertex shader"},
    {GL_FRAGMENT_SHADER, "fragment shader"},
    {GL_GEOMETRY_SHADER, "geometry shader"}};

GLuint wrap_convert(const Texture::Wrap & wrap){
  const std::map<Texture::Wrap, GLuint> wrap_map{
      {Texture::Wrap::CLAMP, GL_CLAMP_TO_EDGE},
      {Texture::Wrap::REPEAT, GL_REPEAT}};
  return wrap_map.at(wrap);
}

struct FormatPair {
  GLuint internal_format;
  GLuint format;
};

FormatPair format_convert(const Texture::Format &format) {
  const std::map<Texture::Format, FormatPair> format_map {
      {Texture::Format::R, {GL_RED, GL_RED}},
      {Texture::Format::RG, {GL_RG, GL_RG}},
      {Texture::Format::SRGB, {GL_SRGB, GL_RGB}},
      {Texture::Format::SRGBA, {GL_SRGB_ALPHA, GL_RGBA}},
      {Texture::Format::RGB, {GL_RGB, GL_RGB}},
      {Texture::Format::RGBA, {GL_RGBA, GL_RGBA}}};
  return format_map.at(format);
}

Renderer::Renderer(const glm::vec4 &color, const glm::ivec2 &resolution) :
    cube_camera_index_({0, 0}),
    standard_target_(resolution),
    multi_target_(resolution),
    blur_target0_(resolution / 4),
    blur_target1_(resolution / 4),
    shadow_maps_render_buffer_(512),
    shadow_maps_{ShadowMapTarget(shadow_maps_render_buffer_),
                 ShadowMapTarget(shadow_maps_render_buffer_)},
    environment_render_buffer_(128),
    environment_maps_targets{EnvironmentMapTarget(environment_render_buffer_),
                             EnvironmentMapTarget(environment_render_buffer_)},
    quad_(),
    black_texture_(GL_RGBA, GL_RGBA, 1, 1, GL_NEAREST, GL_REPEAT, std::array<unsigned char, 4>{0, 0, 0, 0}.data()),
    white_texture_(GL_RGBA, GL_RGBA, 1, 1, GL_NEAREST, GL_REPEAT, std::array<unsigned char, 4>{255, 255, 255, 255}.data())
{

  if (!gladLoadGL()) {
    printf("No valid OpenGL context.\n");
    exit(-1);
  }

  fprintf(stdout, "Status: OpenGL version: %s\n", glGetString(GL_VERSION));
  fprintf(stdout, "Max uniform locations: %s\n",
          glGetString(GL_MAX_ARRAY_TEXTURE_LAYERS));

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
  glEnable(GL_FRAMEBUFFER_SRGB);

  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

  glDepthFunc(GL_LEQUAL);
  glDepthMask(GL_TRUE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  clear(color);

  auto brdf_lut_texture = Texture2D("assets/brdfLUT.png", false);
  brdf_lut_texture.format = Texture::Format::RGB;
  brdf_lut_texture.wrap = Texture::Wrap::CLAMP;
  brdf_lut_texture_ = create_texture(brdf_lut_texture);
}

Renderer::~Renderer() {
  for (auto &fb : frame_buffers_) {
    glDeleteFramebuffers(1, &fb.second);
  }

  for (auto &rb : render_buffers) {
    glDeleteRenderbuffers(1, &rb.second);
  }

  for (auto &pb : pixel_buffers_) {
    glDeleteBuffers(1, &pb.second);
  }

  for (auto &t : textures_) {
    glDeleteTextures(1, &t.second.id);
  }

  for (auto &ab : array_buffers_) {
    glDeleteBuffers(1, &ab.second.id);
  }

  for (auto &eab : element_array_buffers_) {
    glDeleteBuffers(1, &eab.second.id);
  }

  for (auto &va : vertex_arrays_) {
    glDeleteVertexArrays(1, &va.second);
  }
}

void Renderer::load(const Model &model) {
  load(model.mesh);
  load(model.material.albedo_map);
  load(model.material.emission_map);
  load(model.material.normal_map);
  load(model.material.metallic_map);
  load(model.material.roughness_map);
  load(model.material.ambient_occlusion_map);
  for (auto &m : model.models) {
    load(m);
  }
}

void Renderer::unload(const Model &model) {
  unload(model.mesh);
  unload(model.material.albedo_map);
  unload(model.material.emission_map);
  unload(model.material.normal_map);
  unload(model.material.metallic_map);
  unload(model.material.roughness_map);
  unload(model.material.ambient_occlusion_map);
  for (auto &m : model.models) {
    unload(m);
  }
}

void Renderer::load_or_update(const Texture2D &texture) {
  if (textures_.find(texture.id()) == textures_.end()) {
    GLuint gl_id = create_texture(texture);
    textures_.insert({texture.id(), Buffer{gl_id, texture.layers.modified()}});
  } else {
    auto buffer = textures_.at(texture.id());
    if (texture.layers.modified() > buffer.modified) {
      glBindTexture(GL_TEXTURE_2D, buffer.id);
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

void Renderer::load(const SharedTexture2D &texture) {
  if (texture) {
    load_or_update(*texture);
  }
}

void Renderer::unload(const SharedTexture2D &texture) {
  if (texture) {
    if (textures_.find(texture->id()) != textures_.end()) {
      auto gl_id = textures_.at(texture->id()).id;
      glDeleteTextures(1, &gl_id);
      textures_.erase(texture->id());
    }
  }
}

void Renderer::clear_buffers() {
  for (auto &texture : textures_) {
    glDeleteTextures(1, &texture.second.id);
  }
  textures_.clear();

  for (auto &ab : array_buffers_) {
    glDeleteBuffers(1, &ab.second.id);
  }
  array_buffers_.clear();

  for (auto &eab : element_array_buffers_) {
    glDeleteBuffers(1, &eab.second.id);
  }
  element_array_buffers_.clear();
}

unsigned int Renderer::create_shader(const std::string &source,
                                     const unsigned int type,
                                     const std::string &name) {
  auto const *chars = source.c_str();
  auto id = glCreateShader(type);

  std::cout << "Compiling: " << (!name.empty() ? name + " " : "") << shader_types_.at(type) << std::endl;
  glShaderSource(id, 1, &chars, NULL);
  glCompileShader(id);
  return id;
}

bool Renderer::check_shader(const unsigned int shader,
                            const std::string &name) {
  if (!shader) {
    return false;
  }
  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

  GLint type;
  glGetShaderiv(shader, GL_SHADER_TYPE, &type);

  if (status == GL_FALSE) {
    int length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    if (length > 0) {
      std::vector<char> buffer(length);
      glGetShaderInfoLog(shader, length, NULL, &buffer[0]);
      std::cerr << "Compile failure in:  " << (!name.empty() ? name + " " : "") << shader_types_.at(type) << std::endl;
      std::cerr << std::string(buffer.begin(), buffer.end()) << std::endl;
    }
    return false;
  }
  return true;
}

bool Renderer::check_program(const unsigned int program, const std::string &name = "") {
  if (!program) {
    return false;
  }

  GLint status;
  glGetShaderiv(program, GL_LINK_STATUS, &status);

  if (status == GL_FALSE) {
    int length;
    glGetShaderiv(program, GL_INFO_LOG_LENGTH, &length);
    if (length > 0) {
      std::vector<char> buffer(length);
      glGetShaderInfoLog(program, length, NULL, &buffer[0]);
      std::cerr << "Link failure in" + name + " program" << std::endl;
      std::cerr << std::string(buffer.begin(), buffer.end()) << std::endl;
    }
    return false;
  }
  return true;
}

GLuint Renderer::create_texture(const Texture2D &texture) {
  GLuint id;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  GLfloat sampling = texture.mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampling);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampling);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_convert(texture.wrap));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_convert(texture.wrap));

  glTexImage2D(GL_TEXTURE_2D, 0,
               format_convert(texture.format).internal_format,
               texture.width(), texture.height(), 0,
               format_convert(texture.format).format,
               GL_UNSIGNED_BYTE, texture.layers[0].data());

  if (texture.mipmaps) {
    glGenerateMipmap(GL_TEXTURE_2D);
  };
  glBindTexture(GL_TEXTURE_2D, 0);
  return id;
}

GLuint Renderer::create_texture(const SharedTexture2D &texture) {
  return create_texture(*texture);
}

void Renderer::render_scene(const Camera &camera,
                            const Scene &scene,
                            const glm::vec2 &resolution) {
  glViewport(0, 0, resolution.x, resolution.y);
  glUseProgram(standard_program_.program);
  glUniform1i(standard_program_.brdf_lut, 0);
  glUniform1i(standard_program_.shadow_maps[0], 1);
  glUniform1i(standard_program_.shadow_maps[1], 2);
  glUniform1i(standard_program_.environment_maps[0].map, 3);
  glUniform1i(standard_program_.environment_maps[1].map, 4);

  glUniform1i(standard_program_.material_albedo_map, 5);
  glUniform1i(standard_program_.material_emission_map, 6);
  glUniform1i(standard_program_.material_normal_map, 7);
  glUniform1i(standard_program_.material_metallic_map, 8);
  glUniform1i(standard_program_.material_roughness_map, 9);
  glUniform1i(standard_program_.material_ambient_occlusion_map, 10);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, brdf_lut_texture_);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, shadow_maps_[0].texture);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, shadow_maps_[1].texture);

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_CUBE_MAP, environment_maps_targets[0].texture);

  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_CUBE_MAP, environment_maps_targets[1].texture);

  for (int i = 0; i < scene.environment_lights.size(); i++) {
    glUniform3fv(standard_program_.environment_maps[i].position, 1,
                 glm::value_ptr(scene.environment_lights[i].box_.position()));
    glUniform3fv(standard_program_.environment_maps[i].extent, 1,
                 glm::value_ptr(scene.environment_lights[i].box_.extent));
    glUniform1fv(standard_program_.environment_maps[i].strength, 1,
                 &scene.environment_lights[i].strength);
  }

  // Camera in world space
  auto position = camera.position();
  glUniform3fv(standard_program_.camera_position, 1, glm::value_ptr(position));

  for (int i = 0; i < scene.lights.size(); i++) {
    glUniform3fv(standard_program_.lights[i].position, 1,
                 glm::value_ptr(glm::vec3(glm::vec4(scene.lights[i].position(), 1.0f))));
    auto light_color =
        scene.lights[i].color * scene.lights[i].strength
            / 11.5f; // 11.5 divider is for same light strength as in Blender/cycles.
    glUniform3fv(standard_program_.lights[i].color, 1, glm::value_ptr(light_color));

    glUniformMatrix4fv(standard_program_.lights[i].view, 1, GL_FALSE,
                       &scene.lights[i].camera.view[0][0]);
    glUniformMatrix4fv(standard_program_.lights[i].projection, 1, GL_FALSE,
                       &scene.lights[i].camera.projection[0][0]);

    auto light_angle = scene.lights[i].angle();
    glUniform1fv(standard_program_.lights[i].angle, 1, &light_angle);
    glUniform3fv(standard_program_.lights[i].direction, 1, glm::value_ptr(scene.lights[i].direction()));
  }

  glUniform2fv(standard_program_.camera_resolution, 1, glm::value_ptr(resolution));

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
  render_particles(scene.particle_clouds, camera, resolution);
}

void Renderer::render_boxes(const Boxes &boxes, const mos::gfx::Camera &camera) {

  glUseProgram(box_program_.program);
  glBindVertexArray(box.vertex_array);

  for (auto &box : boxes) {
    glm::vec3 size = box.size();
    glm::mat4 transform = box.transform;
    glm::mat4 mv = camera.view * transform * glm::scale(glm::mat4(1.0f), size);
    glm::mat4 mvp = camera.projection * camera.view * transform *
        glm::scale(glm::mat4(1.0f), size);

    glUniformMatrix4fv(box_program_.mvp, 1, GL_FALSE, &mvp[0][0]);

    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT,
                   (GLvoid *) (4 * sizeof(GLuint)));
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT,
                   (GLvoid *) (8 * sizeof(GLuint)));
  }
  glBindVertexArray(0);
}

void Renderer::render_particles(const ParticleClouds &clouds,
                                const mos::gfx::Camera &camera,
                                const glm::vec2 &resolution) {
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
        array_buffers_.insert({particles.id(), Buffer{array_buffer, particles.particles.modified()}});
      }
      glBindBuffer(GL_ARRAY_BUFFER, array_buffers_.at(particles.id()).id);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);
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

    glm::mat4 mv = camera.view;
    glm::mat4 mvp = camera.projection * camera.view;

    glUseProgram(particle_program_.program);

    glBindVertexArray(vertex_arrays_.at(particles.id()));

    load(particles.emission_map);
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, particles.emission_map
                                 ? textures_.at(particles.emission_map->id()).id
                                 : black_texture_.texture);
    glUniform1i(particle_program_.texture, 10);

    glUniformMatrix4fv(particle_program_.mvp, 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(particle_program_.mv, 1, GL_FALSE, &mv[0][0]);
    glUniformMatrix4fv(particle_program_.p, 1, GL_FALSE, &camera.projection[0][0]);
    glUniform2fv(particle_program_.resolution, 1, glm::value_ptr(resolution));

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDrawArrays(GL_POINTS, 0, particles.particles.size());
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
}

void Renderer::render_model(const Model &model,
                            const glm::mat4 &parent_transform,
                            const Camera &camera,
                            const Lights &lights,
                            const EnvironmentLights &environment_lights,
                            const Fog &fog,
                            const glm::vec2 &resolution,
                            const StandardProgram &program) {

  const glm::mat4 mvp = camera.projection * camera.view * parent_transform * model.transform;

  if (model.mesh) {
    glBindVertexArray(vertex_arrays_.at(model.mesh->id()));

    const auto &uniforms = program;

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, model.material.albedo_map
                                 ? textures_.at(model.material.albedo_map->id()).id
                                 : black_texture_.texture);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, model.material.emission_map
                                 ? textures_.at(model.material.emission_map->id()).id
                                 : black_texture_.texture);

    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, model.material.normal_map
                                 ? textures_.at(model.material.normal_map->id()).id
                                 : black_texture_.texture);

    glActiveTexture(GL_TEXTURE8);
    glBindTexture(GL_TEXTURE_2D, model.material.metallic_map
                                 ? textures_.at(model.material.metallic_map->id()).id
                                 : black_texture_.texture);

    glActiveTexture(GL_TEXTURE9);
    glBindTexture(GL_TEXTURE_2D, model.material.roughness_map
                                 ? textures_.at(model.material.roughness_map->id()).id
                                 : black_texture_.texture);

    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, model.material.ambient_occlusion_map
                                 ? textures_.at(model.material.ambient_occlusion_map->id()).id
                                 : white_texture_.texture);

    static const glm::mat4 bias(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0,
                                0.5, 0.0, 0.5, 0.5, 0.5, 1.0);

    for (int i = 0; i < lights.size(); i++) {
      const glm::mat4 depth_bias_mvp = bias * lights[i].camera.projection *
          lights[i].camera.view * parent_transform *
          model.transform;
      glUniformMatrix4fv(uniforms.depth_bias_mvps[i], 1, GL_FALSE,
                         &depth_bias_mvp[0][0]);
    }

    glUniformMatrix4fv(uniforms.model_view_projection_matrix, 1, GL_FALSE,
                       &mvp[0][0]);
    auto model_matrix = parent_transform * model.transform;
    glUniformMatrix4fv(uniforms.model_matrix, 1, GL_FALSE, &model_matrix[0][0]);

    glm::mat3 normal_matrix = glm::inverseTranspose(glm::mat3(parent_transform) *
        glm::mat3(model.transform));
    normal_matrix =
        glm::inverseTranspose(glm::mat3(parent_transform * model.transform));
    glUniformMatrix3fv(uniforms.normal_matrix, 1, GL_FALSE, &normal_matrix[0][0]);

    glm::vec4 albedo =
        glm::vec4(model.material.albedo, model.material.albedo_map || model.material.emission_map ? 0.0f : 1.0f);
    glUniform4fv(uniforms.material_albedo, 1,
                 glm::value_ptr(albedo));
    glm::vec4 emission =
        glm::vec4(model.material.emission, model.material.emission_map || model.material.albedo_map ? 0.0f : 1.0f);
    glUniform4fv(uniforms.material_emission, 1,
                 glm::value_ptr(emission));
    glUniform1fv(uniforms.material_roughness, 1,
                 &model.material.roughness);
    glUniform1fv(uniforms.material_metallic, 1,
                 &model.material.metallic);
    glUniform1fv(uniforms.material_opacity, 1, &model.material.opacity);

    glUniform1fv(uniforms.material_emission_strength, 1, &model.material.emission_strength);
    glUniform1fv(uniforms.material_ambient_occlusion, 1, &model.material.ambient_occlusion);
    glUniform3fv(uniforms.material_factor, 1, glm::value_ptr(model.material.factor));

    glDrawElements(GL_TRIANGLES, model.mesh->triangles.size() * 3, GL_UNSIGNED_INT, 0);
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

void Renderer::render_shadow_maps(const Models &models, const Lights &lights) {
  for (int i = 0; i < shadow_maps_.size(); i++) {
    auto frame_buffer = shadow_maps_[i].frame_buffer;
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
    glClear(GL_DEPTH_BUFFER_BIT);
    auto resolution = glm::ivec2(512, 512);
    glUseProgram(depth_program_.program);
    glViewport(0, 0, resolution.x, resolution.y);
    for (auto &model : models) {
      render_model_depth(model, glm::mat4(1.0f), lights[i].camera, resolution, depth_program_);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //Generate mipmaps
    glBindTexture(GL_TEXTURE_2D, shadow_maps_[i].texture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
}

void Renderer::render_environment(const Scene &scene, const glm::vec4 &clear_color) {
  for (int i = 0; i < environment_maps_targets.size(); i++) {
    GLuint frame_buffer_id = environment_maps_targets[i].frame_buffer;
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);

    auto texture_id = environment_maps_targets[i].texture;

    auto cube_camera = scene.environment_lights[i].cube_camera_.cameras[cube_camera_index_[i]];

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_CUBE_MAP_POSITIVE_X + cube_camera_index_[i], texture_id, 0);
    clear_depth();
    auto resolution = glm::vec2(environment_render_buffer_.resolution,
                                environment_render_buffer_.resolution);
    render_scene(cube_camera, scene, resolution);

    cube_camera_index_[i] = cube_camera_index_[i] >= 5 ? 0 : ++cube_camera_index_[i]; //TODO PROBLEM

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
    if (cube_camera_index_[i] == 0) {
      glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  }
}
void Renderer::load(const Mesh &mesh) {
  if (vertex_arrays_.find(mesh.id()) == vertex_arrays_.end()) {
    unsigned int vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    if (array_buffers_.find(mesh.id()) == array_buffers_.end()) {
      unsigned int array_buffer_id;
      glGenBuffers(1, &array_buffer_id);
      glBindBuffer(GL_ARRAY_BUFFER, array_buffer_id);
      glBufferData(GL_ARRAY_BUFFER,
                   mesh.vertices.size() * sizeof(Vertex),
                   mesh.vertices.data(), GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      array_buffers_.insert({mesh.id(), Buffer{array_buffer_id, mesh.vertices.modified()}});
    }
    if (element_array_buffers_.find(mesh.id()) ==
        element_array_buffers_.end()) {
      unsigned int element_array_buffer_id;
      glGenBuffers(1, &element_array_buffer_id);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer_id);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                   mesh.triangles.size() * 3 * sizeof(unsigned int),
                   mesh.triangles.data(), GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      element_array_buffers_.insert({mesh.id(), Buffer{element_array_buffer_id, mesh.triangles.modified()}});
    }
    glBindBuffer(GL_ARRAY_BUFFER, array_buffers_.at(mesh.id()).id);
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

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
      auto abo = array_buffers_.at(mesh.id());
      glDeleteBuffers(1, &abo.id);
      array_buffers_.erase(mesh.id());
    }
    if (element_array_buffers_.find(mesh.id()) !=
        element_array_buffers_.end()) {
      auto ebo = element_array_buffers_.at(mesh.id());
      glDeleteBuffers(1, &ebo.id);
      element_array_buffers_.erase(mesh.id());
    }
  }
}

void Renderer::load(const SharedMesh &mesh) {
  if (mesh) {
    load(*mesh);
  }
}

void Renderer::unload(const SharedMesh &mesh) {
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

      GLuint texture_id = create_texture(target.texture);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             GL_TEXTURE_2D, texture_id, 0);
      textures_.insert({target.texture->id(),
                        Buffer{texture_id, target.texture->layers.modified()}});

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

    auto texture_id = textures_.at(target.texture->id()).id;

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
                                  const DepthProgram &program) {
  const glm::mat4 mvp = camera.projection * camera.view * transform * model.transform;

  if (model.mesh) {
    glBindVertexArray(vertex_arrays_.at(model.mesh->id()));
    glUniformMatrix4fv(program.model_view_projection_matrix, 1, GL_FALSE,
                       &mvp[0][0]);
    const int num_elements = model.mesh ? model.mesh->triangles.size() * 3 : 0;
    glDrawElements(GL_TRIANGLES, num_elements, GL_UNSIGNED_INT, 0);
  }
  for (const auto &child : model.models) {
    render_model_depth(child, transform * model.transform, camera, resolution, program);
  }
}

void Renderer::link_program(const GLuint program, const std::string &name = "") {
  std::cout << "Linking " + name + " program." << std::endl;
  glLinkProgram(program);
}
void Renderer::render(const Scenes &scenes, const glm::vec4 &color, const glm::ivec2 &resolution) {
  for (auto &scene : scenes) {
    load(scene.models);
  }
  //for (auto it = scenes.begin(); it != scenes.end(); it++) {
  render_shadow_maps(scenes[0].models, scenes[0].lights);
  render_environment(scenes[0], color);
  render_texture_targets(scenes[0]);
  //}
  glBindFramebuffer(GL_FRAMEBUFFER, standard_target_.frame_buffer);
  clear(color);

  for (auto it = scenes.begin(); it != scenes.end(); it++) {
    render_scene(it->camera, *it, resolution);
  }

  //RenderQuad
  glBindFramebuffer(GL_FRAMEBUFFER, multi_target_.frame_buffer);
  glUseProgram(multisample_program_.program);

  glBindVertexArray(quad_.vertex_array);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, standard_target_.texture);
  glUniform1i(multisample_program_.color_texture, 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, standard_target_.depth_texture);
  glUniform1i(multisample_program_.depth_texture, 1);

  glDrawArrays(GL_TRIANGLES, 0, 6);

  glViewport(0, 0, resolution.x / 4.0f, resolution.y / 4.0f);

  glBindFramebuffer(GL_FRAMEBUFFER, blur_target0_.frame_buffer);
  glUseProgram(blur_program_.program);
  glBindVertexArray(quad_.vertex_array);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, multi_target_.bright_texture);
  glUniform1i(blur_program_.color_texture, 0);
  GLint horizontal = false;
  glUniform1iv(blur_program_.horizontal, 1, &horizontal);
  glDrawArrays(GL_TRIANGLES, 0, 6);

  for (int i = 0; i < 5; i++) {
    horizontal = (i % 2 == 0);
    glBindFramebuffer(GL_FRAMEBUFFER, horizontal ? blur_target1_.frame_buffer : blur_target0_.frame_buffer);
    glUseProgram(blur_program_.program);
    glBindVertexArray(quad_.vertex_array);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, horizontal ? blur_target0_.texture : blur_target1_.texture);
    glUniform1i(blur_program_.color_texture, 0);
    glUniform1iv(blur_program_.horizontal, 1, &horizontal);

    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

  glViewport(0, 0, resolution.x, resolution.y);
  //Render to screen
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glUseProgram(bloom_program_.program);

  glBindVertexArray(quad_.vertex_array);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, multi_target_.color_texture);
  glUniform1i(bloom_program_.color_texture, 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, blur_target0_.texture);
  glUniform1i(bloom_program_.bright_color_texture, 1);
  float strength = 0.1f;
  glUniform1fv(bloom_program_.strength, 1, &strength);

  glDrawArrays(GL_TRIANGLES, 0, 6);
}

Renderer::DepthProgram::DepthProgram() {
  std::string name = "depth";
  auto vert_source = text("assets/shaders/" + name + ".vert");
  auto frag_source = text("assets/shaders/" + name + ".frag");

  auto vertex_shader = create_shader(vert_source, GL_VERTEX_SHADER, name);
  check_shader(vertex_shader, name);
  auto fragment_shader = create_shader(frag_source, GL_FRAGMENT_SHADER, name);
  check_shader(fragment_shader, name);

  program = glCreateProgram();

  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glBindAttribLocation(program, 0, "position");
  link_program(program, name);
  check_program(program, name);

  model_view_projection_matrix = glGetUniformLocation(program, "model_view_projection");
}

Renderer::DepthProgram::~DepthProgram() {
  glDeleteProgram(program);
}
Renderer::StandardProgram::StandardProgram() {
  std::string name = "standard";
  std::string vert_source = text("assets/shaders/" + name + ".vert");
  std::string frag_source = text("assets/shaders/" + name + ".frag");

  auto vertex_shader = create_shader(vert_source, GL_VERTEX_SHADER, name);
  assert(check_shader(vertex_shader, name));

  auto fragment_shader =
      create_shader(frag_source, GL_FRAGMENT_SHADER, name);
  assert(check_shader(fragment_shader, name));

  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);

  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "normal");
  glBindAttribLocation(program, 2, "tangent");
  glBindAttribLocation(program, 3, "uv");

  link_program(program, name);
  check_program(program, name);

  model_view_projection_matrix = (glGetUniformLocation(program, "model_view_projection"));
  model_matrix = glGetUniformLocation(program, "model");
  normal_matrix = glGetUniformLocation(program, "normal_matrix");
  for (int i = 0; i < 2; i++) {
    depth_bias_mvps[i] = glGetUniformLocation(program,
                                              std::string("depth_bias_model_view_projections[" + std::to_string(i)
                                                              + "]").c_str());
  }

  for (int i = 0; i < environment_maps.size(); i++) {
    environment_maps[i].map =
        glGetUniformLocation(program, std::string("environment_maps[" + std::to_string(i) + "]").c_str());
    environment_maps[i].position =
        glGetUniformLocation(program, std::string("environments[" + std::to_string(i) + "].position").c_str());
    environment_maps[i].extent =
        glGetUniformLocation(program, std::string("environments[" + std::to_string(i) + "].extent").c_str());
    environment_maps[i].strength =
        glGetUniformLocation(program, std::string("environments[" + std::to_string(i) + "].strength").c_str());
  }

  material_albedo_map = glGetUniformLocation(program, "material.albedo_map");
  material_emission_map = glGetUniformLocation(program, "material.emission_map");
  material_normal_map = glGetUniformLocation(program, "material.normal_map");
  material_metallic_map = glGetUniformLocation(program, "material.metallic_map");
  material_roughness_map = glGetUniformLocation(program, "material.roughness_map");
  material_ambient_occlusion_map = glGetUniformLocation(program, "material.ambient_occlusion_map");
  material_albedo = glGetUniformLocation(program, "material.albedo");
  material_roughness = glGetUniformLocation(program, "material.roughness");
  material_metallic = glGetUniformLocation(program, "material.metallic");
  material_opacity = glGetUniformLocation(program, "material.opacity");
  material_emission = glGetUniformLocation(program, "material.emission");
  material_emission_strength = glGetUniformLocation(program, "material.emission_strength");
  material_ambient_occlusion = glGetUniformLocation(program, "material.ambient_occlusion");
  material_factor = glGetUniformLocation(program, "material.factor");

  camera_position = glGetUniformLocation(program, "camera.position");
  camera_resolution = glGetUniformLocation(program, "camera.resolution");

  for (int i = 0; i < lights.size(); i++) {
    lights[i].position =
        glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].position").c_str());
    lights[i].color = glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].color").c_str());
    lights[i].view = glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].view").c_str());
    lights[i].projection =
        glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].projection").c_str());
    lights[i].angle = glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].angle").c_str());
    lights[i].direction =
        glGetUniformLocation(program, std::string("lights[" + std::to_string(i) + "].direction").c_str());

    shadow_maps[i] = glGetUniformLocation(program, std::string("shadow_maps[" + std::to_string(i) + "]").c_str());
  }

  fog_color_near = glGetUniformLocation(program, "fog.color_near");
  fog_color_far = glGetUniformLocation(program, "fog.color_far");
  fog_attenuation_factor = glGetUniformLocation(program, "fog.attenuation_factor");
  brdf_lut = glGetUniformLocation(program, "brdf_lut");
}
Renderer::StandardProgram::~StandardProgram() {
  glDeleteProgram(program);
}
Renderer::ParticleProgram::ParticleProgram() {
  std::string name = "particles";
  std::string vert_source = text("assets/shaders/" + name + ".vert");
  std::string frag_source = text("assets/shaders/" + name + ".frag");
  auto vertex_shader = create_shader(vert_source, GL_VERTEX_SHADER, name);
  assert(check_shader(vertex_shader, name));
  auto fragment_shader = create_shader(frag_source, GL_FRAGMENT_SHADER, name);
  assert(check_shader(fragment_shader, name));

  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "color");

  link_program(program, name);
  check_program(program, name);
  mvp = glGetUniformLocation(program, "model_view_projection"),
  mv = glGetUniformLocation(program, "model_view"),
  p = glGetUniformLocation(program, "projection"),
  texture = glGetUniformLocation(program, "tex"),
  resolution = glGetUniformLocation(program, "resolution");
}
Renderer::ParticleProgram::~ParticleProgram() {
  glDeleteProgram(program);
}
Renderer::BoxProgram::BoxProgram() {
  std::string name = "box";
  std::string vert_source = text("assets/shaders/" + name + ".vert");
  std::string frag_source = text("assets/shaders/" + name + ".frag");

  auto vertex_shader = create_shader(vert_source, GL_VERTEX_SHADER, name);
  assert(check_shader(vertex_shader, name));
  auto fragment_shader = create_shader(frag_source, GL_FRAGMENT_SHADER, name);
  assert(check_shader(fragment_shader, name));

  program = glCreateProgram();

  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glBindAttribLocation(program, 0, "position");
  link_program(program, name);
  check_program(program, name);

  mvp = glGetUniformLocation(program, "model_view_projection");
}
Renderer::BoxProgram::~BoxProgram() {
  glDeleteProgram(program);
}
Renderer::MultisampleProgram::MultisampleProgram() {
  std::string name = "multisample";
  auto vert_source = text("assets/shaders/" + name + ".vert");
  auto frag_source = text("assets/shaders/" + name + ".frag");

  auto vertex_shader = create_shader(vert_source, GL_VERTEX_SHADER, name);
  assert(check_shader(vertex_shader, name));
  auto fragment_shader = create_shader(frag_source, GL_FRAGMENT_SHADER, name);
  assert(check_shader(fragment_shader, name));

  program = glCreateProgram();

  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "uv");
  link_program(program, name);
  check_program(program, name);

  color_texture = glGetUniformLocation(program, "color_texture");
  depth_texture = glGetUniformLocation(program, "depth_texture");
}
Renderer::MultisampleProgram::~MultisampleProgram() {
  glDeleteProgram(program);
}

Renderer::BloomProgram::BloomProgram() {
  std::string name = "bloom";
  auto vert_source = text("assets/shaders/" + name + ".vert");
  auto frag_source = text("assets/shaders/" + name + ".frag");

  auto vertex_shader = create_shader(vert_source, GL_VERTEX_SHADER, name);
  assert(check_shader(vertex_shader, name));
  auto fragment_shader = create_shader(frag_source, GL_FRAGMENT_SHADER, name);
  assert(check_shader(fragment_shader, name));

  program = glCreateProgram();

  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "uv");
  link_program(program, name);
  check_program(program, name);
  color_texture = glGetUniformLocation(program, "color_texture");
  bright_color_texture = glGetUniformLocation(program, "bright_color_texture");
  strength = glGetUniformLocation(program, "strength");

}
Renderer::BloomProgram::~BloomProgram() {
  glDeleteProgram(program);
}
Renderer::BlurProgram::BlurProgram() {
  std::string name = "blur";
  auto vert_source = text("assets/shaders/" + name + ".vert");
  auto frag_source = text("assets/shaders/" + name + ".frag");

  auto vertex_shader = create_shader(vert_source, GL_VERTEX_SHADER, name);
  assert(check_shader(vertex_shader, name));
  auto fragment_shader = create_shader(frag_source, GL_FRAGMENT_SHADER, name);
  assert(check_shader(fragment_shader, name));

  program = glCreateProgram();

  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "uv");
  link_program(program, name);
  check_program(program, name);
  color_texture = glGetUniformLocation(program, "color_texture");
  horizontal = glGetUniformLocation(program, "horizontal");
}
Renderer::BlurProgram::~BlurProgram() {
  glDeleteProgram(program);
}

Renderer::ShadowMapTarget::ShadowMapTarget(const RenderBuffer &render_buffer) {
  glGenFramebuffers(1, &frame_buffer);
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

  glGenTextures(1, &texture);
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
Renderer::ShadowMapTarget::~ShadowMapTarget() {
  glDeleteFramebuffers(1, &frame_buffer);
  glDeleteTextures(1, &texture);
}

Renderer::RenderBuffer::RenderBuffer(const int resolution) : resolution(resolution) {
  glGenRenderbuffers(1, &render_buffer);
  glBindRenderbuffer(GL_RENDERBUFFER, render_buffer);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                        resolution,
                        resolution);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);
}
Renderer::RenderBuffer::~RenderBuffer() {
  glDeleteRenderbuffers(1, &render_buffer);
}
Renderer::EnvironmentMapTarget::EnvironmentMapTarget(const Renderer::RenderBuffer &render_buffer) {
  glGenFramebuffers(1, &frame_buffer);
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
                  GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
                  GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
                  GL_CLAMP_TO_EDGE);

  for (int i = 0; i < 6; i++) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                 GL_RGB,
                 render_buffer.resolution, render_buffer.resolution, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, nullptr);
  }
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_CUBE_MAP_POSITIVE_X, texture, 0);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, render_buffer.render_buffer);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("Framebuffer incomplete.");
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
Renderer::EnvironmentMapTarget::~EnvironmentMapTarget() {
  glDeleteTextures(1, &texture);
  glDeleteFramebuffers(1, &frame_buffer);
}

Renderer::StandardTarget::StandardTarget(const glm::vec2 &resolution) {
  glGenFramebuffers(1, &frame_buffer);
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, resolution.x, resolution.y, GL_TRUE);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture, 0);

  glGenTextures(1, &depth_texture);
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
Renderer::StandardTarget::~StandardTarget() {
  glDeleteFramebuffers(1, &frame_buffer);
  glDeleteTextures(1, &texture);
  glDeleteTextures(1, &depth_texture);
}
Renderer::MultiTarget::MultiTarget(const glm::vec2 &resolution) {
  glGenFramebuffers(1, &frame_buffer);
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

  glGenTextures(1, &color_texture);
  glBindTexture(GL_TEXTURE_2D, color_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, resolution.x, resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_2D, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_texture, 0);

  glGenTextures(1, &bright_texture);
  glBindTexture(GL_TEXTURE_2D, bright_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, resolution.x, resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_2D, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bright_texture, 0);

  unsigned int attachments[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
  glDrawBuffers(2, attachments);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("Framebuffer incomplete");
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
Renderer::MultiTarget::~MultiTarget() {
  glDeleteFramebuffers(1, &frame_buffer);
  glDeleteTextures(1, &color_texture);
  glDeleteTextures(1, &bright_texture);
}
Renderer::BlurTarget::BlurTarget(const glm::vec2 &resolution) {
  glGenFramebuffers(1, &frame_buffer);
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, resolution.x, resolution.y, 0, GL_RGB, GL_FLOAT, NULL);
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
Renderer::BlurTarget::~BlurTarget() {
  glDeleteFramebuffers(1, &frame_buffer);
  glDeleteTextures(1, &texture);
}
Renderer::Quad::Quad() {
  static const float quad_vertices[] = {
      -1.0f, 1.0f, 0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f, 0.0f,
      1.0f, -1.0f, 1.0f, 0.0f,
      -1.0f, 1.0f, 0.0f, 1.0f,
      1.0f, -1.0f, 1.0f, 0.0f,
      1.0f, 1.0f, 1.0f, 1.0f
  };
  glGenVertexArrays(1, &vertex_array);
  glGenBuffers(1, &buffer);
  glBindVertexArray(vertex_array);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) 0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *) (2 * sizeof(float)));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
Renderer::Quad::~Quad() {
  glDeleteBuffers(1, &buffer);
  glDeleteVertexArrays(1, &vertex_array);
}
Renderer::Box::Box() {
  // Render boxes
  float vertices[] = {
      -0.5, -0.5, -0.5, 1.0, 0.5, -0.5, -0.5, 1.0, 0.5, 0.5, -0.5,
      1.0, -0.5, 0.5, -0.5, 1.0, -0.5, -0.5, 0.5, 1.0, 0.5, -0.5,
      0.5, 1.0, 0.5, 0.5, 0.5, 1.0, -0.5, 0.5, 0.5, 1.0,
  };

  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  unsigned int elements[] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 4, 1, 5, 2, 6, 3, 7};

  glGenBuffers(1, &element_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glGenVertexArrays(1, &vertex_array);
  glBindVertexArray(vertex_array);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, // attribute
                        4, // number of elements per vertex, here (x,y,z,w)
                        GL_FLOAT, // the type of each element
                        GL_FALSE, // take our values as-is
                        0,        // no extra data between each position
                        0         // offset of first element
  );
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
  glBindVertexArray(0);

}
Renderer::Box::~Box() {

}
Renderer::Texture2DBuffer::Texture2DBuffer(const GLint internal_format,
                                   const GLint external_format,
                                   const GLint width,
                                   const GLint height,
                                   const GLint filter,
                                   const GLint wrap,
                                   const void *data) {
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

  glTexImage2D(GL_TEXTURE_2D, 0,
      internal_format,
               width, height, 0,
               external_format, GL_UNSIGNED_BYTE, data);
  glBindTexture(GL_TEXTURE_2D, 0);
}
Renderer::Texture2DBuffer::~Texture2DBuffer() {
  glDeleteTextures(1, &texture);
}
}
}
