#include <mos/gfx/decal.hpp>
namespace mos {
namespace gfx {
Decal::Decal(const glm::mat4 &projection,
                  const glm::mat4 &view,
                  const Material &material)
    : projection(projection), view(view), material(material) {
  if (material.albedo_map) {
    material.albedo_map->wrap = Texture2D::Wrap::CLAMP_TO_BORDER;
  }
  if (material.normal_map) {
    material.normal_map->wrap = Texture2D::Wrap::CLAMP_TO_EDGE;
  }
}
Decal::Decal() {
}
}
}