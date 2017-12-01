#include <iostream>
#include <lodepng.h>
#include <mos/render/texture_2d.hpp>

namespace mos {

Texture2D::Texture2D(const unsigned int width, const unsigned int height,
                     const Format &format, const Wrap &wrap,
                     const bool mipmaps)
    : Texture(width, height, format, wrap, mipmaps) {}

Texture2D::~Texture2D() {}

SharedTexture2D Texture2D::load(const std::string &path, const bool mipmaps, const Texture2D::Wrap &wrap) {
  if (path.empty() || path.back() == '/') {
    return SharedTexture2D();
  }
  return std::make_shared<Texture2D>(path, mipmaps, wrap);
}

Texture2D::Texture2D(const std::string &path, const bool mipmaps, const Texture2D::Wrap &wrap)
    : Texture({path}, Format::SRGBA, wrap, mipmaps) {}
}
