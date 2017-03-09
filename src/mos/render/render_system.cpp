#include <GL/glew.h>
#include <glm/glm.hpp>
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
#include <string>
#include <vector>

#include <mos/render/mesh.hpp>
#include <mos/render/model.hpp>
#include <mos/render/render_system.hpp>
#include <mos/render/texture_2d.hpp>
#include <mos/render/vertex.hpp>
#include <mos/util.hpp>

namespace mos {

static std::map<Texture::Format, GLuint> format_map{
    {Texture::Format::SRGB, GL_SRGB},
    {Texture::Format::SRGBA, GL_SRGB_ALPHA},
    {Texture::Format::RGB, GL_RGB},
    {Texture::Format::RGBA, GL_RGBA},
    {Texture::Format::DEPTH, GL_DEPTH_COMPONENT}};

static std::map<Texture::Format, GLuint> format_map_compressed{
    {Texture::Format::SRGB, GL_COMPRESSED_SRGB},
    {Texture::Format::SRGBA, GL_COMPRESSED_SRGB_ALPHA},
    {Texture::Format::RGB, GL_COMPRESSED_RGB},
    {Texture::Format::RGBA, GL_COMPRESSED_RGBA},
    {Texture::Format::DEPTH, GL_DEPTH_COMPONENT}};

static std::map<Texture::Wrap, GLuint> wrap_map{
    {Texture::Wrap::CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE},
    {Texture::Wrap::CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER},
    {Texture::Wrap::REPEAT, GL_REPEAT}};

RenderSystem::RenderSystem(const glm::vec4 &color) : lightmaps_(true) {

  glewExperimental = GL_TRUE;
  GLenum err = glewInit();

  // std::cout << "Uniforms: " << glewGetString(GL_MAX_UNIFORM_LOCATIONS) <<
  // std::endl;
  // char *version = (char*)glewGetString(GL_VERSION);
  // std::cout << version << std::endl;
  if (GLEW_OK != err) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
  fprintf(stdout, "Status: OpenGL version: %s\n", glGetString(GL_VERSION));
  fprintf(stdout, "Max uniform locations: %s\n",
          glGetString(GL_MAX_ARRAY_TEXTURE_LAYERS));

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_FRAMEBUFFER_SRGB);

  // glEnable(GL_TEXTURE_CUBE_MAP);
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

  glDepthFunc(GL_LEQUAL);
  glDepthMask(GL_TRUE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  clear(color);

  std::string shader_path = "assets/shaders/";

  std::string standard_vert = "standard_330.vert";
  std::string standard_frag = "standard_330.frag";
  std::string standard_vert_source = text(shader_path + standard_vert);
  std::string standard_frag_source = text(shader_path + standard_frag);
  add_vertex_program(RenderScene::Shader::STANDARD, standard_vert_source,
                     standard_frag_source, standard_vert, standard_frag);

  std::string text_vert = "text_330.vert";
  std::string text_frag = "text_330.frag";
  std::string text_vert_source = text(shader_path + text_vert);
  std::string text_frag_source = text(shader_path + text_frag);
  add_vertex_program(RenderScene::Shader::TEXT, text_vert_source,
                     text_frag_source, text_vert, text_frag);

  std::string effect_vert = "effect_330.vert";
  std::string effect_frag = "effect_330.frag";
  add_vertex_program(RenderScene::Shader::EFFECT,
                     text(shader_path + effect_vert),
                     text(shader_path + effect_frag), effect_vert, effect_frag);

  std::string blur_vert = "blur_330.vert";
  std::string blur_frag = "blur_330.frag";
  add_vertex_program(RenderScene::Shader::BLUR, text(shader_path + blur_vert),
                     text(shader_path + blur_frag), blur_vert, blur_frag);

  std::string crt_vert = "crt_330.vert";
  std::string crt_frag = "crt_330.frag";
  add_vertex_program(RenderScene::Shader::CRT, text(shader_path + crt_vert),
                     text(shader_path + crt_frag), crt_vert, crt_frag);

  std::string particles_vert = "particles_330.vert";
  std::string particles_frag = "particles_330.frag";
  std::string particles_vert_source = text(shader_path + particles_vert);
  std::string particles_frag_source = text(shader_path + particles_frag);
  add_particle_program("particles", particles_vert_source,
                       particles_frag_source, particles_vert, particles_frag);

  std::string box_vert = "box_330.vert";
  std::string box_frag = "box_330.frag";
  std::string box_vert_source = text(shader_path + box_vert);
  std::string box_frag_source = text(shader_path + box_frag);
  add_box_program("box", box_vert_source, box_frag_source, box_vert, box_frag);

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
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               data.data());
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
                                const glm::mat4 &view,
                                const glm::mat4 &projection,
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
                                   const std::string &fs_source,
                                   const std::string &vs_file,
                                   const std::string &fs_file) {
  auto vertex_shader = create_shader(vs_source, GL_VERTEX_SHADER);
  check_shader(vertex_shader, vs_file);
  auto fragment_shader = create_shader(fs_source, GL_FRAGMENT_SHADER);
  check_shader(fragment_shader, fs_file);

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
  check_shader(vertex_shader, "depth_330.vert");
  auto fragment_shader = create_shader(frag_source, GL_FRAGMENT_SHADER);
  check_shader(fragment_shader, "depth_330.frag");

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
                                        const std::string fs_source,
                                        const std::string &vs_file,
                                        const std::string &fs_file) {
  auto vertex_shader = create_shader(vs_source, GL_VERTEX_SHADER);
  check_shader(vertex_shader, vs_file);
  auto fragment_shader = create_shader(fs_source, GL_FRAGMENT_SHADER);
  check_shader(fragment_shader, fs_file);

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

void RenderSystem::add_vertex_program(const RenderScene::Shader shader,
                                      const std::string vertex_shader_source,
                                      const std::string fragment_shader_source,
                                      const std::string &vert_file_name,
                                      const std::string &frag_file_name) {
  auto vertex_shader = create_shader(vertex_shader_source, GL_VERTEX_SHADER);
  check_shader(vertex_shader, vert_file_name);

  auto fragment_shader =
      create_shader(fragment_shader_source, GL_FRAGMENT_SHADER);
  check_shader(fragment_shader, frag_file_name);

  auto program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);

  glBindAttribLocation(program, 0, "position");
  glBindAttribLocation(program, 1, "normal");
  glBindAttribLocation(program, 2, "tangent");
  glBindAttribLocation(program, 3, "uv");
  glBindAttribLocation(program, 4, "uv_lightmap");

  std::cout << "Linking program" << std::endl;
  glLinkProgram(program);
  check_program(program);

  vertex_programs_.insert(
      VertexProgramPair(shader, VertexProgramData(program)));
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

    // Lightmap UV
    glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<const void *>(sizeof(glm::vec3) * 3 +
                                                         sizeof(glm::vec2)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                 element_array_buffers_.at(model.mesh->id()));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glBindVertexArray(0);
    vertex_arrays_.insert({model.mesh->id(), vertex_array});
  }

  if (model.mesh && !model.mesh->valid()) {
    glBindBuffer(GL_ARRAY_BUFFER, array_buffers_[model.mesh->id()]);
    glBufferData(GL_ARRAY_BUFFER, model.mesh->vertices_size() * sizeof(Vertex),
                 model.mesh->vertices_data(), GL_DYNAMIC_DRAW);
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

  if (model.material.diffuse_map) {
    if (textures_.find(model.material.diffuse_map->id()) == textures_.end()) {
      load(model.material.diffuse_map);
    }
  }
  if (model.material.normal_map) {
    if (textures_.find(model.material.normal_map->id()) == textures_.end()) {
      load(model.material.normal_map);
    }
  }
  if (model.material.light_map) {
    if (textures_.find(model.material.light_map->id()) == textures_.end()) {
      load(model.material.light_map);
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

  if (model.material.diffuse_map) {
    if (textures_.find(model.material.diffuse_map->id()) != textures_.end()) {
      unload(model.material.diffuse_map);
    }
  }
  if (model.material.normal_map) {
    if (textures_.find(model.material.normal_map->id()) != textures_.end()) {
      unload(model.material.normal_map);
    }
  }
  if (model.material.light_map) {
    if (textures_.find(model.material.light_map->id()) != textures_.end()) {
      unload(model.material.light_map);
    }
  }
}

void RenderSystem::load(const SharedTextureCube &texture) {
  if (texture) { // TODO: maybe just send in the texture reference.
    if (texture_cubes_.find(texture->id()) == texture_cubes_.end()) {
      GLuint gl_id = create_texture_cube(texture);
      texture_cubes_.insert({texture->id(), gl_id});
    }
  }
}
void RenderSystem::unload(const SharedTextureCube &texture) {
  if (texture_cubes_.find(texture->id()) != textures_.end()) {
    auto gl_id = texture_cubes_[texture->id()];
    glDeleteTextures(1, &gl_id);
    texture_cubes_.erase(texture->id());
  }
}

void RenderSystem::load(const SharedTexture2D &texture) {
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

void RenderSystem::unload(const SharedTexture2D &texture) {
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

bool RenderSystem::check_shader(const unsigned int shader,
                                const std::string &name) {
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
      std::cerr << "Compile failure in " << types[type] << " " << name
                << " shader" << std::endl;
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

unsigned int RenderSystem::create_texture(const SharedTexture2D &texture) {
  GLuint id;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  GLfloat sampling = texture->mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
  if (texture->format == Texture::Format::DEPTH){
    sampling = GL_NEAREST;
  }

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampling);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampling);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_map[texture->wrap]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_map[texture->wrap]);
  if (glewGetExtension("GL_EXT_texture_filter_anisotropic")) {
    float aniso = 0.0f;
    glBindTexture(GL_TEXTURE_2D, id);
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
  }

  glTexImage2D(GL_TEXTURE_2D, 0, texture->compress ? format_map_compressed[texture->format]
                                                   : format_map[texture->format], texture->width(), texture->height(),
               0, texture->format == Texture::Format::DEPTH ? GL_DEPTH_COMPONENT : GL_RGBA, GL_UNSIGNED_BYTE, texture->data());

  if (texture->mipmaps) {
    glGenerateMipmap(GL_TEXTURE_2D);
  };
  glBindTexture(GL_TEXTURE_2D, 0);
  return id;
}

unsigned int
RenderSystem::create_texture_cube(const SharedTextureCube &texture) {
  GLuint id;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, id);

  GLfloat sampling = texture->mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;

  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, sampling);
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, sampling);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
                  wrap_map[texture->wrap]);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
                  wrap_map[texture->wrap]);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
                  wrap_map[texture->wrap]);

  for (int i = 0; i < 6; i++) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                 texture->compress ? format_map_compressed[texture->format]
                                   : format_map[texture->format],
                 texture->width(), texture->height(), 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, texture->data(i));
  }
  if (texture->mipmaps) {
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  };
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  return id;
}

unsigned int
RenderSystem::create_texture_and_pbo(const SharedTexture2D &texture) {
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
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width(), texture->height(),
               0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
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

void RenderSystem::render_scene(const RenderCamera &camera,
                                const RenderScene &render_scene) {
  glViewport(0, 0, camera.resolution.x, camera.resolution.y);
  glUseProgram(vertex_programs_[render_scene.shader].program);
  for (auto &model : render_scene.models) {
    render(model, render_scene.decals, glm::mat4(1.0f), camera,
           render_scene.light, render_scene.environment, render_scene.fog,
           render_scene.shader, render_scene.draw);
  }

  auto &uniforms = box_programs_.at("box");

  glUseProgram(uniforms.program);
  glBindVertexArray(box_va);

  for (auto &box : render_scene.render_boxes) {
    glm::vec3 size = box.size();
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), box.position);
    glm::mat4 mv = camera.view * transform * glm::scale(glm::mat4(1.0f), size);
    glm::mat4 mvp = camera.projection * camera.view * transform *
                    glm::scale(glm::mat4(1.0f), size);

    glUniformMatrix4fv(uniforms.mvp, 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(uniforms.mv, 1, GL_FALSE, &mv[0][0]);

    // glDrawArrays(GL_POINTS, 0, 16);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
    glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT,
                   (GLvoid *)(4 * sizeof(GLuint)));
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT,
                   (GLvoid *)(8 * sizeof(GLuint)));
  }

  if (vertex_arrays_.find(render_scene.particles.id()) ==
      vertex_arrays_.end()) {
    unsigned int vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    if (array_buffers_.find(render_scene.particles.id()) ==
        array_buffers_.end()) {
      unsigned int array_buffer;
      glGenBuffers(1, &array_buffer);
      glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
      glBufferData(GL_ARRAY_BUFFER,
                   render_scene.particles.size() * sizeof(Particle),
                   render_scene.particles.data(), GL_STREAM_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      array_buffers_.insert({render_scene.particles.id(), array_buffer});
    }
    glBindBuffer(GL_ARRAY_BUFFER, array_buffers_[render_scene.particles.id()]);
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
    vertex_arrays_.insert({render_scene.particles.id(), vertex_array});
  }

  glBindBuffer(GL_ARRAY_BUFFER, array_buffers_[render_scene.particles.id()]);
  glBufferData(GL_ARRAY_BUFFER,
               render_scene.particles.size() * sizeof(Particle),
               render_scene.particles.data(), GL_STREAM_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glm::mat4 mv = camera.view;
  glm::mat4 mvp = camera.projection * camera.view;

  auto &uniforms2 = particle_programs_.at("particles");

  glUseProgram(uniforms2.program);

  glBindVertexArray(vertex_arrays_[render_scene.particles.id()]);

  glUniformMatrix4fv(uniforms2.mvp, 1, GL_FALSE, &mvp[0][0]);
  glUniformMatrix4fv(uniforms2.mv, 1, GL_FALSE, &mv[0][0]);

  glDrawArrays(GL_POINTS, 0, render_scene.particles.size());
}

void RenderSystem::render(const Model &model, const Decals &decals,
                          const glm::mat4 &parent_transform,
                          const RenderCamera &camera, const Light &light,
                          const EnvironmentLight &environment, const Fog &fog,
                          const RenderScene::Shader &shader,
                          const RenderScene::Draw &draw) {
  // glViewport(0, 0, camera.resolution.x, camera.resolution.y);
  static const glm::mat4 bias(0.5, 0.0, 0.0, 0.0,
                              0.0, 0.5, 0.0, 0.0,
                              0.0, 0.0, 0.5, 0.0,
                              0.5, 0.5, 0.5, 1.0);

  load(environment.texture);
  load(model);

  const glm::mat4 mv = camera.view * parent_transform * model.transform;
  const glm::mat4 mvp = camera.projection * mv;

  if (model.mesh) {
    glBindVertexArray(vertex_arrays_.at(model.mesh->id()));
  };

  const auto &uniforms = vertex_programs_.at(shader);

  int texture_unit = 0;

  glActiveTexture(GLenum(GL_TEXTURE0 + texture_unit));
  glBindTexture(GL_TEXTURE_2D, model.material.diffuse_map
                                   ? textures_[model.material.diffuse_map->id()]
                                   : empty_texture_);
  glUniform1i(uniforms.material_diffuse_map, texture_unit);
  texture_unit++;


/*
  for (int i = 0; i < decals.size(); i++){
    auto &decal = decals[i];
    load(decal.material.diffuse_map);
    glActiveTexture(GLenum(GL_TEXTURE0 + texture_unit));
    glBindTexture(GL_TEXTURE_2D,
                  decal.material.diffuse_map
                      ? textures_[decal.material.diffuse_map->id()]
                      : empty_texture_);
    glUniform1i(uniforms.decal_material_diffuse_maps[i], texture_unit);

    const glm::mat4 decal_mvp = bias * decal.projection * decal.view * parent_transform * model.transform;
    glUniformMatrix4fv(uniforms.decal_mvps[i],
                       1, GL_FALSE, &decal_mvp[0][0]);

    if (decal.material.diffuse_map->id() != decals.front().material.diffuse_map->id()){
      texture_unit++;
    }
  }
  texture_unit++;
  */


  // Shadowmap
  if (light.shadow_map) {
    load(light.shadow_map);
    glActiveTexture(GL_TEXTURE0 + texture_unit);
    glBindTexture(GL_TEXTURE_2D, textures_[light.shadow_map->id()]);
    glUniform1i(uniforms.light_shadow_map, texture_unit);
    texture_unit++;
  }

  glActiveTexture(GLenum(GL_TEXTURE0 + texture_unit));
  glBindTexture(GL_TEXTURE_2D, model.material.light_map
                                   ? textures_[model.material.light_map->id()]
                                   : empty_texture_);
  glUniform1i(uniforms.material_light_map, texture_unit);
  texture_unit++;

  glActiveTexture(GLenum(GL_TEXTURE0 + texture_unit));
  glBindTexture(GL_TEXTURE_2D, model.material.normal_map
                                   ? textures_[model.material.normal_map->id()]
                                   : empty_texture_);
  glUniform1i(uniforms.material_normal_map, texture_unit);
  texture_unit++;

  glActiveTexture(GLenum(GL_TEXTURE0 + texture_unit));
  glBindTexture(GL_TEXTURE_CUBE_MAP,
                environment.texture
                    ? environment.texture
                          ? texture_cubes_[environment.texture->id()]
                          : empty_texture_
                    : empty_texture_);
  glUniform1i(uniforms.environment_map, texture_unit);
  texture_unit++;

  glUniform3fv(uniforms.environment_position, 1,
               glm::value_ptr(environment.box.position));
  glUniform3fv(uniforms.environment_extent, 1,
               glm::value_ptr(environment.box.extent));

  glUniformMatrix4fv(uniforms.model_view_projection_matrix, 1, GL_FALSE,
                     &mvp[0][0]);
  glUniformMatrix4fv(uniforms.model_view_matrix, 1, GL_FALSE, &mv[0][0]);
  glUniformMatrix4fv(uniforms.view_matrix, 1, GL_FALSE, &camera.view[0][0]);
  auto model_matrix = parent_transform * model.transform;
  glUniformMatrix4fv(uniforms.model_matrix, 1, GL_FALSE, &model_matrix[0][0]);

  const glm::mat4 depth_bias_mvp =
      bias * light.projection * light.view * model.transform;
  glUniformMatrix4fv(uniforms.depth_bias_mvp, 1, GL_FALSE,
                     &depth_bias_mvp[0][0]);

  glm::mat3 normal_matrix = glm::inverseTranspose(glm::mat3(parent_transform) *
                                                  glm::mat3(model.transform));
  normal_matrix =
      glm::inverseTranspose(glm::mat3(parent_transform * model.transform));
  glUniformMatrix3fv(uniforms.normal_matrix, 1, GL_FALSE, &normal_matrix[0][0]);

  glUniform3fv(uniforms.material_ambient_color, 1,
               glm::value_ptr(model.material.ambient));
  glUniform3fv(uniforms.material_diffuse_color, 1,
               glm::value_ptr(model.material.diffuse));
  glUniform3fv(uniforms.material_specular_color, 1,
               glm::value_ptr(model.material.specular));
  glUniform1fv(uniforms.material_specular_exponent, 1,
               &model.material.specular_exponent);
  glUniform1fv(uniforms.material_opacity, 1, &model.material.opacity);

  // Camera in world space
  glUniform3fv(uniforms.camera_position, 1, glm::value_ptr(camera.position()));

  // Send light in world space
  glUniform3fv(uniforms.light_position, 1,
               glm::value_ptr(glm::vec3(glm::vec4(light.position, 1.0f))));
  glUniform3fv(uniforms.light_diffuse_color, 1, glm::value_ptr(light.diffuse));
  glUniform3fv(uniforms.light_specular_color, 1,
               glm::value_ptr(light.specular));
  glUniformMatrix4fv(uniforms.light_view, 1, GL_FALSE, &light.view[0][0]);
  glUniformMatrix4fv(uniforms.light_projection, 1, GL_FALSE,
                     &light.projection[0][0]);

  glUniform1fv(uniforms.light_linear_attenuation_factor, 1,
               &light.linear_attenuation_factor);
  glUniform1fv(uniforms.light_quadratic_attenuation_factor, 1,
               &light.quadratic_attenuation_factor);

  glUniform2fv(uniforms.camera_resolution, 1,
               glm::value_ptr(camera.resolution));

  glUniform3fv(uniforms.fog_color_near, 1, glm::value_ptr(fog.color_near));
  glUniform3fv(uniforms.fog_color_far, 1, glm::value_ptr(fog.color_far));
  glUniform1fv(uniforms.fog_near, 1, &fog.near);
  glUniform1fv(uniforms.fog_far, 1, &fog.far);
  glUniform1fv(uniforms.fog_linear_factor, 1, &fog.linear_factor);
  glUniform1fv(uniforms.fog_exponential_factor, 1, &fog.exponential_factor);
  glUniform1fv(uniforms.fog_exponential_power, 1, &fog.exponential_power);
  glUniform1fv(uniforms.fog_exponential_attenuation_factor, 1,
               &fog.exponential_attenuation_factor);

  const int num_elements = model.mesh ? model.mesh->elements_size() : 0;
  int draw_type = GL_TRIANGLES;
  if (draw == RenderScene::Draw::LINES) {
    draw_type = GL_LINES;
  } else if (draw == RenderScene::Draw::POINTS) {
    draw_type = GL_POINTS;
  }
  if (model.mesh) {
    if (num_elements > 0) {
      glDrawElements(draw_type, num_elements, GL_UNSIGNED_INT, 0);
    } else {
      glDrawArrays(draw_type, 0, model.mesh->vertices_size());
    }
  }
  for (const auto &child : model.models) {
    render(child, decals, parent_transform * model.transform, camera, light,
           environment, fog, shader, draw);
  }
}

void RenderSystem::render_target(const OptTarget &target) {
  if (target) {
    if (frame_buffers_.find(target->id()) == frame_buffers_.end()) {
      GLuint frame_buffer_id;
      glGenFramebuffers(1, &frame_buffer_id);
      glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);

      if (target->texture) {
        GLuint texture_id = create_texture(target->texture);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, texture_id, 0);
        textures_.insert({target->texture->id(), texture_id});
      }

      if (target->texture_cube) {
        GLuint texture_id = create_texture_cube(target->texture_cube);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X, texture_id, 0);

        texture_cubes_.insert({target->texture_cube->id(), texture_id});
      }
      if (target->depth_texture){
        GLuint texture_id = create_texture(target->depth_texture);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_2D, texture_id, 0);
        textures_.insert({target->depth_texture->id(), texture_id});
      }
      else {
        GLuint depthrenderbuffer_id;
        glGenRenderbuffers(1, &depthrenderbuffer_id);
        glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer_id);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                              target->width(), target->height());
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                  GL_RENDERBUFFER, depthrenderbuffer_id);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
        render_buffers.insert({target->id(), depthrenderbuffer_id});
      }

      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Framebuffer incomplete.");
      }

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

void RenderSystem::render_scenes(
    const std::initializer_list<RenderScene> &batches_init,
    const glm::vec4 &color, const OptTarget &target) {
  render_scenes(batches_init.begin(), batches_init.end(), color, target);
}

RenderSystem::VertexProgramData::VertexProgramData(const GLuint program)
    : program(program), model_view_projection_matrix(glGetUniformLocation(
                            program, "model_view_projection")),
      model_view_matrix(glGetUniformLocation(program, "model_view")),
      model_matrix(glGetUniformLocation(program, "model")),
      view_matrix(glGetUniformLocation(program, "view")),
      normal_matrix(glGetUniformLocation(program, "normal_matrix")),
      depth_bias_mvp(
          glGetUniformLocation(program, "depth_bias_model_view_projection")),
      material_diffuse_map(
          glGetUniformLocation(program, "material.diffuse_map")),
      material_light_map(glGetUniformLocation(program, "material.light_map")),
      material_normal_map(glGetUniformLocation(program, "material.normal_map")),
      environment_map(glGetUniformLocation(program, "environment.texture")),
      environment_position(
          glGetUniformLocation(program, "environment.position")),
      environment_extent(glGetUniformLocation(program, "environment.extent")),
      material_ambient_color(glGetUniformLocation(program, "material.ambient")),
      material_diffuse_color(glGetUniformLocation(program, "material.diffuse")),
      material_specular_color(
          glGetUniformLocation(program, "material.specular")),
      material_specular_exponent(
          glGetUniformLocation(program, "material.specular_exponent")),
      material_opacity(glGetUniformLocation(program, "material.opacity")),
      camera_position(glGetUniformLocation(program, "camera.position")),
      camera_resolution(glGetUniformLocation(program, "camera.resolution")),
      light_position(glGetUniformLocation(program, "light.position")),
      light_diffuse_color(glGetUniformLocation(program, "light.diffuse")),
      light_specular_color(glGetUniformLocation(program, "light.specular")),
      light_view(glGetUniformLocation(program, "light.view")),
      light_projection(glGetUniformLocation(program, "light.projection")),
      light_linear_attenuation_factor(
          glGetUniformLocation(program, "light.linear_attenuation_factor")),
      light_quadratic_attenuation_factor(
          glGetUniformLocation(program, "light.quadratic_attenuation_factor")),
      light_shadow_map(
          glGetUniformLocation(program, "light.shadow_map")),

      fog_color_near(glGetUniformLocation(program, "fog.color_near")),
      fog_color_far(glGetUniformLocation(program, "fog.color_far")),
      fog_near(glGetUniformLocation(program, "fog.near")),
      fog_far(glGetUniformLocation(program, "fog.far")),
      fog_linear_factor(glGetUniformLocation(program, "fog.linear_factor")),
      fog_exponential_factor(
          glGetUniformLocation(program, "fog.exponential_factor")),
      fog_exponential_power(
          glGetUniformLocation(program, "fog.exponential_power")),
      fog_exponential_attenuation_factor(
          glGetUniformLocation(program, "fog.exponential_attenuation_factor"))

      /*,
      decal_mvps{glGetUniformLocation(
          program, "diffuse_decal_model_view_projection_matrix")},
      decal_material_diffuse_maps{
          glGetUniformLocation(program, "decal_material.diffuse_map")}*/ {

  for (int i = 0; i < decal_material_diffuse_maps.size(); i++) {
    auto decals_uniform_name = "decal_materials[" + std::to_string(i) + "].diffuse_map";
    decal_material_diffuse_maps[i] = glGetUniformLocation(program, decals_uniform_name.c_str());

    auto decal_mvps_uniform_name = "decal_model_view_projections[" + std::to_string(i) + "]";
    decal_mvps[i] = glGetUniformLocation(program, decal_mvps_uniform_name.c_str());
  }

}
}
