#include <mos/render/texture_cube.hpp>
#include <filesystem/path.h>
namespace mos {
namespace gfx {
TextureCube::TextureCube(const int width, const int height, const bool mipmaps, const Texture::Format &format)
    : Texture({Data(), Data(), Data(), Data(), Data(), Data()}, width, height, format, Wrap::CLAMP_TO_BORDER, mipmaps) {
}
}
}
