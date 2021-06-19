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

auto Texture_2D::load(const std::string &path, bool color_data, bool generate_mipmaps, const Texture::Filter &filter, const Texture::Wrap &wrap) -> Texture_2D
{

  std::map<int, gli::format> gli_map{{1, gli::FORMAT_R8_UNORM_PACK8},
                                     {2, gli::FORMAT_RG8_UNORM_PACK8},
                                     {3, color_data ? gli::FORMAT_RGB8_SRGB_PACK8 : gli::FORMAT_RGB8_UNORM_PACK8},
                                     {4, color_data ? gli::FORMAT_RGBA8_SRGB_PACK8 : gli::FORMAT_RGBA8_UNORM_PACK8}};
  int bpp = 0;
  int width = 0;
  int height = 0;
  unsigned char *pixels = stbi_load(path.c_str(), &width, &height, &bpp, 0);
  if (pixels == nullptr) {
    throw std::runtime_error("Could not read texture: " + path);
  }

  return Texture_2D(pixels, pixels + width * height * bpp,
                    width, height,
                    gli_map[bpp],
                    filter,
                    wrap,
                    generate_mipmaps);
}

auto Texture_2D::width() const -> int {
  return texture_.extent().x;
}

auto Texture_2D::height() const -> int {
  return texture_.extent().y;
}

auto Texture_2D::data() const -> const void* {
  return texture_.data();
}

auto Texture_2D::format() const -> gli::format {
  return texture_.format();
}

auto Texture_2D::swizzles() const -> gli::swizzles {
  return texture_.swizzles();
}
}
