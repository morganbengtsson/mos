#include <mos/gfx/gpu/model.hpp>

glm::vec3 mos::gfx::gpu::Model::position() const {
  return glm::vec3(transform[3]);
}
