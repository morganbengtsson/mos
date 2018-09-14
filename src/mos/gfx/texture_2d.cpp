#include <iostream>
#include <mos/gfx/texture_2d.hpp>

namespace mos {
namespace gfx {

Texture_2D::Texture_2D(const unsigned int width, const unsigned int height,
                     const Format &format, const Wrap &wrap,
                     const bool mipmaps)
    : Texture(width, height, format, wrap, mipmaps) {}

Texture_2D::~Texture_2D() {}

Shared_texture_2D Texture_2D::load(const std::string &path, const bool color_data, const bool mipmaps, const Wrap &wrap) {
  if (path.empty() || path.back() == '/') {
    return Shared_texture_2D();
  }
  return std::make_shared<Texture_2D>(path, color_data, mipmaps, wrap);
}

Texture_2D::Texture_2D(const std::string &path,
                     const bool color_data,
                     const bool mipmaps,
                     const Texture_2D::Wrap &wrap)
    : Texture({path}, color_data, wrap, mipmaps) {}
}
}