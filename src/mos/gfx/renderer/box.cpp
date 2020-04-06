#include <mos/gfx/renderer.hpp>

namespace mos::gfx {

Renderer::Box::Box()
    : buffer(generate(glGenBuffers)), element_buffer(generate(glGenBuffers)),
      vertex_array(generate(glGenVertexArrays)) {
  // Render boxes
  float vertices[] = {
      -0.5, -0.5, -0.5, 1.0,  0.5, -0.5, -0.5, 1.0, 0.5, 0.5, -0.5,
      1.0,  -0.5, 0.5,  -0.5, 1.0, -0.5, -0.5, 0.5, 1.0, 0.5, -0.5,
      0.5,  1.0,  0.5,  0.5,  0.5, 1.0,  -0.5, 0.5, 0.5, 1.0,
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
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
  glBindVertexArray(0);
}

Renderer::Box::~Box() {
  glDeleteBuffers(1, &buffer);
  glDeleteVertexArrays(1, &vertex_array);
  glDeleteBuffers(1, &element_buffer);
}
} // namespace mos::gfx
