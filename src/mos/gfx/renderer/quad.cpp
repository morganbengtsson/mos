#include <mos/gfx/renderer.hpp>

namespace mos::gfx {

Renderer::Quad::Quad()
    : vertex_array(generate(glGenVertexArrays)),
      buffer(generate(glGenBuffers)) {
  static const float quad_vertices[] = {-1.0f, 1.0f, 0.0f, 1.0f,  -1.0f, -1.0f,
                                        0.0f,  0.0f, 1.0f, -1.0f, 1.0f,  0.0f,
                                        -1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  -1.0f,
                                        1.0f,  0.0f, 1.0f, 1.0f,  1.0f,  1.0f};
  glBindVertexArray(vertex_array);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), &quad_vertices,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)nullptr);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
Renderer::Quad::~Quad() {
  glDeleteBuffers(1, &buffer);
  glDeleteVertexArrays(1, &vertex_array);
}
} // namespace mos::gfx
