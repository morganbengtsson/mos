#pragma once

#include <glm/gtx/matrix_decompose.hpp>
#include <mos/gfx/model.hpp>
#include <vector>

#include <mos/gpu/material.hpp>
#include <mos/gpu/mesh.hpp>

namespace mos::gfx::gl {
class Renderer;
}

namespace mos::gfx::gpu {

class Model;

using Models = std::vector<mos::gfx::gpu::Model>;

class Model final {
  friend class mos::gfx::gl::Renderer;

private:
  explicit Model(mos::gfx::Model model);

public:
  Model() = default;
  /** Loaded mesh **/
  Mesh mesh = Mesh();

  /** Material. */
  Material material;

  /** Transform. */
  glm::mat4 transform{0.0f};

  /** Children models. */
  Models models{};

  /** Get centroid position. */
  auto centroid() const -> glm::vec3;

  /** Get radious of bounding sphere */
  auto radius() const -> float;

  auto position() const -> glm::vec3;
};
} // namespace mos::gfx::gpu
