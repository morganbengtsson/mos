#include <mos/render/decal.hpp>
mos::Decal::Decal(const glm::mat4 &transform,
                  const glm::mat4 &projection,
                  const glm::mat4 &view,
                  const mos::SharedTexture &texture)
    : transform(transform), projection(projection), view(view), texture(texture) {
  texture->wrap = mos::Texture::Wrap::CLAMP_TO_BORDER;
}
