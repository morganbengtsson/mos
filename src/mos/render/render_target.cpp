#include <mos/render/render_target.hpp>
#include <mos/render/texture_cube.hpp>

namespace mos {

std::atomic_uint RenderTarget::current_id_(0);

unsigned int RenderTarget::id() const { return id_; }

RenderTarget::RenderTarget(const SharedTexture &texture,
                           const SharedTexture &depth_texture,
                           const SharedTextureCube texture_cube) :
    texture(texture),
    depth_texture(depth_texture),
    texture_cube(texture_cube) , id_(current_id_++) {

}
int RenderTarget::width() const {
  return texture ? texture->width() : texture_cube ? texture_cube->width() : 0;
}
int RenderTarget::height() const {
  return texture ? texture->height() : texture_cube ? texture_cube->height() : 0;
}
}
