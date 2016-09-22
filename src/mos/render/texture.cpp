#include <iostream>
#include <lodepng.h>
#include <mos/render/texture.hpp>

namespace mos {

std::atomic_uint Texture::current_id_;

Texture::Texture(const unsigned int width, const unsigned int height,
                 const bool mipmaps, const bool compress,
                 const Texture::Wrap &wrap)
    : mipmaps(mipmaps), compress(compress), width_(width), height_(height),
      id_(current_id_++), wrap(wrap) {}

Texture::~Texture() {}

SharedTexture Texture::load(const std::string &path, const bool mipmaps,
                            const bool compress, const Texture::Wrap &wrap) {
  std::vector<unsigned char> pixels;
  unsigned int width, height;

  if (path.empty()) {
    return std::shared_ptr<Texture>(nullptr);
  }
  auto error = lodepng::decode(pixels, width, height, path);
  if (error) {
    std::string e = "Decoder error: " + std::to_string(error) + ": " +
                    std::string(lodepng_error_text(error));
    throw std::runtime_error(e);
  }

  return std::make_shared<Texture>(pixels.begin(), pixels.end(), width, height, mipmaps, compress,
                                   wrap);
}

Texture::Texture(const std::string &path, const bool mipmaps,
                 const bool compress, const Texture::Wrap &wrap)
    : mipmaps(mipmaps), compress(compress), wrap(wrap) {
  auto error = lodepng::decode(texels_, width_, height_, path);
  if (error) {
    std::string e = "Decoder error: " + std::to_string(error) + ": " +
                    std::string(lodepng_error_text(error));
    throw std::runtime_error(e);
  }
}

Texture::Texels::const_iterator Texture::begin() const {
  return texels_.begin();
}

Texture::Texels::const_iterator Texture::end() const { return texels_.end(); }

unsigned int Texture::id() const { return id_; }

unsigned int Texture::width() const { return width_; }

unsigned int Texture::height() const { return height_; }

unsigned int Texture::size() const { return texels_.size(); }

glm::vec4 Texture::sample(const unsigned int x, const unsigned int y) {
  unsigned int index = x * 4 * height() + y * 4;
  unsigned char r = texels_[index];
  unsigned char g = texels_[index + 1];
  unsigned char b = texels_[index + 2];
  unsigned char a = texels_[index + 3];
  return glm::vec4((float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f,
                   (float)a / 255.0f);
}

const unsigned char *Texture::data() const { return texels_.data(); }
}
