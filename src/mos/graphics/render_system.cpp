#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtx/io.hpp>
#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <memory>

#include <mos/graphics/render_system.hpp>
#include <mos/graphics/vertex.hpp>
#include <mos/graphics/mesh.hpp>
#include <mos/graphics/texture.hpp>
#include <mos/graphics/model.hpp>
#include <mos/util.hpp>

namespace mos {

RenderSystem::RenderSystem(const glm::vec4 &color) : lightmaps_(true) {
  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
  glEnable(GL_POINT_SMOOTH);

  glDepthFunc(GL_LEQUAL);
  glDepthMask(GL_TRUE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  clear(color);

#ifdef MOS_SRGB
  glEnable(GL_FRAMEBUFFER_SRGB);
#endif

  std::string standard_vert_source = text("assets/shaders/standard_330.vert");
  std::string standard_frag_source = text("assets/shaders/standard_330.frag");
  add_vertex_program(RenderBatch::Shader::STANDARD, standard_vert_source,
                     standard_frag_source);

  std::string text_vert_source = text("assets/shaders/text_330.vert");
  std::string text_frag_source = text("assets/shaders/text_330.frag");
  add_vertex_program(RenderBatch::Shader::TEXT, text_vert_source, text_frag_source);

  add_vertex_program(RenderBatch::Shader::EFFECT,
                     text("assets/shaders/effect_330.vert"),
                     text("assets/shaders/effect_330.frag"));

  add_vertex_program(RenderBatch::Shader::BLUR, text("assets/shaders/blur_330.vert"),
                     text("assets/shaders/blur_330.frag"));

  add_vertex_program(RenderBatch::Shader::CRT, text("assets/shaders/crt_330.vert"),
                     text("assets/shaders/crt_330.frag"));

  std::string particles_vert_source = text("assets/shaders/particles_330.vert");
  std::string particles_frag_source = text("assets/shaders/particles_330.frag");
  add_particle_program("particles", particles_vert_source,
                       particles_frag_source);

  std::string box_vert_source = text("assets/shaders/box_330.vert");
  std::string box_frag_source = text("assets/shaders/box_330.frag");
  add_box_program("box", box_vert_source, box_frag_source);

  create_depth_program();

  // Render boxes
  float vertices[] = {
      -0.5, -0.5, -0.5, 1.0,  0.5, -0.5, -0.5, 1.0, 0.5, 0.5, -0.5,
      1.0,  -0.5, 0.5,  -0.5, 1.0, -0.5, -0.5, 0.5, 1.0, 0.5, -0.5,
      0.5,  1.0,  0.5,  0.5,  0.5, 1.0,  -0.5, 0.5, 0.5, 1.0,
  };

  glGenBuffers(1, &box_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, box_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  unsigned int elements[] = {0, 1, 2, 3, 4, 5, 6, 7, 0, 4, 1, 5, 2, 6, 3, 7};

  glGenBuffers(1, &box_ebo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, box_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glGenVertexArrays(1, &box_va);
  glBindVertexArray(box_va);
  glBindBuffer(GL_ARRAY_BUFFER, box_vbo);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, // attribute
                        4, // number of elements per vertex, here (x,y,z,w)
                        GL_FLOAT, // the type of each element
                        GL_FALSE, // take our values as-is
                        0,        // no extra data between each position
                        0         // offset of first element
                        );
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, box_ebo);
  glBindVertexArray(0);

  // Empty texture

  glGenTextures(1, &empty_texture_);
  glBindTexture(GL_TEXTURE_2D, empty_texture_);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  auto data = std::array<unsigned char, 4>{0, 0, 0, 0};
#ifdef MOS_SRGB
  glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, 1, 1, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data.data());
#else
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data.data());
#endif
  glBindTexture(GL_TEXTURE_2D, 0);

  // Shadow maps frame buffer

  /*
  glGenTextures(1, &depth_texture_);
  glBindTexture(GL_TEXTURE_2D, depth_texture_);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0,
               GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
  GL_COMPARE_REF_TO_TEXTURE);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenFramebuffers(1, &depth_frame_buffer_);
  glBindFramebuffer(GL_FRAMEBUFFER, depth_frame_buffer_);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
  depth_texture_, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("Shadowmap framebuffer incomplete.");
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
 */

  /*
  GLuint frame_buffer_id;
  glGenFramebuffers(1, &frame_buffer_id);
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);

  GLuint texture_id;
  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, 1024,
               1024, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D, texture_id, 0);

  GLuint depthrenderbuffer_id;
  glGenRenderbuffers(1, &depthrenderbuffer_id);
  glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer_id);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                        1024,
                        1024);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, depthrenderbuffer_id);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    throw std::runtime_error("Framebuffer incomplete.");
  }

  depth_texture_ = texture_id;
  depth_frame_buffer_ = frame_buffer_id;

  glBindFramebuffer(GL_FRAMEBUFFER, 0);*/
}

RenderSystem::~RenderSystem() {
  for (auto &vp : vertex_programs_) {
    glDeleteProgram(vp.second.program);
  }

  for (auto &pp : particle_programs_) {
    glDeleteProgram(pp.second.program);
  }

  for (auto &bp : box_programs_) {
    glDeleteProgram(bp.second.program);
  }

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
    glDeleteTextures(1, &t.second);
  }

  for (auto &ab : array_buffers_) {
    glDeleteBuffers(1, &ab.second);
  }

  for (auto &eab : element_array_buffers_) {
    glDeleteBuffers(1, &eab.second);
  }

  for (auto &va : vertex_arrays_) {
    glDeleteVertexArrays(1, &va.second);
  }
}

void RenderSystem::update_depth(const Model &model,
                            const glm::mat4 &parent_transform,
                            const glm::mat4 &view, const glm::mat4 &projection,
                            const glm::vec2 &resolution) {

  glViewport(0, 0, 1024, 1024);
  load(model);

  auto transform = model.transform;
  glm::mat4 mvp = projection * view * parent_transform * transform;

  glUseProgram(depth_program_.program);

  if (model.mesh) {
    glBindVertexArray(vertex_arrays_.at(model.mesh->id()));
  };

  glUniformMatrix4fv(depth_program_.mvp, 1, GL_FALSE, &mvp[0][0]);

  int num_elements = model.mesh ? std::distance(model.mesh->elements_begin(),
                                                model.mesh->elements_end())
                                : 0;

  if (model.mesh) {
    if (num_elements > 0) {
      glDrawElements(GL_TRIANGLES, num_elements, GL_UNSIGNED_INT, 0);
    } else {
      glDrawArrays(GL_TRIANGLES, 0, model.mesh->vertices_size());
    }
  }
  for (auto &child : model.models) {
    update_depth(child, parent_transform * model.transform, view, projection,
                 resolution);
  }
}

void RenderSystem::add_box_program(const std::string &name,
                               const std::string &vs_source,
                               const std::string &fs_source) {
  auto vertex_shader = create_shader(vs_source, GL_VERTEX_SHADER);
  check_shader(vertex_shader);
  auto fragment_shader = create_shader(fs_source, GL_FRAGMENT_SHADER);
  check_shader(fragment_shader);

  auto program = glCreateProgram();

  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glBindAttribLocation(program, 0, "position");
  glLinkProgram(program);
  check_program(program);

  box_programs_.insert(
      {name, BoxProgramData{program, glGetUniformLocation(
                                         program, "model_view_projection"),
                            glGetUniformLocation(program, "model_view")}});
}

void RenderSystem::create_depth_program() {
  auto vert_source = text("assets/shaders/depth_330.vert");
  auto frag_source = text("assets/shaders/depth_330.frag");

  auto vertex_shader = create_shader(vert_source, GL_VERTEX_SHADER);
  check_shader(vertex_shader);
  auto fragment_shader = create_shader(frag_source, GL_FRAGMENT_SHADER);
  check_shader(fragment_shader);

  auto program = glCreateProgram();

  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glBindAttribLocation(program, 0, "position");
  glLinkProgram(program);
  check_program(program);

  depth_program_ = DepthProgramData{
      program, glGetUniformLocation(program, "model_view_projection")};
}

void RenderSystem::add_particle_program(const std::string name,
                                    const std::string vs_source,
                                    const std::string fs_source) {
  auto vertex_shader = create_shader(vs_source, GL_VERTEX_SHADER);
  check_shader(vertex_shader);
  auto fragment_shader = create_shader(fs_source, GL_FRAGMENT_SHADER);
  check_shader(fragment_shader);

  auto program = glCreateProgram();

  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "color");

  glLinkProgram(program);
  check_program(program);

  particle_programs_.insert(ParticleProgramPair(
      name, ParticleProgramData{
                program, glGetUniformLocation(program, "model_view_projection"),
                glGetUniformLocation(program, "model_view")}));
}

void RenderSystem::add_vertex_program(const RenderBatch::Shader shader,
                                  const std::string vertex_shader_source,
                                  const std::string fragment_shader_source) {
  auto vertex_shader = create_shader(vertex_shader_source, GL_VERTEX_SHADER);
  check_shader(vertex_shader);

  auto fragment_shader =
      create_shader(fragment_shader_source, GL_FRAGMENT_SHADER);
  check_shader(fragment_shader);

  auto program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);

  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "normal");
  glBindAttribLocation(program, 2, "uv");
  glBindAttribLocation(program, 3, "uv_lightmap");

  std::cout << "Linking program" << std::endl;
  glLinkProgram(program);
  check_program(program);

  vertex_programs_.insert(VertexProgramPair(
      shader,
      VertexProgramData{
          program, glGetUniformLocation(program, "model_view_projection"),
          glGetUniformLocation(program, "model_view"),
          glGetUniformLocation(program, "normal_matrix"),
          glGetUniformLocation(program, "depth_bias_model_view_projection"),
          glGetUniformLocation(program, "textures.first"),
          glGetUniformLocation(program, "textures.second"),
          glGetUniformLocation(program, "lightmaps.first"),
          glGetUniformLocation(program, "lightmaps.second"),
          glGetUniformLocation(program, "lightmaps.mix"),
          glGetUniformLocation(program, "normalmap"),
          glGetUniformLocation(program, "shadowmap"),
          glGetUniformLocation(program, "material.ambient"),
          glGetUniformLocation(program, "material.diffuse"),
          glGetUniformLocation(program, "material.specular"),
          glGetUniformLocation(program, "material.specular_exponent"),
          glGetUniformLocation(program, "material.opacity"),
          glGetUniformLocation(program, "light.position"),
          glGetUniformLocation(program, "light.diffuse"),
          glGetUniformLocation(program, "light.specular"),
          glGetUniformLocation(program, "light.ambient"),
          glGetUniformLocation(program, "light.view"),
          glGetUniformLocation(program, "light.projection"),
          glGetUniformLocation(program, "receives_light"),
          glGetUniformLocation(program, "resolution"),
          glGetUniformLocation(program, "fogs.linear.color"),
          glGetUniformLocation(program, "fogs.linear.near"),
          glGetUniformLocation(program, "fogs.linear.far"),
          glGetUniformLocation(program, "fogs.exp.color"),
          glGetUniformLocation(program, "fogs.exp.density"),
          glGetUniformLocation(program, "time"),
          glGetUniformLocation(program, "overlay"),
          glGetUniformLocation(program, "multiply")}));
}

void RenderSystem::load(const Model &model) {
  if (model.mesh &&
      vertex_arrays_.find(model.mesh->id()) == vertex_arrays_.end()) {
    unsigned int vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    if (array_buffers_.find(model.mesh->id()) == array_buffers_.end()) {
      unsigned int array_buffer;
      glGenBuffers(1, &array_buffer);
      glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
      glBufferData(GL_ARRAY_BUFFER,
                   model.mesh->vertices_size() * sizeof(Vertex),
                   model.mesh->vertices_data(), GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      array_buffers_.insert({model.mesh->id(), array_buffer});
    }
    if (element_array_buffers_.find(model.mesh->id()) ==
        element_array_buffers_.end()) {
      unsigned int element_array_buffer;
      glGenBuffers(1, &element_array_buffer);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                   model.mesh->elements_size() * sizeof(unsigned int),
                   model.mesh->elements_data(), GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      element_array_buffers_.insert({model.mesh->id(), element_array_buffer});
    }
    glBindBuffer(GL_ARRAY_BUFFER, array_buffers_.at(model.mesh->id()));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<const void *>(sizeof(glm::vec3)));
    glVertexAttribPointer(
        2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        reinterpret_cast<const void *>(sizeof(glm::vec3) * 2));
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<const void *>(sizeof(glm::vec3) * 2 +
                                                         sizeof(glm::vec2)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                 element_array_buffers_.at(model.mesh->id()));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glBindVertexArray(0);
    vertex_arrays_.insert({model.mesh->id(), vertex_array});
  }

  if (model.mesh && !model.mesh->valid()) {
    glBindBuffer(GL_ARRAY_BUFFER, array_buffers_[model.mesh->id()]);
    glBufferData(GL_ARRAY_BUFFER, model.mesh->vertices_size() * sizeof(Vertex),
                 model.mesh->vertices_data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /*
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
       element_array_buffers_[model.mesh->id()]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     model.mesh->elements_size() * sizeof (unsigned int),
                     model.mesh->elements_data(),
                     GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        */
    model.mesh->valid_ = true;
  }

  if (model.textures.first) {
    if (textures_.find(model.textures.first->id()) == textures_.end()) {
      load(model.textures.first);
    }
  }

  if (model.textures.second) {
    if (textures_.find(model.textures.second->id()) == textures_.end()) {
      load(model.textures.second);
    }
  }

  if (model.lightmaps.first) {
    if (textures_.find(model.lightmaps.first->id()) == textures_.end()) {
      load(model.lightmaps.first);
    }
  }

  if (model.lightmaps.second) {
    if (textures_.find(model.lightmaps.second->id()) == textures_.end()) {
      load(model.lightmaps.second);
    }
  }

  if (model.normalmap) {
    if (textures_.find(model.normalmap->id()) == textures_.end()) {
      load(model.normalmap);
    }
  }
}

void RenderSystem::unload(const Model &model) {
  if (vertex_arrays_.find(model.mesh->id()) != vertex_arrays_.end()) {
    auto va_id = vertex_arrays_[model.mesh->id()];
    glDeleteVertexArrays(1, &va_id);
    vertex_arrays_.erase(model.mesh->id());

    if (array_buffers_.find(model.mesh->id()) != array_buffers_.end()) {
      auto abo_id = array_buffers_[model.mesh->id()];
      glDeleteBuffers(1, &abo_id);
      array_buffers_.erase(model.mesh->id());
    }
    if (element_array_buffers_.find(model.mesh->id()) !=
        element_array_buffers_.end()) {
      auto ebo_id = element_array_buffers_[model.mesh->id()];
      glDeleteBuffers(1, &ebo_id);
      element_array_buffers_.erase(model.mesh->id());
    }
  }

  if (model.textures.first) {
    if (textures_.find(model.textures.first->id()) != textures_.end()) {
      unload(model.textures.first);
    }
  }

  if (model.textures.second) {
    if (textures_.find(model.textures.second->id()) != textures_.end()) {
      unload(model.textures.second);
    }
  }

  if (model.lightmaps.first) {
    if (textures_.find(model.lightmaps.first->id()) != textures_.end()) {
      unload(model.lightmaps.first);
    }
  }

  if (model.lightmaps.second) {
    if (textures_.find(model.lightmaps.second->id()) != textures_.end()) {
      unload(model.lightmaps.second);
    }
  }

  if (model.normalmap) {
    if (textures_.find(model.normalmap->id()) != textures_.end()) {
      unload(model.normalmap);
    }
  }
}

void RenderSystem::load(const std::shared_ptr<Texture> &texture) {
#ifdef STREAM_TEXTURES
  if (textures_.find(texture->id()) == textures_.end()) {
    GLuint gl_id = create_texture_and_pbo(texture);
    textures_.insert({texture->id(), gl_id});
  }
#else
  if (textures_.find(texture->id()) == textures_.end()) {
    GLuint gl_id = create_texture(texture);
    textures_.insert({texture->id(), gl_id});
  }
#endif
}

void RenderSystem::unload(const std::shared_ptr<Texture> &texture) {
  if (textures_.find(texture->id()) == textures_.end()) {

  } else {
    auto gl_id = textures_[texture->id()];
    glDeleteTextures(1, &gl_id);
    textures_.erase(texture->id());
  }
}

void RenderSystem::clear_buffers() {
  for (auto &texture : textures_) {
    glDeleteTextures(1, &texture.second);
  }
  textures_.clear();

  for (auto &ab : array_buffers_) {
    glDeleteBuffers(1, &ab.second);
  }
  array_buffers_.clear();

  for (auto &eab : element_array_buffers_) {
    glDeleteBuffers(1, &eab.second);
  }
  element_array_buffers_.clear();
}

void RenderSystem::lightmaps(const bool lightmaps) { lightmaps_ = lightmaps; }

bool RenderSystem::lightmaps() const { return lightmaps_; }

unsigned int RenderSystem::create_shader(const std::string &source,
                                     const unsigned int type) {
  auto const *chars = source.c_str();
  auto id = glCreateShader(type);

  std::map<unsigned int, std::string> types{
      {GL_VERTEX_SHADER, "vertex shader"},
      {GL_FRAGMENT_SHADER, "fragment shader"},
      {GL_GEOMETRY_SHADER, "geometry shader"}};

  std::cout << "Compiling " << types[type] << std::endl;
  glShaderSource(id, 1, &chars, NULL);
  glCompileShader(id);
  return id;
}

bool RenderSystem::check_shader(const unsigned int shader) {
  if (!shader) {
    return false;
  }
  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

  std::map<unsigned int, std::string> types{
      {GL_VERTEX_SHADER, "vertex shader"},
      {GL_FRAGMENT_SHADER, "fragment shader"},
      {GL_GEOMETRY_SHADER, "geometry shader"}};
  GLint type;
  glGetShaderiv(shader, GL_SHADER_TYPE, &type);

  if (status == GL_FALSE) {
    int length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    if (length > 0) {
      std::vector<char> buffer(length);
      glGetShaderInfoLog(shader, length, NULL, &buffer[0]);
      std::cerr << "Compile failure in " << types[type] << " shader"
                << std::endl;
      std::cerr << std::string(buffer.begin(), buffer.end()) << std::endl;
    }
    return false;
  }
  return true;
}

bool RenderSystem::check_program(const unsigned int program) {
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
      std::cerr << "Link failure in program" << std::endl;
      std::cerr << std::string(buffer.begin(), buffer.end()) << std::endl;
    }
    return false;
  }
  return true;
}

unsigned int RenderSystem::create_texture(std::shared_ptr<Texture> texture) {
  GLuint id;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  GLfloat sampling = texture->mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampling);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampling);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  if (glewGetExtension("GL_EXT_texture_filter_anisotropic")) {
    float aniso = 0.0f;
    glBindTexture(GL_TEXTURE_2D, id);
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
  }

#ifdef MOS_SRGB

  glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, texture->width(),
               texture->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
               texture->data());
#else
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width(),
               texture->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
               texture->data());
#endif

  if (texture->mipmaps) {
    glGenerateMipmap(GL_TEXTURE_2D);
  };
  glBindTexture(GL_TEXTURE_2D, 0);
  return id;
}

unsigned int
RenderSystem::create_texture_and_pbo(const std::shared_ptr<Texture> &texture) {
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  GLuint texture_id;

  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

#ifdef MOS_SRGB
  glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, texture->width(),
               texture->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
#else
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width(),
               texture->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
#endif

  GLuint buffer_id;
  glGenBuffers(1, &buffer_id);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer_id);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, texture->size(), nullptr,
               GL_STREAM_DRAW);

  void *ptr = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, texture->size(),
                               (GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT));

  memcpy(ptr, texture->data(), texture->size());
  glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, GLsizei(texture->width()),
                  GLsizei(texture->height()), GL_RGBA, GL_UNSIGNED_BYTE,
                  nullptr);
  glDeleteBuffers(1, &buffer_id);

  /*
  if(texture->mipmaps) {
      glGenerateMipmap(GL_TEXTURE_2D);
  };
  */

  glBindTexture(GL_TEXTURE_2D, 0);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

  return texture_id;
}

void RenderSystem::update(const Particles &particles, const glm::mat4 &view,
                      const glm::mat4 &projection) {
  time_ += 0;
  if (vertex_arrays_.find(particles.id()) == vertex_arrays_.end()) {
    unsigned int vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    if (array_buffers_.find(particles.id()) == array_buffers_.end()) {
      unsigned int array_buffer;
      glGenBuffers(1, &array_buffer);
      glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
      glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle),
                   particles.data(), GL_STREAM_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      array_buffers_.insert({particles.id(), array_buffer});
    }
    glBindBuffer(GL_ARRAY_BUFFER, array_buffers_[particles.id()]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle),
                          reinterpret_cast<const void *>(sizeof(glm::vec3)));
    glVertexAttribPointer(
        2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle),
        reinterpret_cast<const void *>(sizeof(glm::vec3) + sizeof(glm::vec4)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glBindVertexArray(0);
    vertex_arrays_.insert({particles.id(), vertex_array});
  }

  //if (!particles.valid()) {
    glBindBuffer(GL_ARRAY_BUFFER, array_buffers_[particles.id()]);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle),
                 particles.data(), GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //particles.valid_ = true;
  //}

  glm::mat4 mv = view;
  glm::mat4 mvp = projection * view;

  auto &uniforms = particle_programs_.at("particles");

  glUseProgram(uniforms.program);

  glBindVertexArray(vertex_arrays_[particles.id()]);

  glUniformMatrix4fv(uniforms.mvp, 1, GL_FALSE, &mvp[0][0]);
  glUniformMatrix4fv(uniforms.mv, 1, GL_FALSE, &mv[0][0]);

  glDrawArrays(GL_POINTS, 0, particles.size());
}

void RenderSystem::update(const Box &box, const glm::mat4 &view,
                      const glm::mat4 &projection) {
  time_ += 0;

  auto &uniforms = box_programs_.at("box");

  glUseProgram(uniforms.program);

  glBindVertexArray(box_va);

  glm::vec3 size = box.size();

  glm::mat4 transform = glm::translate(glm::mat4(1.0f), box.position);

  glm::mat4 mv = view * transform * glm::scale(glm::mat4(1.0f), size);
  glm::mat4 mvp =
      projection * view * transform * glm::scale(glm::mat4(1.0f), size);

  glUniformMatrix4fv(uniforms.mvp, 1, GL_FALSE, &mvp[0][0]);
  glUniformMatrix4fv(uniforms.mv, 1, GL_FALSE, &mv[0][0]);

  // glDrawArrays(GL_POINTS, 0, 16);
  glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
  glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT,
                 (GLvoid *)(4 * sizeof(GLuint)));
  glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (GLvoid *)(8 * sizeof(GLuint)));
}

void RenderSystem::update(const Box &box, const Camera &camera) {
  update(box, camera.view, camera.projection);
}


void RenderSystem::update(const Model &model, const Camera &camera,
                      const glm::vec2 &resolution, const Light &light,
                      const FogExp &fog_exp, const FogLinear &fog_linear, const float multiply) {
  update(model, glm::mat4(1.0f), camera.view, camera.projection, resolution,
         light, fog_exp, fog_linear, multiply);
}

void RenderSystem::update(const Model &model, const glm::mat4 &view,
                      const glm::mat4 &projection,
                      const glm::vec2 &resolution, const Light &light,
                      const FogExp &fog_exp, const FogLinear &fog_linear,
                      const float multiply,
                      const RenderBatch::Shader &shader,
                      const RenderBatch::Draw &draw) {
  update(model, glm::mat4(1.0f), view, projection, resolution, light, fog_exp, fog_linear,
         multiply, shader, draw);
}

void RenderSystem::update(const Model &model, const glm::mat4 parent_transform,
                      const glm::mat4 view, const glm::mat4 projection, const glm::vec2 &resolution,
                      const Light &light, const FogExp &fog_exp, const FogLinear &fog_linear, const float multiply,
                      const RenderBatch::Shader &shader,
                      const RenderBatch::Draw &draw) {
  time_ += 0.0f; // TODO: Not correct since called many times!
  glViewport(0, 0, resolution.x, resolution.y);
  load(model);

  auto transform = model.transform;
  glm::mat4 mv = view * parent_transform * transform;
  glm::mat4 mvp = projection * view * parent_transform * transform;

  if (model.mesh) {
    glBindVertexArray(vertex_arrays_.at(model.mesh->id()));
  };

  auto &uniforms = vertex_programs_.at(shader);

  int texture_unit = 0;

  glActiveTexture(GLenum(GL_TEXTURE0 + texture_unit));
  glBindTexture(GL_TEXTURE_2D, model.textures.first
                                   ? textures_[model.textures.first->id()]
                                   : empty_texture_);
  glUniform1i(uniforms.textures_first, texture_unit);
  texture_unit++;

  // Shadowmap
  glActiveTexture(GLenum(GL_TEXTURE0 + texture_unit));
  glBindTexture(GL_TEXTURE_2D, depth_texture_);
  glUniform1i(uniforms.shadowmap, texture_unit);
  // glBindTexture(GL_TEXTURE_2D, textures_[1]);
  // glUniform1i(uniforms.shadowmap, texture_unit);
  texture_unit++;

  glActiveTexture(GLenum(GL_TEXTURE0 + texture_unit));
  glBindTexture(GL_TEXTURE_2D, model.textures.second
                                   ? textures_[model.textures.second->id()]
                                   : empty_texture_);
  glUniform1i(uniforms.textures_second, texture_unit);
  texture_unit++;

  glActiveTexture(GLenum(GL_TEXTURE0 + texture_unit));
  glBindTexture(GL_TEXTURE_2D, model.lightmaps.first
                                   ? textures_[model.lightmaps.first->id()]
                                   : empty_texture_);
  glUniform1i(uniforms.lightmaps_first, texture_unit);
  texture_unit++;

  glActiveTexture(GLenum(GL_TEXTURE0 + texture_unit));
  glBindTexture(GL_TEXTURE_2D, model.lightmaps.second
                                   ? textures_[model.lightmaps.second->id()]
                                   : empty_texture_);
  glUniform1i(uniforms.lightmaps_second, texture_unit);
  texture_unit++;

  glActiveTexture(GLenum(GL_TEXTURE0 + texture_unit));
  glBindTexture(GL_TEXTURE_2D, model.normalmap
                                   ? textures_[model.normalmap->id()]
                                   : empty_texture_);
  glUniform1i(uniforms.normalmap, texture_unit);
  texture_unit++;

  glUniformMatrix4fv(uniforms.mvp, 1, GL_FALSE, &mvp[0][0]);
  glUniformMatrix4fv(uniforms.mv, 1, GL_FALSE, &mv[0][0]);

  const glm::mat4 bias(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.5,
                       0.0, 0.5, 0.5, 0.5, 1.0);

  glm::mat4 depth_bias_mvp = bias * light.projection * light.view * transform;
  glUniformMatrix4fv(uniforms.depth_bias_mvp, 1, GL_FALSE,
                     &depth_bias_mvp[0][0]);

  glm::mat3 normal_matrix = glm::inverseTranspose(glm::mat3(mv));
  glUniformMatrix3fv(uniforms.normal_matrix, 1, GL_FALSE, &normal_matrix[0][0]);

  if (model.material) {
    glUniform3fv(uniforms.material_ambient_color, 1,
                 glm::value_ptr(model.material->ambient));
    glUniform3fv(uniforms.material_diffuse_color, 1,
                 glm::value_ptr(model.material->diffuse));
    glUniform3fv(uniforms.material_specular_color, 1,
                 glm::value_ptr(model.material->specular));
    glUniform1fv(uniforms.material_specular_exponent, 1,
                 &model.material->specular_exponent);
    glUniform1fv(uniforms.opacity, 1, &model.material->opacity);
  } else {
    auto opacity = 1.0f;
    glUniform1fv(uniforms.opacity, 1, &opacity);
  }

  // Transform light position to eye space.
  glUniform3fv(uniforms.light_position, 1,
               glm::value_ptr(glm::vec3(
                   view * glm::vec4(light.position.x, light.position.y,
                                    light.position.z, 1.0f))));
  glUniform3fv(uniforms.light_diffuse_color, 1, glm::value_ptr(light.diffuse));
  glUniform3fv(uniforms.light_specular_color, 1,
               glm::value_ptr(light.specular));
  glUniform3fv(uniforms.light_ambient_color, 1, glm::value_ptr(light.ambient));
  glUniformMatrix4fv(uniforms.light_view, 1, GL_FALSE, &light.view[0][0]);
  glUniformMatrix4fv(uniforms.light_projection, 1, GL_FALSE,
                     &light.projection[0][0]);

  glUniform1i(uniforms.receives_light, model.receives_light);
  glUniform2fv(uniforms.resolution, 1, glm::value_ptr(resolution));

  glUniform3fv(uniforms.fogs_exp_color, 1, glm::value_ptr(fog_exp.color));
  glUniform1fv(uniforms.fogs_exp_density, 1, &fog_exp.density);

  glUniform3fv(uniforms.fogs_linear_color, 1, glm::value_ptr(fog_linear.color));
  glUniform1fv(uniforms.fogs_linear_near, 1, &fog_linear.near);
  glUniform1fv(uniforms.fogs_linear_far, 1, &fog_linear.far);

  glUniform1fv(uniforms.time, 1, &time_);
  glUniform4fv(uniforms.overlay, 1, glm::value_ptr(model.overlay()));
  auto v = model.multiply();
  glUniform1fv(uniforms.multiply, 1, &v);
  glUniform1fv(uniforms.lightmaps_mix, 1, &model.lightmaps.mix);

  int num_elements = model.mesh ? std::distance(model.mesh->elements_begin(),
                                                model.mesh->elements_end())
                                : 0;
  int draw_type = GL_TRIANGLES;
  if (draw == RenderBatch::Draw::LINES) {
    draw_type = GL_LINES;
  } else if (draw == RenderBatch::Draw::POINTS) {
    draw_type = GL_POINTS;
  }
  if (model.mesh) {
    if (num_elements > 0) {
      glDrawElements(draw_type, num_elements, GL_UNSIGNED_INT, 0);
    } else {
      glDrawArrays(draw_type, 0, model.mesh->vertices_size());
    }
  }
  for (auto &child : model.models) {
    update(child, parent_transform * model.transform, view, projection,
           resolution, light, fog_exp, fog_linear, multiply);
  }
}

void RenderSystem::render_target(const OptTarget &target) {
  if (target) {
    if (frame_buffers_.find(target->id()) == frame_buffers_.end()) {
      GLuint frame_buffer_id;
      glGenFramebuffers(1, &frame_buffer_id);
      glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);

      GLuint texture_id;
      glGenTextures(1, &texture_id);
      glBindTexture(GL_TEXTURE_2D, texture_id);
#ifdef MOS_SRGB
      glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, target->texture->width(),
                   target->texture->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
#else
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, target->texture->width(),
                   target->texture->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
#endif
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                             GL_TEXTURE_2D, texture_id, 0);

      GLuint depthrenderbuffer_id;
      glGenRenderbuffers(1, &depthrenderbuffer_id);
      glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer_id);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                            target->texture->width(),
                            target->texture->height());
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                GL_RENDERBUFFER, depthrenderbuffer_id);
      glBindRenderbuffer(GL_RENDERBUFFER, 0);

      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Framebuffer incomplete.");
      }

      textures_.insert({target->texture->id(), texture_id});
      render_buffers.insert({target->id(), depthrenderbuffer_id});
      frame_buffers_.insert({target->id(), frame_buffer_id});

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    auto fb = frame_buffers_[target->id()];
    glBindFramebuffer(GL_FRAMEBUFFER, fb);
  } else {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
}

void RenderSystem::clear(const glm::vec4 &color) {
  glClearDepthf(1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(color.r, color.g, color.b, color.a);
}

void RenderSystem::batches(const std::initializer_list<RenderBatch> &batches_init,
                           const std::initializer_list<ParticlesBatch> &particles_batches,
                       const glm::vec4 &color,
                       const OptTarget &target) {
  batches(batches_init.begin(), batches_init.end(), particles_batches.begin(), particles_batches.end(), color, target);
}
}
