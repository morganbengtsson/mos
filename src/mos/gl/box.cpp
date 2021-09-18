#include <mos/gl/renderer.hpp>
#include <array>

namespace mos::gl {

Box::Box()
    : buffer(Renderer::generate(glGenBuffers)), element_buffer(Renderer::generate(glGenBuffers)),
      vertex_array(Renderer::generate(glGenVertexArrays)) {
  std::array vertices{
      -0.5f, -0.5f, -0.5f, 1.0f,  0.5f, -0.5f, -0.5f, 1.0f, 0.5f, 0.5f, -0.5f,
      1.0f,  -0.5f, 0.5f,  -0.5f, 1.0f, -0.5f, -0.5f, 0.5f, 1.0f, 0.5f, -0.5f,
      0.5f,  1.0f,  0.5f,  0.5f,  0.5f, 1.0f,  -0.5f, 0.5f, 0.5f, 1.0f,
  };

  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  std::array elements{0u, 1u, 2u, 3u, 4u, 5u, 6u, 7u, 0u, 4u, 1u, 5u, 2u, 6u, 3u, 7u};

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * elements.size(), elements.data(),
               GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glBindVertexArray(vertex_array);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
  glBindVertexArray(0);
}

Box::~Box() {
  glDeleteBuffers(1, &buffer);
  glDeleteVertexArrays(1, &vertex_array);
  glDeleteBuffers(1, &element_buffer);
}
} // namespace mos::gfx
