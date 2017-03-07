#include <iostream>
#include <lodepng.h>
#include <mos/render/texture_2d.hpp>

namespace mos {

std::atomic_uint Texture2D::current_id_;

Texture2D::Texture2D(const unsigned int width, const unsigned int height,
                 const bool mipmaps, const bool compress,
                 const Texture2D::Wrap &wrap)
    : mipmaps(mipmaps), compress(compress), width_(width), height_(height),
      id_(current_id_++), wrap(wrap) {}

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
    : mipmaps(mipmaps), compress(compress), wrap(wrap), id_(current_id_++) {

  std::vector<unsigned char> pixels;
  auto error = lodepng::decode(pixels, width_, height_, path);
  if (error) {
    std::string e = "Decoder error: " + std::to_string(error) + ": " +
                    std::string(lodepng_error_text(error));
    throw std::runtime_error(e);
  }
  texels_.assign(pixels.begin(), pixels.end());
}

Texture2D::Texels::const_iterator Texture2D::begin() const {
  return texels_.begin();
}

Texture2D::Texels::const_iterator Texture2D::end() const { return texels_.end(); }

unsigned int Texture2D::id() const { return id_; }

unsigned int Texture2D::width() const { return width_; }

unsigned int Texture2D::height() const { return height_; }

unsigned int Texture2D::size() const { return texels_.size(); }

glm::vec4 Texture2D::sample(const unsigned int x, const unsigned int y) {
  unsigned int index = x * 4 * height() + y * 4;
  unsigned char r = texels_[index];
  unsigned char g = texels_[index + 1];
  unsigned char b = texels_[index + 2];
  unsigned char a = texels_[index + 3];
  return glm::vec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f,
                   (float)a / 255.0f);
}

const unsigned char *Texture2D::data() const { return texels_.data(); }
}
