#include <mos/gfx/gpu/texture_2d.hpp>

namespace mos::gfx::gpu {

Texture_2D::Texture_2D(const mos::gfx::Texture_2D &texture)
    : id(texture.id()) {}
}
