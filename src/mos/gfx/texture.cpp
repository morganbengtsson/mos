#include <mos/gfx/texture.hpp>
#include <stdexcept>
#include <stb_image.h>
#include <map>

namespace mos::gfx {
std::atomic_int Texture::current_id_;
Texture::Texture(const int width, const int height,
                 const Format &format, const Filter &filter,
                 const Wrap &wrap, const bool mipmaps)
    : mipmaps(mipmaps), filter(filter), wrap(wrap),
      format(format), layers{Data()}, id_(current_id_++), width_(width),
      height_(height) {}

Texture::Texture(const std::initializer_list<Texture::Data> &layers,
                 const int width,
                 const int height,
                 const Format &format,
                 const Filter &filter,
                 const Wrap &wrap,
                 const bool mipmaps) : Texture(layers.begin(),
                                               layers.end(),
                                               width,
                                               height,
                                               format,
                                               filter,
                                               wrap,
                                               mipmaps) {}

Texture::Texture(const std::initializer_list<std::string> &paths,
                 const bool color_data, const Filter &filter,
                 const Texture::Wrap &wrap, const bool mipmaps)
    : mipmaps(mipmaps), filter(filter), wrap(wrap), id_(current_id_++),
      width_(0), height_(0) {
  std::map<int, Format> bpp_map{{1, Format::R}, {2, Format::RG}, {3, color_data ? Format::SRGB : Format::RGB}, {4, color_data ? Format::SRGBA : Format::RGBA}};
  for (auto &path : paths) {
    int bpp;
    unsigned char *pixels = stbi_load(path.c_str(), &width_, &height_, &bpp, 0);
    layers.push_back(Data(pixels, pixels + (width_ * height_ * bpp)));
    format = bpp_map[bpp];
  }
}

auto Texture::id() const -> int {
  return id_;
}
auto Texture::width() const -> int {
  return width_;
}
auto Texture::height() const -> int {
  return height_;
}
auto Texture::depth() const -> Tracked_container<Data>::size_type {
  return layers.size();
}
}
