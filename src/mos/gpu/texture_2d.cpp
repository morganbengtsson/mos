#include <mos/gpu/texture_2d.hpp>

namespace mos::gfx::gpu {

Texture_2D::Texture_2D() : Resource(invalid_id) {

}

Texture_2D::Texture_2D(const mos::gfx::Texture_2D &texture)
    : Resource(texture.id()) {

}

Texture_2D::Texture_2D(const Shared_texture_2D &shared_texture) : Resource(shared_texture ? shared_texture->id() : invalid_id) {

}
}
