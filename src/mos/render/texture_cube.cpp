#include <mos/render/texture_cube.hpp>
#include <lodepng.h>

namespace mos {
std::atomic_uint Texture::current_id_;

TextureCube::TextureCube(const std::string &positive_x_path,
                         const std::string &negative_x_path,
                         const std::string &positive_y_path,
                         const std::string &negative_y_path,
                         const std::string &positive_z_path,
                         const std::string &negative_z_path,
                         const bool mipmaps,
                         const bool compress) : mipmaps(mipmaps),
                                                compress(compress),
                                                id_(current_id_++) {
  decode(positive_x, positive_x_path);
  decode(negative_x, negative_x_path);
  decode(positive_y, positive_y_path);
  decode(negative_y, negative_y_path);
  decode(positive_z, positive_z_path);
  decode(negative_z, negative_z_path);

}

void TextureCube::decode(Pixels &pixels, const std::string &path) {
  auto error = lodepng::decode(pixels, width_, height_, path);
  if (error) {
    std::string e = "Error decoding " + path + " : " + std::to_string(error) + ": " +
        std::string(lodepng_error_text(error));
    throw std::runtime_error(e);
  }

}
}