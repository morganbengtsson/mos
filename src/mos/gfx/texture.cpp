#include <mos/gfx/texture.hpp>
#include <stdexcept>
#include <stb_image.h>
#include <map>

namespace mos {
namespace gfx {
std::atomic_int Texture::current_id_;
Texture::Texture(const int width, const int height,
                 const Texture::Format &format, const Texture::Filter &filter,
                 const Texture::Wrap &wrap, const bool mipmaps)
    : mipmaps(mipmaps), filter(filter), wrap(wrap),
      format(format), layers{Data()}, id_(current_id_++), width_(width),
      height_(height) {}

Texture::Texture(const std::initializer_list<Texture::Data> &layers,
                 const int width,
                 const int height,
                 const Texture::Format &format,
                 const Texture::Filter &filter,
                 const Texture::Wrap &wrap,
                 const bool mipmaps) : Texture(layers.begin(),
                                               layers.end(),
                                               width,
                                               height,
                                               format,
                                               filter,
                                               wrap,
                                               mipmaps) {}

Texture::Texture(const std::initializer_list<std::string> &paths,
                 const bool color_data, const Texture::Filter &filter,
                 const Texture::Wrap &wrap, const bool mipmaps)
    : mipmaps(mipmaps), filter(filter), wrap(wrap), id_(current_id_++),
      width_(0), height_(0) {
  for (auto &path : paths) {
    int bpp;
    unsigned char *pixels = stbi_load(path.c_str(), &width_, &height_, &bpp, 0);
    layers.push_back(Data(pixels, pixels + (width_ * height_ * bpp)));
    std::map<int, Format> bpp_map{{1, Format::R}, {2, Format::RG}, {3, color_data ? Format::SRGB : Format::RGB}, {4, color_data ? Format::SRGBA : Format::RGBA}};
    format = bpp_map[bpp];
  }
}

int Texture::id() const {
  return id_;
}
int Texture::width() const {
  return width_;
}
int Texture::height() const {
  return height_;
}
Tracked_container<Texture::Data>::size_type Texture::depth() const {
  return layers.size();
}

}
}
