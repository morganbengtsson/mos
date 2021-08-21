#pragma once

#include <mos/gfx/mesh.hpp>

class Mesh final {
  friend class Model;
  friend class Renderer;
private:
  Mesh(mos::gfx::Mesh mesh) : centroid(mesh.centroid), radius(mesh.radius), indices_size(mesh.indices.size()), id(mesh.id()) {}
public:
  Mesh() = default;
  glm::vec3 centroid{0.0f};
  float radius{0.0f};
  int indices_size = 0;
  int id = -1;

};
