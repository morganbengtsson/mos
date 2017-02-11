#include <mos/render/texture_cube.hpp>
#include <lodepng.h>
#include <filesystem/path.h>

namespace mos {
std::atomic_uint TextureCube::current_id_;

TextureCube::TextureCube(const std::string &positive_x_path,
                         const std::string &negative_x_path,
                         const std::string &positive_y_path,
                         const std::string &negative_y_path,
                         const std::string &positive_z_path,
                         const std::string &negative_z_path,
                         const bool mipmaps,
                         const bool compress,
                         const Wrap &wrap) : mipmaps(mipmaps),format(Format::SRGBA),
                                                                  compress(compress), wrap(wrap),
                                                                  id_(current_id_++) {
  decode(layers_[0], positive_x_path);
  decode(layers_[1], negative_x_path);
  decode(layers_[2], positive_y_path);
  decode(layers_[3], negative_y_path);
  decode(layers_[4], positive_z_path);
  decode(layers_[5], negative_z_path);
}

void TextureCube::decode(Data &pixels, const std::string &path) {
  auto error = lodepng::decode(pixels, width_, height_, path);
  if (error) {
    std::string e = "Error decoding " + path + " : " + std::to_string(error) + ": " +
        std::string(lodepng_error_text(error));
    throw std::runtime_error(e);
  }
}
unsigned int TextureCube::width() const {
  return width_;
}
unsigned int TextureCube::height() const {
  return height_;
}
unsigned int TextureCube::id() const {
  return id_;
}
const unsigned char *TextureCube::data_positive_x() {
  return layers_[0].data();
}
const unsigned char *TextureCube::data_negative_x() {
  return layers_[1].data();
}
const unsigned char *TextureCube::data_positive_y() {
  return layers_[2].data();
}
const unsigned char *TextureCube::data_negative_y() {
  return layers_[3].data();
}
const unsigned char *TextureCube::data_positive_z() {
  return layers_[4].data();
}
const unsigned char *TextureCube::data_negative_z() {
  return layers_[5].data();
}
SharedTextureCube TextureCube::load(const std::string &positive_x_path,
                                    const std::string &negative_x_path,
                                    const std::string &positive_y_path,
                                    const std::string &negative_y_path,
                                    const std::string &positive_z_path,
                                    const std::string &negative_z_path,
                                    const bool mipmaps,
                                    const bool compress,
                                    const TextureCube::Wrap &wrap) {
  return std::make_shared<TextureCube>(positive_x_path,
                                negative_x_path,
                                positive_y_path,
                                negative_y_path,
                                positive_z_path,
                                negative_z_path,
                                       mipmaps,
                                       compress,
                                       wrap);
}

TextureCube::TextureCube(const std::string &base_path,
                         const bool mipmaps,
                         const bool compress,
                         const TextureCube::Wrap &wrap) : TextureCube(
    base_path.substr(0, base_path.find_last_of(".")) + "_positive_x.png",
    base_path.substr(0, base_path.find_last_of(".")) + "_negative_x.png",
    base_path.substr(0, base_path.find_last_of(".")) + "_positive_y.png",
    base_path.substr(0, base_path.find_last_of(".")) + "_negative_y.png",
    base_path.substr(0, base_path.find_last_of(".")) + "_positive_z.png",
    base_path.substr(0, base_path.find_last_of(".")) + "_negative_z.png", mipmaps, compress, wrap) {}

SharedTextureCube TextureCube::load(const std::string &base_path,
                                    const bool mipmaps,
                                    const bool compress,
                                    const TextureCube::Wrap &wrap) {
  if (base_path.empty() || base_path.back() == '/') {
    return nullptr;
  }
  return std::make_shared<TextureCube>(base_path, mipmaps, compress, wrap);
}
const unsigned char *TextureCube::data(const int i) {
  return layers_[i].data();
}
}