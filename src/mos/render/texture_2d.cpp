#include <iostream>
#include <lodepng.h>
#include <mos/render/texture_2d.hpp>

namespace mos {

Texture2D::Texture2D(const unsigned int width, const unsigned int height,
                     const bool mipmaps, const bool compress,
                     const Texture2D::Wrap &wrap)
    : Texture(width, height, wrap, mipmaps, compress) {}

Texture2D::~Texture2D() {}

SharedTexture Texture2D::load(const std::string &path, const bool mipmaps,
                              const bool compress, const Texture2D::Wrap &wrap) {
  if (path.empty() || path.back() == '/') {
    return SharedTexture();
  }
  return std::make_shared<Texture2D>(path, mipmaps, compress, wrap);
}

Texture2D::Texture2D(const std::string &path, const bool mipmaps,
                     const bool compress, const Texture2D::Wrap &wrap)
    : Texture(0, 0, wrap, mipmaps, compress) {

  std::vector<unsigned char> pixels;
  auto error = lodepng::decode(pixels, width_, height_, path);
  if (error) {
    std::string e = "Decoder error: " + std::to_string(error) + ": " +
        std::string(lodepng_error_text(error));
    throw std::runtime_error(e);
  }
  layers_[0].assign(pixels.begin(), pixels.end());
}

Texture2D::Data::const_iterator Texture2D::begin() const {
  return layers_[0].begin();
}

Texture2D::Data::const_iterator Texture2D::end() const {
  return layers_[0].end();
}
}
