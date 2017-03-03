#include <mos/render/decal.hpp>
mos::Decal::Decal(const glm::mat4 &projection,
                  const glm::mat4 &view,
                  const mos::SharedTexture &texture)
    : projection(projection), view(view), texture(texture) {
  texture->wrap = mos::Texture::Wrap::CLAMP_TO_BORDER;
}
