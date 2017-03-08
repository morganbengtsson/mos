#include <mos/render/texture.hpp>
#include <stdexcept>

namespace mos {
std::atomic_uint Texture::current_id_;
Texture::Texture(const int width,
                 const int height,
                 const Texture::Format &format,
                 const Texture::Wrap &wrap,
                 const bool mipmaps,
                 const bool compress) : id_(current_id_++),
                                        width_(width),
                                        height_(height),
                                        format(format),
                                        wrap(wrap),
                                        mipmaps(mipmaps),
                                        compress(compress),
                                        layers_{Data()} {}

Texture::Texture(const std::initializer_list<Texture::Data> &layers,
                 const int width,
                 const int height,
                 const Texture::Format &format,
                 const Texture::Wrap &wrap,
                 const bool mipmaps,
                 const bool compress) : Texture(layers.begin(),
                                                layers.end(),
                                                width,
                                                height,
                                                format,
                                                wrap,
                                                mipmaps,
                                                compress) {}

Texture::Texture(const std::initializer_list<std::string> &paths,
                 const Texture::Format &format,
                 const Texture::Wrap &wrap,
                 const bool mipmaps,
                 const bool compress) : wrap(wrap), format(format), mipmaps(mipmaps), compress(compress) {
  for (auto &path : paths) {
    std::vector<unsigned char> pixels;
    auto error = lodepng::decode(pixels, width_, height_, path);
    if (error) {
      std::string e = "Decoder error: " + std::to_string(error) + ": " +
          std::string(lodepng_error_text(error));
      throw std::runtime_error(e);
    }
    layers_.push_back(Data(pixels.begin(), pixels.end()));
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
int Texture::depth() const {
  return layers_.size();
}
int Texture::size(const int layer) {
  return layers_[layer].size();
}
const unsigned char *Texture::data(const int layer) const {
  return layers_[layer].data();
}
}