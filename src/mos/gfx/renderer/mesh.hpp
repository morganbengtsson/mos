#pragma once

#include <mos/gfx/mesh.hpp>

class Mesh final {
  friend class Model;
  friend class Renderer;
private:
  Mesh(const mos::gfx::Mesh& mesh);
public:
  Mesh() = default;
  glm::vec3 centroid{0.0f};
  float radius{0.0f};
  int indices_size = 0;
  int id = -1;

};
