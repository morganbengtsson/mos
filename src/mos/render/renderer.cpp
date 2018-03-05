#include <GL/glew.h>
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
#include <mos/render/mesh.hpp>
#include <mos/render/model.hpp>
#include <mos/render/renderer.hpp>
#include <mos/util.hpp>

namespace mos {
namespace gfx {

Renderer::Renderer(const glm::vec4 &color) :
    format_map_{
        {Texture::Format::R, {GL_RED, GL_RED}},
        {Texture::Format::RG, {GL_RG, GL_RG}},
        {Texture::Format::SRGB, {GL_SRGB, GL_RGB}},
        {Texture::Format::SRGBA, {GL_SRGB_ALPHA, GL_RGBA}},
        {Texture::Format::RGB, {GL_RGB, GL_RGB}},
        {Texture::Format::RGBA, {GL_RGBA, GL_RGBA}},
        {Texture::Format::DEPTH, {GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT}},
        {Texture::Format::COMPRESSED_SRGB, {GL_COMPRESSED_SRGB, GL_RGB}},
        {Texture::Format::COMPRESSED_SRGBA, {GL_COMPRESSED_SRGB_ALPHA, GL_RGBA}},
        {Texture::Format::COMPRESSED_RGB, {GL_COMPRESSED_RGB, GL_RGB}},
        {Texture::Format::COMPRESSED_RGBA, {GL_COMPRESSED_RGBA, GL_RGBA}},
        {Texture::Format::R16F, {GL_R16F, GL_R}},
        {Texture::Format::RG16F, {GL_RG16F, GL_RG}},
        {Texture::Format::RGB16F, {GL_RGB16F, GL_RGB}},
        {Texture::Format::RGBA16F, {GL_RGBA16F, GL_RGBA}},
        {Texture::Format::R32F, {GL_R32F, GL_R}},
        {Texture::Format::RG32F, {GL_RG32F, GL_RG}},
        {Texture::Format::RGB32F, {GL_RGB32F, GL_RGB}},
        {Texture::Format::RGBA32F, {GL_RGBA32F, GL_RGBA}}},
    wrap_map_{
        {Texture::Wrap::CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE},
        {Texture::Wrap::CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER},
        {Texture::Wrap::REPEAT, GL_REPEAT}},
    draw_map_{{Scene::Draw::LINES, GL_LINES},
              {Scene::Draw::POINTS, GL_POINTS},
              {Scene::Draw::TRIANGLES, GL_TRIANGLES}} {

  glewExperimental = GL_TRUE;
  GLenum err = glewInit();

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

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

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
  add_vertex_program(Scene::Shader::STANDARD, standard_vert_source,
                     standard_frag_source, standard_vert, standard_frag);

  std::string text_vert = "text_330.vert";
  std::string text_frag = "text_330.frag";
  std::string text_vert_source = text(shader_path + text_vert);
  std::string text_frag_source = text(shader_path + text_frag);
  add_vertex_program(Scene::Shader::TEXT, text_vert_source,
                     text_frag_source, text_vert, text_frag);

  std::string depth_vert = "depth_330.vert";
  std::string depth_frag = "depth_330.frag";
  std::string depth_vert_source = text(shader_path + depth_vert);
  std::string depth_frag_source = text(shader_path + depth_frag);
  add_vertex_program(Scene::Shader::DEPTH, depth_vert_source,
                     depth_frag_source, depth_vert, depth_frag);

  std::string effect_vert = "effect_330.vert";
  std::string effect_frag = "effect_330.frag";
  add_vertex_program(Scene::Shader::EFFECT,
                     text(shader_path + effect_vert),
                     text(shader_path + effect_frag), effect_vert, effect_frag);

  std::string blur_vert = "blur_330.vert";
  std::string blur_frag = "blur_330.frag";
  add_vertex_program(Scene::Shader::BLUR, text(shader_path + blur_vert),
                     text(shader_path + blur_frag), blur_vert, blur_frag);

  std::string crt_vert = "crt_330.vert";
  std::string crt_frag = "crt_330.frag";
  add_vertex_program(Scene::Shader::CRT, text(shader_path + crt_vert),
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
      -0.5, -0.5, -0.5, 1.0, 0.5, -0.5, -0.5, 1.0, 0.5, 0.5, -0.5,
      1.0, -0.5, 0.5, -0.5, 1.0, -0.5, -0.5, 0.5, 1.0, 0.5, -0.5,
      0.5, 1.0, 0.5, 0.5, 0.5, 1.0, -0.5, 0.5, 0.5, 1.0,
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

  glGenTextures(1, &black_texture_);
  glBindTexture(GL_TEXTURE_2D, black_texture_);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  auto data = std::array<unsigned char, 4>{0, 0, 0, 0};
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               data.data());
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenTextures(1, &white_texture_);
  glBindTexture(GL_TEXTURE_2D, white_texture_);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  auto data_white = std::array<unsigned char, 4>{255, 255, 255, 255};
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
               data_white.data());
  glBindTexture(GL_TEXTURE_2D, 0);

  auto brdf_lut_texture = Texture2D("assets/brdfLUT.png", false);
  brdf_lut_texture.format = Texture::Format::RGB;
  brdf_lut_texture.wrap = Texture::Wrap::CLAMP_TO_EDGE;
  brdf_lut_texture_ = create_texture(brdf_lut_texture);
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
    glDeleteBuffers(1, &ab.second.id);
  }

  for (auto &eab : element_array_buffers_) {
    glDeleteBuffers(1, &eab.second.id);
  }

  for (auto &va : vertex_arrays_) {
    glDeleteVertexArrays(1, &va.second);
  }
}

void Renderer::add_box_program(const std::string &name,
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

void Renderer::create_depth_program() {
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

void Renderer::add_particle_program(const std::string name,
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
          glGetUniformLocation(program, "model_view"),
          glGetUniformLocation(program, "projection"),
          glGetUniformLocation(program, "tex"),
          glGetUniformLocation(program, "resolution")}));
}

void Renderer::add_vertex_program(const Scene::Shader shader,
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

  std::cout << "Linking program" << std::endl;
  glLinkProgram(program);
  check_program(program);

  vertex_programs_.insert(
      VertexProgramPair(shader, VertexProgramData(program)));
}

void Renderer::load(const Model &model) {
  if (model.mesh &&
      vertex_arrays_.find(model.mesh->id()) == vertex_arrays_.end()) {
    unsigned int vertex_array;
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    if (array_buffers_.find(model.mesh->id()) == array_buffers_.end()) {
      unsigned int array_buffer_id;
      glGenBuffers(1, &array_buffer_id);
      glBindBuffer(GL_ARRAY_BUFFER, array_buffer_id);
      glBufferData(GL_ARRAY_BUFFER,
                   model.mesh->vertices.size() * sizeof(Vertex),
                   model.mesh->vertices.data(), GL_STATIC_DRAW);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      array_buffers_.insert({model.mesh->id(), Buffer{array_buffer_id, model.mesh->modified()}});
    }
    if (element_array_buffers_.find(model.mesh->id()) ==
        element_array_buffers_.end()) {
      unsigned int element_array_buffer_id;
      glGenBuffers(1, &element_array_buffer_id);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffer_id);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                   model.mesh->indices.size() * sizeof(unsigned int),
                   model.mesh->indices.data(), GL_STATIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
      element_array_buffers_.insert({model.mesh->id(), Buffer{element_array_buffer_id, model.mesh->modified()}});
    }
    glBindBuffer(GL_ARRAY_BUFFER, array_buffers_.at(model.mesh->id()).id);
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

    // AO
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<const void *>(sizeof(glm::vec3) * 3 +
                              sizeof(glm::vec2)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
                 element_array_buffers_.at(model.mesh->id()).id);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glBindVertexArray(0);
    vertex_arrays_.insert({model.mesh->id(), vertex_array});
  }

  if (model.mesh && (model.mesh->modified() > array_buffers_[model.mesh->id()].modified)) {
    glBindBuffer(GL_ARRAY_BUFFER, array_buffers_[model.mesh->id()].id);
    glBufferData(GL_ARRAY_BUFFER, model.mesh->vertices.size() * sizeof(Vertex),
                 model.mesh->vertices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (model.mesh->indices.size() > 0) {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array_buffers_[model.mesh->id()].id);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                   model.mesh->indices.size() * sizeof(unsigned int),
                   model.mesh->indices.data(),
                   GL_DYNAMIC_DRAW);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

  }
  load(model.material.albedo_map);
  load(model.material.normal_map);
  load(model.material.metallic_map);
  load(model.material.roughness_map);
  load(model.material.ambient_occlusion_map);
}

void Renderer::unload(const Model &model) {
  if (vertex_arrays_.find(model.mesh->id()) != vertex_arrays_.end()) {
    auto va_id = vertex_arrays_[model.mesh->id()];
    glDeleteVertexArrays(1, &va_id);
    vertex_arrays_.erase(model.mesh->id());

    if (array_buffers_.find(model.mesh->id()) != array_buffers_.end()) {
      auto abo = array_buffers_[model.mesh->id()];
      glDeleteBuffers(1, &abo.id);
      array_buffers_.erase(model.mesh->id());
    }
    if (element_array_buffers_.find(model.mesh->id()) !=
        element_array_buffers_.end()) {
      auto ebo = element_array_buffers_[model.mesh->id()];
      glDeleteBuffers(1, &ebo.id);
      element_array_buffers_.erase(model.mesh->id());
    }
  }
  unload(model.material.albedo_map);
  unload(model.material.normal_map);
}

void Renderer::unload(const SharedTextureCube &texture) {
  if (texture) {
    if (texture_cubes_.find(texture->id()) != textures_.end()) {
      auto gl_id = texture_cubes_[texture->id()];
      glDeleteTextures(1, &gl_id);
      texture_cubes_.erase(texture->id());
    }
  }
}

void Renderer::load(const Texture2D &texture) {
  if (textures_.find(texture.id()) == textures_.end()) {
    GLuint gl_id = create_texture(texture);
    textures_.insert({texture.id(), gl_id});
  }
}

void Renderer::load(const SharedTexture2D &texture) {
#ifdef STREAM_TEXTURES
  if (textures_.find(texture->id()) == textures_.end()) {
    GLuint gl_id = create_texture_and_pbo(texture);
    textures_.insert({texture->id(), gl_id});
  }
#else
  if (texture) {
    load(*texture);
  }
#endif
}

void Renderer::unload(const SharedTexture2D &texture) {
  if (texture) {
    if (textures_.find(texture->id()) != textures_.end()) {
      auto gl_id = textures_[texture->id()];
      glDeleteTextures(1, &gl_id);
      textures_.erase(texture->id());
    }
  }
}

void Renderer::clear_buffers() {
  for (auto &texture : textures_) {
    glDeleteTextures(1, &texture.second);
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

bool Renderer::check_shader(const unsigned int shader,
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

unsigned int Renderer::create_texture(const Texture2D &texture) {
  GLuint id;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  GLfloat sampling = texture.mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
  if (texture.format == Texture::Format::DEPTH) {
    sampling = GL_LINEAR;
  }

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, sampling);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, sampling);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_map_.at(texture.wrap));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_map_.at(texture.wrap));
  if (glewGetExtension("GL_EXT_texture_filter_anisotropic")) {
    float aniso = 0.0f;
    glBindTexture(GL_TEXTURE_2D, id);
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
  }

  glTexImage2D(GL_TEXTURE_2D, 0,
               format_map_[texture.format].internal_format,
               texture.width(), texture.height(), 0,
               format_map_[texture.format].format,
               GL_UNSIGNED_BYTE, texture.data());

  if (texture.mipmaps) {
    glGenerateMipmap(GL_TEXTURE_2D);
  };
  glBindTexture(GL_TEXTURE_2D, 0);
  return id;
}

unsigned int Renderer::create_texture(const SharedTexture2D &texture) {
  return create_texture(*texture);
}

unsigned int
Renderer::create_texture_cube(const TextureCube &texture) {
  GLuint id;
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, id);

  GLfloat sampling = texture.mipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;

  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, sampling);
  glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, sampling);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,
                  wrap_map_[texture.wrap]);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,
                  wrap_map_[texture.wrap]);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,
                  wrap_map_[texture.wrap]);

  for (int i = 0; i < 6; i++) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                 format_map_[texture.format].internal_format,
                 texture.width(), texture.height(), 0, format_map_[texture.format].format,
                 GL_UNSIGNED_BYTE, texture.data(i));
  }
  if (texture.mipmaps) {
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  };
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  return id;
}

unsigned int
Renderer::create_texture_and_pbo(const SharedTexture2D &texture) {
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

void Renderer::render_scene(const Camera &camera,
                                const Scene &render_scene,
                                const glm::vec2 &resolution) {
  glViewport(0, 0, resolution.x, resolution.y);
  glUseProgram(vertex_programs_[render_scene.shader].program);
  for (auto &model : render_scene.models) {
    render(model, render_scene.decals, glm::mat4(1.0f), camera,
           render_scene.light, render_scene.environment, render_scene.fog,
           resolution, render_scene.shader, render_scene.draw);
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
                   (GLvoid *) (4 * sizeof(GLuint)));
    glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT,
                   (GLvoid *) (8 * sizeof(GLuint)));
  }

  for (auto &particles : render_scene.particle_clouds) {

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
        array_buffers_.insert({particles.id(), Buffer{array_buffer, particles.modified_}});
      }
      glBindBuffer(GL_ARRAY_BUFFER, array_buffers_[particles.id()].id);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), 0);
      glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle),
                            reinterpret_cast<const void *>(sizeof(glm::vec3)));
      glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle),
                            reinterpret_cast<const void *>(sizeof(glm::vec3) +
                                sizeof(glm::vec4)));
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glEnableVertexAttribArray(2);
      glBindVertexArray(0);
      vertex_arrays_.insert({particles.id(), vertex_array});
    }
    glBindBuffer(GL_ARRAY_BUFFER, array_buffers_[particles.id()].id);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(Particle),
                 particles.data(), GL_STREAM_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glm::mat4 mv = camera.view;
    glm::mat4 mvp = camera.projection * camera.view;

    auto &uniforms2 = particle_programs_.at("particles");

    glUseProgram(uniforms2.program);

    glBindVertexArray(vertex_arrays_[particles.id()]);

    load(particles.emission_map);
    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, particles.emission_map
                                 ? textures_[particles.emission_map->id()]
                                 : black_texture_);
    glUniform1i(uniforms2.texture, 10);

    glUniformMatrix4fv(uniforms2.mvp, 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(uniforms2.mv, 1, GL_FALSE, &mv[0][0]);
    glUniformMatrix4fv(uniforms2.p, 1, GL_FALSE, &camera.projection[0][0]);
    glUniform2fv(uniforms2.resolution, 1, glm::value_ptr(resolution));

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDrawArrays(GL_POINTS, 0, particles.size());
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
}

void Renderer::render(const Model &model, const Scene::Decals &decals,
                          const glm::mat4 &parent_transform,
                          const Camera &camera, const Light &light,
                          const EnvironmentLight &environment, const Fog &fog,
                          const glm::vec2 &resolution,
                          const Scene::Shader &shader,
                          const Scene::Draw &draw) {
  // glViewport(0, 0, camera.resolution.x, camera.resolution.y);
  static const glm::mat4 bias(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0,
                              0.5, 0.0, 0.5, 0.5, 0.5, 1.0);

  //load(environment.texture);
  load(model);

  const glm::mat4 mv = camera.view * parent_transform * model.transform;
  const glm::mat4 mvp = camera.projection * mv;

  if (model.mesh) {
    glBindVertexArray(vertex_arrays_.at(model.mesh->id()));
  };

  const auto &uniforms = vertex_programs_.at(shader);

  glActiveTexture(GLenum(GL_TEXTURE0));
  glBindTexture(GL_TEXTURE_2D, model.material.albedo_map
                               ? textures_[model.material.albedo_map->id()]
                               : black_texture_);
  glUniform1i(uniforms.material_albedo_map, 0);


  // Shadowmap
  if (light.shadow_map) {
    load(light.shadow_map);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures_[light.shadow_map->id()]);
    glGenerateMipmap(GL_TEXTURE_2D);
    glUniform1i(uniforms.light_shadow_map, 1);
  } else {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, white_texture_);
    glUniform1i(uniforms.light_shadow_map, 1);
  }

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, model.material.normal_map
                               ? textures_[model.material.normal_map->id()]
                               : black_texture_);
  glUniform1i(uniforms.material_normal_map, 2);

  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture_cubes_[environment.texture.id()]);
  glUniform1i(uniforms.environment_map, 3);

  glActiveTexture(GL_TEXTURE4);
  glBindTexture(GL_TEXTURE_2D, model.material.metallic_map
                               ? textures_[model.material.metallic_map->id()]
                               : black_texture_);
  glUniform1i(uniforms.material_metallic_map, 4);

  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, model.material.roughness_map
                               ? textures_[model.material.roughness_map->id()]
                               : black_texture_);
  glUniform1i(uniforms.material_roughness_map, 5);

  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_2D, model.material.ambient_occlusion_map
                               ? textures_[model.material.ambient_occlusion_map->id()]
                               : white_texture_);
  glUniform1i(uniforms.material_ambient_occlusion_map, 6);

  glActiveTexture(GL_TEXTURE7);
  glBindTexture(GL_TEXTURE_2D, brdf_lut_texture_);
  glUniform1i(uniforms.brdf_lut, 7);

  for (int i = 0; i < decals.size(); i++) {
    auto &decal = decals[i];
    load(decal.material.albedo_map);
    load(decal.material.normal_map);

    glActiveTexture(GL_TEXTURE0 + 8 + i);
    glBindTexture(GL_TEXTURE_2D,
                  decal.material.albedo_map
                  ? textures_[decal.material.albedo_map->id()]
                  : black_texture_);
    glUniform1i(uniforms.decal_material_diffuse_maps[i], 8 + i);

    glActiveTexture(GL_TEXTURE0 + 18 + i);
    glBindTexture(GL_TEXTURE_2D,
                  decal.material.normal_map
                  ? textures_[decal.material.normal_map->id()]
                  : black_texture_);
    glUniform1i(uniforms.decal_material_normal_maps[i], 18 + i);

    const glm::mat4 decal_mvp = bias * decal.projection * decal.view *
        parent_transform * model.transform;
    glUniformMatrix4fv(uniforms.decal_mvps[i], 1, GL_FALSE, &decal_mvp[0][0]);
  }

  glUniform3fv(uniforms.environment_position, 1,
               glm::value_ptr(environment.box.position));
  glUniform3fv(uniforms.environment_extent, 1,
               glm::value_ptr(environment.box.extent));
  glUniform1fv(uniforms.environment_strength, 1,
               &environment.strength);

  glUniformMatrix4fv(uniforms.model_view_projection_matrix, 1, GL_FALSE,
                     &mvp[0][0]);
  glUniformMatrix4fv(uniforms.model_view_matrix, 1, GL_FALSE, &mv[0][0]);
  glUniformMatrix4fv(uniforms.view_matrix, 1, GL_FALSE, &camera.view[0][0]);
  auto model_matrix = parent_transform * model.transform;
  glUniformMatrix4fv(uniforms.model_matrix, 1, GL_FALSE, &model_matrix[0][0]);

  const glm::mat4 depth_bias_mvp = bias * light.camera.projection *
      light.camera.view * parent_transform *
      model.transform;
  glUniformMatrix4fv(uniforms.depth_bias_mvp, 1, GL_FALSE,
                     &depth_bias_mvp[0][0]);

  glm::mat3 normal_matrix = glm::inverseTranspose(glm::mat3(parent_transform) *
      glm::mat3(model.transform));
  normal_matrix =
      glm::inverseTranspose(glm::mat3(parent_transform * model.transform));
  glUniformMatrix3fv(uniforms.normal_matrix, 1, GL_FALSE, &normal_matrix[0][0]);

  glUniform3fv(uniforms.material_albedo, 1,
               glm::value_ptr(model.material.albedo));
  glUniform1fv(uniforms.material_roughness, 1,
               &model.material.roughness);
  glUniform1fv(uniforms.material_metallic, 1,
               &model.material.metallic);
  glUniform1fv(uniforms.material_opacity, 1, &model.material.opacity);
  auto emission = model.material.emission_strength * model.material.emission;
  glUniform3fv(uniforms.material_emission, 1, glm::value_ptr(emission));
  glUniform1fv(uniforms.material_ambient_occlusion, 1, &model.material.ambient_occlusion);

  // Camera in world space
  glUniform3fv(uniforms.camera_position, 1, glm::value_ptr(camera.position()));

  // Send light in world space
  glUniform3fv(uniforms.light_position, 1,
               glm::value_ptr(glm::vec3(glm::vec4(light.position(), 1.0f))));
  auto light_color =
      light.color * light.strength / 11.5f; // 11.5 divider is for same light strength as in Blender/cycles.
  glUniform3fv(uniforms.light_color, 1, glm::value_ptr(light_color));

  glUniformMatrix4fv(uniforms.light_view, 1, GL_FALSE,
                     &light.camera.view[0][0]);
  glUniformMatrix4fv(uniforms.light_projection, 1, GL_FALSE,
                     &light.camera.projection[0][0]);

  auto light_angle = light.angle();
  glUniform1fv(uniforms.light_angle, 1, &light_angle);
  glUniform3fv(uniforms.light_direction, 1, glm::value_ptr(light.direction()));

  glUniform2fv(uniforms.camera_resolution, 1, glm::value_ptr(resolution));

  glUniform3fv(uniforms.fog_color_near, 1, glm::value_ptr(fog.color_near));
  glUniform3fv(uniforms.fog_color_far, 1, glm::value_ptr(fog.color_far));
  glUniform1fv(uniforms.fog_attenuation_factor, 1,
               &fog.attenuation_factor);

  const int num_elements = model.mesh ? model.mesh->indices.size() : 0;
  const int draw_type = draw_map_[draw];
  if (model.mesh) {
    if (num_elements > 0) {
      glDrawElements(draw_type, num_elements, GL_UNSIGNED_INT, 0);
    } else {
      glDrawArrays(draw_type, 0, model.mesh->vertices.size());
    }
  }
  for (const auto &child : model.models) {
    render(child, decals, parent_transform * model.transform, camera, light,
           environment, fog, resolution, shader, draw);
  }
}

void Renderer::clear(const glm::vec4 &color) {
  glClearDepthf(1.0f);
  glClearColor(color.r, color.g, color.b, color.a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::render_scenes(
    const std::initializer_list<Scene> &batches_init,
    const glm::vec4 &color, const glm::ivec2 &resolution) {
  render_scenes(batches_init.begin(), batches_init.end(), color, resolution);
}

void Renderer::render_shadow_map(const Scene &scene) {
  if (frame_buffers_.find(scene.light.target.id()) == frame_buffers_.end()) {
    GLuint frame_buffer_id;
    glGenFramebuffers(1, &frame_buffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);

    GLuint texture_id = create_texture(scene.light.shadow_map);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, texture_id, 0);
    textures_.insert({scene.light.shadow_map->id(), texture_id});

    GLuint depthrenderbuffer_id;
    glGenRenderbuffers(1, &depthrenderbuffer_id);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                          scene.light.shadow_map->width(),
                          scene.light.shadow_map->height());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, depthrenderbuffer_id);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    render_buffers.insert({scene.light.target.id(), depthrenderbuffer_id});

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      throw std::runtime_error("Framebuffer incomplete.");
    }

    frame_buffers_.insert({scene.light.target.id(), frame_buffer_id});
  }
  auto fb = frame_buffers_[scene.light.target.id()];
  glBindFramebuffer(GL_FRAMEBUFFER, fb);
  clear(glm::vec4(0.0f));
  auto shadow_scene = scene;
  shadow_scene.shader = Scene::Shader::DEPTH;

  render_scene(scene.light.camera,
               shadow_scene,
               glm::ivec2(scene.light.shadow_map->width(), scene.light.shadow_map->height()));
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Renderer::render_environment(const Scene &scene, const glm::vec4 &clear_color) {
  if (frame_buffers_.find(scene.environment.target.id()) == frame_buffers_.end()) {
    GLuint frame_buffer_id;
    glGenFramebuffers(1, &frame_buffer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);

    GLuint texture_id = create_texture_cube(scene.environment.texture);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_CUBE_MAP_POSITIVE_X, texture_id, 0);
    //texture_cubes_.insert({scene.environment.texture->id(), texture_id});

    texture_cubes_ = {{scene.environment.texture.id(), texture_id}};

    GLuint depthrenderbuffer_id;
    glGenRenderbuffers(1, &depthrenderbuffer_id);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer_id);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                          scene.environment.texture.width(),
                          scene.environment.texture.height());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, depthrenderbuffer_id);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    render_buffers.insert({scene.environment.target.id(), depthrenderbuffer_id});

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
      throw std::runtime_error("Framebuffer incomplete.");
    }
    frame_buffers_.insert({scene.environment.target.id(), frame_buffer_id});
  }

  GLuint frame_buffer_id = frame_buffers_[scene.environment.target.id()];
  glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);

  auto texture_id = texture_cubes_[scene.environment.texture.id()];

  for (auto c_it = scene.environment.cube_camera.cameras.begin(); c_it != scene.environment.cube_camera.cameras.end();
       c_it++) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_CUBE_MAP_POSITIVE_X
                               + std::distance(scene.environment.cube_camera.cameras.begin(), c_it), texture_id, 0);
    clear(clear_color);
    auto resolution = glm::vec2(scene.environment.texture.width(), scene.environment.texture.height());
    render_scene(*c_it, scene, resolution);
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}

Renderer::VertexProgramData::VertexProgramData(const GLuint program)
    : program(program), model_view_projection_matrix(glGetUniformLocation(
    program, "model_view_projection")),
      model_view_matrix(glGetUniformLocation(program, "model_view")),
      model_matrix(glGetUniformLocation(program, "model")),
      view_matrix(glGetUniformLocation(program, "view")),
      normal_matrix(glGetUniformLocation(program, "normal_matrix")),
      depth_bias_mvp(
          glGetUniformLocation(program, "depth_bias_model_view_projection")),
      environment_map(glGetUniformLocation(program, "environment.texture")),
      environment_position(
          glGetUniformLocation(program, "environment.position")),
      environment_extent(glGetUniformLocation(program, "environment.extent")),
      environment_strength(glGetUniformLocation(program, "environment.strength")),

      material_albedo_map(
          glGetUniformLocation(program, "material.albedo_map")),
      material_normal_map(glGetUniformLocation(program, "material.normal_map")),
      material_metallic_map(glGetUniformLocation(program, "material.metallic_map")),
      material_roughness_map(glGetUniformLocation(program, "material.roughness_map")),
      material_ambient_occlusion_map(glGetUniformLocation(program, "material.ambient_occlusion_map")),

      material_albedo(glGetUniformLocation(program, "material.albedo")),
      material_roughness(
          glGetUniformLocation(program, "material.roughness")),
      material_metallic(
          glGetUniformLocation(program, "material.metallic")),
      material_opacity(glGetUniformLocation(program, "material.opacity")),
      material_emission(glGetUniformLocation(program, "material.emission")),
      material_ambient_occlusion(glGetUniformLocation(program, "material.ambient_occlusion")),

      camera_position(glGetUniformLocation(program, "camera.position")),
      camera_resolution(glGetUniformLocation(program, "camera.resolution")),
      light_position(glGetUniformLocation(program, "light.position")),
      light_color(glGetUniformLocation(program, "light.color")),
      light_view(glGetUniformLocation(program, "light.view")),
      light_projection(glGetUniformLocation(program, "light.projection")),
      light_shadow_map(glGetUniformLocation(program, "light.shadow_map")),
      light_angle(glGetUniformLocation(program, "light.angle")),
      light_direction(glGetUniformLocation(program, "light.direction")),

      fog_color_near(glGetUniformLocation(program, "fog.color_near")),
      fog_color_far(glGetUniformLocation(program, "fog.color_far")),
      fog_attenuation_factor(
          glGetUniformLocation(program, "fog.attenuation_factor")),
      brdf_lut(
          glGetUniformLocation(program, "brdf_lut")) {

  for (int i = 0; i < decal_material_diffuse_maps.size(); i++) {
    auto decals_uniform_name =
        "decal_materials[" + std::to_string(i) + "].albedo_map";
    decal_material_diffuse_maps[i] =
        glGetUniformLocation(program, decals_uniform_name.c_str());

    auto normal_map_name =
        "decal_materials[" + std::to_string(i) + "].normal_map";
    decal_material_normal_maps[i] =
        glGetUniformLocation(program, normal_map_name.c_str());

    auto decal_mvps_uniform_name =
        "decal_model_view_projections[" + std::to_string(i) + "]";
    decal_mvps[i] =
        glGetUniformLocation(program, decal_mvps_uniform_name.c_str());
  }
}
}
}
