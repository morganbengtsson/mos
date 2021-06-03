#include <iostream>
#include <mos/gfx/texture_2d.hpp>
#include <map>
#include <stb_image.h>

namespace mos::gfx {

Texture_2D::Texture_2D(const int width,
                       const int height,
                       const gli::format &format,
                       const Filter &filter,
                       const Wrap &wrap,
                       const bool generate_mipmaps)
    :  Texture(filter, wrap, generate_mipmaps), texture_(format, gli::extent2d(width, height)) {}

auto Texture_2D::load(const std::string &path, const bool color_data, const bool generate_mipmaps, const Filter &filter, const Wrap &wrap) -> Shared_texture_2D {
  if (path.empty() || path.back() == '/') {
    return Shared_texture_2D();
  }
  return std::make_shared<Texture_2D>(path, color_data, generate_mipmaps, filter, wrap);
}

Texture_2D::Texture_2D(const std::string &path,
                       const bool color_data,
                       const bool generate_mipmaps,
                       const Texture_2D::Filter &filter,
                       const Texture_2D::Wrap &wrap)
    : Texture(filter, wrap, generate_mipmaps) {

  std::map<int, gli::format> gli_map{{1, gli::FORMAT_R8_UNORM_PACK8},
                                     {2, gli::FORMAT_RG8_UNORM_PACK8},
                                     {3, color_data ? gli::FORMAT_RGB8_SRGB_PACK8 : gli::FORMAT_RGB8_UNORM_PACK8},
                                     {4, color_data ? gli::FORMAT_RGBA8_SRGB_PACK8 : gli::FORMAT_RGBA8_UNORM_PACK8}};
  int bpp;
  int width, height;
  unsigned char *pixels = stbi_load(path.c_str(), &width, &height, &bpp, 0);
  if (pixels == nullptr) {
    throw std::runtime_error("Could not read texture: " + path);
  }
  texture_ = gli::texture2d(gli_map[bpp], gli::extent2d(width, height));
  std::memcpy(texture_.data(), pixels, width * height * bpp);
}

auto Texture_2D::width() const -> int {
  return texture_.extent().x;
}

auto Texture_2D::height() const -> int {
  return texture_.extent().y;
}

const void *Texture_2D::data() const {
  return texture_.data();
}

gli::format Texture_2D::format() const {
  return texture_.format();
}

gli::swizzles Texture_2D::swizzles() const {
  return texture_.swizzles();
}
}
