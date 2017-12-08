#include <mos/render/decal.hpp>
mos::Decal::Decal(const glm::mat4 &projection,
                  const glm::mat4 &view,
                  const mos::Material &material)
    : projection(projection), view(view), material(material) {
  if (material.albedo_map) {
    material.albedo_map->wrap = mos::Texture2D::Wrap::CLAMP_TO_BORDER;
  }
}
