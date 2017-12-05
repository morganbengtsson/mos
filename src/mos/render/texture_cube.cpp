#include <mos/render/texture_cube.hpp>
#include <filesystem/path.h>

namespace mos {

TextureCube::TextureCube(const std::string &positive_x_path,
                         const std::string &negative_x_path,
                         const std::string &positive_y_path,
                         const std::string &negative_y_path,
                         const std::string &positive_z_path,
                         const std::string &negative_z_path,
                         const bool mipmaps,
                         const Wrap &wrap) : Texture({positive_x_path, negative_x_path,
                                                      positive_y_path, negative_y_path,
                                                      positive_z_path, negative_z_path},
                                                     Format::SRGBA,
                                                     wrap,
                                                     mipmaps) {}

SharedTextureCube TextureCube::load(const std::string &positive_x_path,
                                    const std::string &negative_x_path,
                                    const std::string &positive_y_path,
                                    const std::string &negative_y_path,
                                    const std::string &positive_z_path,
                                    const std::string &negative_z_path,
                                    const bool mipmaps,
                                    const TextureCube::Wrap &wrap) {
  return std::make_shared<TextureCube>(positive_x_path,
                                       negative_x_path,
                                       positive_y_path,
                                       negative_y_path,
                                       positive_z_path,
                                       negative_z_path,
                                       mipmaps,
                                       wrap);
}

TextureCube::TextureCube(const std::string &base_path,
                         const bool mipmaps,
                         const TextureCube::Wrap &wrap) : TextureCube(
    base_path.substr(0, base_path.find_last_of(".")) + "_positive_x.png",
    base_path.substr(0, base_path.find_last_of(".")) + "_negative_x.png",
    base_path.substr(0, base_path.find_last_of(".")) + "_positive_y.png",
    base_path.substr(0, base_path.find_last_of(".")) + "_negative_y.png",
    base_path.substr(0, base_path.find_last_of(".")) + "_positive_z.png",
    base_path.substr(0, base_path.find_last_of(".")) + "_negative_z.png", mipmaps, wrap) {}

SharedTextureCube TextureCube::load(const std::string &base_path,
                                    const bool mipmaps,
                                    const TextureCube::Wrap &wrap) {
  if (base_path.empty() || base_path.back() == '/') {
    return nullptr;
  }
  return std::make_shared<TextureCube>(base_path, mipmaps, wrap);
}
TextureCube::TextureCube(const int width, const int height, const bool mipmaps, const Texture::Format &format)
    : Texture({Data(), Data(), Data(), Data(), Data(), Data()}, width, height, format, Wrap::CLAMP_TO_BORDER, mipmaps) {
}
}
