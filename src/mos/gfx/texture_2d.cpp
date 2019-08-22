#include <iostream>
#include <mos/gfx/texture_2d.hpp>

namespace mos {
namespace gfx {

Texture_2D::Texture_2D(const int width,
                       const int height,
                       const Format &format,
                       const Filter &filter,
                       const Wrap &wrap,
                       const bool mipmaps)
    : Texture(width, height, format, filter, wrap, mipmaps) {}

Shared_texture_2D Texture_2D::load(const std::string &path, const bool color_data, const bool mipmaps, const Filter &filter, const Wrap &wrap) {
  if (path.empty() || path.back() == '/') {
    return Shared_texture_2D();
  }
  return std::make_shared<Texture_2D>(path, color_data, mipmaps, filter, wrap);
}

Texture_2D::Texture_2D(const std::string &path,
                       const bool color_data,
                       const bool mipmaps,
                       const Texture_2D::Filter &filter,
                       const Texture_2D::Wrap &wrap)
    : Texture({path}, color_data, filter, wrap, mipmaps) {}
}
}
