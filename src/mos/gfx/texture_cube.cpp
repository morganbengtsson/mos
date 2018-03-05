#include <mos/gfx/texture_cube.hpp>

namespace mos {
namespace gfx {

TextureCube::TextureCube(const int width, const int height, const bool mipmaps, const Texture::Format &format)
    : Texture({Data(), Data(), Data(), Data(), Data(), Data()}, width, height, format, Wrap::CLAMP_TO_BORDER, mipmaps) {
}
}
}
