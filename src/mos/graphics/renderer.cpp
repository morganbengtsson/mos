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

#include <mos/graphics/renderer.hpp>
#include <mos/graphics/vertex.hpp>
#include <mos/graphics/mesh.hpp>
#include <mos/graphics/texture2d.hpp>
#include <mos/graphics/model.hpp>
#include <mos/util.hpp>

namespace mos {

Renderer::Renderer() : lightmaps_(true) {
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

  glEnable(GL_DEPTH_TEST);
#ifdef __ANDROID__
#else
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
#endif
  glDepthFunc(GL_LEQUAL);
  glDepthMask(GL_TRUE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_FRAMEBUFFER_SRGB);

  std::string standard_vert_source = text("assets/shaders/standard_330.vert");
  std::string standard_frag_source = text("assets/shaders/standard_330.frag");
  add_vertex_program(Model::Shader::STANDARD, standard_vert_source,
                     standard_frag_source);

  std::string text_vert_source = text("assets/shaders/text_330.vert");
  std::string text_frag_source = text("assets/shaders/text_330.frag");
  add_vertex_program(Model::Shader::TEXT, text_vert_source, text_frag_source);

  add_vertex_program(Model::Shader::EFFECT,
                     text("assets/shaders/effect_330.vert"),
                     text("assets/shaders/effect_330.frag"));

  add_vertex_program(Model::Shader::BLUR, text("assets/shaders/blur_330.vert"),
                     text("assets/shaders/blur_330.frag"));

  add_vertex_program(Model::Shader::CRT, text("assets/shaders/crt_330.vert"),
                     text("assets/shaders/crt_330.frag"));

  std::string particles_vert_source = text("assets/shaders/particles_330.vert");
  std::string particles_frag_source = text("assets/shaders/particles_330.frag");
  add_particle_program("particles", particles_vert_source,
                       particles_frag_source);

  std::string box_vert_source = text("assets/shaders/box_330.vert");
  std::string box_frag_source = text("assets/shaders/box_330.frag");
  add_box_program("box", box_vert_source, box_frag_source);

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
}

Renderer::~Renderer() {
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

void Renderer::add_box_program(const std::string &name,
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

void Renderer::add_particle_program(const std::string name,
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

void Renderer::add_vertex_program(const Model::Shader shader,
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
          glGetUniformLocation(program, "texture"),
          glGetUniformLocation(program, "texture2"),
          glGetUniformLocation(program, "lightmap"),
          glGetUniformLocation(program, "normalmap"),
          glGetUniformLocation(program, "material_ambient_color"),
          glGetUniformLocation(program, "material_diffuse_color"),
          glGetUniformLocation(program, "material_specular_color"),
          glGetUniformLocation(program, "material_specular_exponent"),
          glGetUniformLocation(program, "opacity"),
          glGetUniformLocation(program, "light_position"),
          glGetUniformLocation(program, "light_diffuse_color"),
          glGetUniformLocation(program, "light_specular_color"),
          glGetUniformLocation(program, "has_texture"),
          glGetUniformLocation(program, "has_texture2"),
          glGetUniformLocation(program, "has_lightmap"),
          glGetUniformLocation(program, "has_normalmap"),
          glGetUniformLocation(program, "has_material"),
          glGetUniformLocation(program, "receives_light"),
          glGetUniformLocation(program, "resolution"),
          glGetUniformLocation(program, "fog_color"),
          glGetUniformLocation(program, "fog_density"),
          glGetUniformLocation(program, "time"),
          glGetUniformLocation(program, "overlay")}));
}

void Renderer::load(const Model &model) {
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

  if (model.texture) {
    if (textures_.find(model.texture->id()) == textures_.end()) {
      load(model.texture);
    }
  }

  if (model.texture2) {
    if (textures_.find(model.texture2->id()) == textures_.end()) {
      load(model.texture2);
    }
  }

  if (model.lightmap) {
    if (textures_.find(model.lightmap->id()) == textures_.end()) {
      load(model.lightmap);
    }
  }

  if (model.normalmap) {
    if (textures_.find(model.normalmap->id()) == textures_.end()) {
      load(model.normalmap);
    }
  }
}

void Renderer::unload(const Model &model) {
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

  if (model.texture) {
    if (textures_.find(model.texture->id()) != textures_.end()) {
      unload(model.texture);
    }
  }

  if (model.texture2) {
    if (textures_.find(model.texture2->id()) != textures_.end()) {
      unload(model.texture2);
    }
  }

  if (model.lightmap) {
    if (textures_.find(model.lightmap->id()) != textures_.end()) {
      unload(model.lightmap);
    }
  }

  if (model.normalmap) {
    if (textures_.find(model.normalmap->id()) != textures_.end()) {
      unload(model.normalmap);
    }
  }
}

void Renderer::load(const std::shared_ptr<Texture2D> &texture) {
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

void Renderer::unload(const std::shared_ptr<Texture2D> &texture) {
  if (textures_.find(texture->id()) == textures_.end()) {

  } else {
    auto gl_id = textures_[texture->id()];
    glDeleteTextures(1, &gl_id);
    textures_.erase(texture->id());
  }
}

void Renderer::clear_buffers() {
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

void Renderer::lightmaps(const bool lightmaps) { lightmaps_ = lightmaps; }

bool Renderer::lightmaps() const { return lightmaps_; }

unsigned int Renderer::create_shader(const std::string source,
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

bool Renderer::check_shader(const unsigned int shader) {
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

bool Renderer::check_program(const unsigned int program) {
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

unsigned int Renderer::create_texture(std::shared_ptr<Texture2D> texture) {
  GLuint id;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  GLfloat sampling = texture->mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampling);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampling);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  if (glewGetExtension("GL_EXT_texture_filter_anisotropic")) {
    float aniso = 0.0f;
    glBindTexture(GL_TEXTURE_2D, id);
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
  }
  glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, texture->width(),
               texture->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
               texture->data());

  if (texture->mipmaps) {
    glGenerateMipmap(GL_TEXTURE_2D);
  };
  glBindTexture(GL_TEXTURE_2D, 0);
  return id;
}

unsigned int
Renderer::create_texture_and_pbo(const std::shared_ptr<Texture2D> &texture) {
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  GLuint texture_id;

  glGenTextures(1, &texture_id);
  glBindTexture(GL_TEXTURE_2D, texture_id);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, texture->width(),
               texture->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

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

void Renderer::update(Particles &particles, const glm::mat4 view,
                      const glm::mat4 projection, const float dt) {
  time_ += dt;
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

  if (!particles.valid()) {
    glBindBuffer(GL_ARRAY_BUFFER, array_buffers_[particles.id()]);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle),
                 particles.data(), GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    particles.valid_ = true;
  }

  glm::mat4 mv = view;
  glm::mat4 mvp = projection * view;

  auto &uniforms = particle_programs_.at("particles");

  glUseProgram(uniforms.program);

  glBindVertexArray(vertex_arrays_[particles.id()]);

  glUniformMatrix4fv(uniforms.mvp, 1, GL_FALSE, &mvp[0][0]);
  glUniformMatrix4fv(uniforms.mv, 1, GL_FALSE, &mv[0][0]);

  glDrawArrays(GL_POINTS, 0, particles.size());
}

void Renderer::update(const Box &box, const glm::mat4 &view,
                      const glm::mat4 &projection, const float dt) {
  time_ += dt;

  auto &uniforms = box_programs_.at("box");

  glUseProgram(uniforms.program);

  glBindVertexArray(box_va);

  glm::vec3 size = box.size();

  glm::mat4 transform = glm::translate(glm::mat4(1.0f), box.position());

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

void Renderer::update(const Box &box, const Camera &camera, const float dt) {
  update(box, camera.view, camera.projection, dt);
}

void Renderer::update(const Model &model, const Camera &camera, const float dt,
                      const glm::vec2 &resolution, const Light &light,
                      const Fog &fog) {
  update(model, glm::mat4(1.0f), camera.view, camera.projection, dt, resolution,
         light, fog);
}

void Renderer::update(const Model &model, const glm::mat4 &view,
                      const glm::mat4 &projection, const float dt,
                      const glm::vec2 &resolution, const Light &light,
                      const Fog &fog) {
  update(model, glm::mat4(1.0f), view, projection, dt, resolution, light, fog);
}

void Renderer::update(const Model &model, const glm::mat4 parent_transform,
                      const glm::mat4 view, const glm::mat4 projection,
                      const float dt, const glm::vec2 &resolution,
                      const Light &light, const Fog &fog) {
  time_ += dt;
  glViewport(0, 0, resolution.x, resolution.y);
  load(model);

  auto transform = model.transform;
  glm::mat4 mv = view * parent_transform * transform;
  glm::mat4 mvp = projection * view * parent_transform * transform;

  glUseProgram(vertex_programs_[model.shader].program);

  if (model.mesh) {
    glBindVertexArray(vertex_arrays_.at(model.mesh->id()));
  };

  auto &uniforms = vertex_programs_.at(model.shader);

  int texture_unit = 0;
  if (model.texture != nullptr) {
    glActiveTexture(GLenum(GL_TEXTURE0 + texture_unit));
    glBindTexture(GL_TEXTURE_2D, textures_[model.texture->id()]);
    glUniform1i(uniforms.texture, texture_unit);
    texture_unit++;
  }

  if (model.texture2 != nullptr) {
    glActiveTexture(GLenum(GL_TEXTURE0 + texture_unit));
    glBindTexture(GL_TEXTURE_2D, textures_[model.texture2->id()]);
    glUniform1i(uniforms.texture2, texture_unit);
    texture_unit++;
  }

  if (model.lightmap) {
    glActiveTexture(GLenum(GL_TEXTURE0 + texture_unit));
    glBindTexture(GL_TEXTURE_2D, textures_[model.lightmap->id()]);
    glUniform1i(uniforms.lightmap, texture_unit);
    texture_unit++;
  }

  if (model.normalmap) {
    glActiveTexture(GLenum(GL_TEXTURE0 + texture_unit));
    glBindTexture(GL_TEXTURE_2D, textures_[model.normalmap->id()]);
    glUniform1i(uniforms.normalmap, texture_unit);
    texture_unit++;
  }

  glUniformMatrix4fv(uniforms.mvp, 1, GL_FALSE, &mvp[0][0]);
  glUniformMatrix4fv(uniforms.mv, 1, GL_FALSE, &mv[0][0]);

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
  glUniform3fv(uniforms.light_diffuse_color, 1,
               glm::value_ptr(light.diffuse_color));
  glUniform3fv(uniforms.light_specular_color, 1,
               glm::value_ptr(light.specular_color));

  glUniform1i(uniforms.has_texture, model.texture ? true : false);
  glUniform1i(uniforms.has_texture2, model.texture2 ? true : false);
  glUniform1i(uniforms.has_lightmap,
              model.lightmap ? true : lightmaps_ ? true : false);
  glUniform1i(uniforms.has_normalmap, model.normalmap ? true : false);
  glUniform1i(uniforms.has_material, model.material ? true : false);

  glUniform1i(uniforms.receives_light, model.receives_light);
  glUniform2fv(uniforms.resolution, 1, glm::value_ptr(resolution));

  glUniform3fv(uniforms.fog_color, 1, glm::value_ptr(fog.color));
  glUniform1fv(uniforms.fog_density, 1, &fog.density);

  glUniform1fv(uniforms.time, 1, &time_);
  glUniform3fv(uniforms.overlay, 1, glm::value_ptr(model.overlay()));

  int num_elements = model.mesh ? std::distance(model.mesh->elements_begin(),
                                                model.mesh->elements_end())
                                : 0;
  int draw_type = GL_TRIANGLES;
  if (model.draw == Model::Draw::LINES) {
    draw_type = GL_LINES;
  } else if (model.draw == Model::Draw::POINTS) {
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
    update(child, parent_transform * model.transform, view, projection, dt,
           resolution, light, fog);
  }
}

void Renderer::render_target(const OptTarget &target) {
  if (target) {
    if (frame_buffers_.find(target->id()) == frame_buffers_.end()) {
      GLuint frame_buffer_id;
      glGenFramebuffers(1, &frame_buffer_id);
      glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);

      GLuint texture_id;
      glGenTextures(1, &texture_id);
      glBindTexture(GL_TEXTURE_2D, texture_id);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, target->texture->width(),
                   target->texture->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
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

void Renderer::clear(const glm::vec4 &color) {
  glClearDepthf(1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(color.r, color.g, color.b, color.a);
}
}
