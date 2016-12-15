#ifndef MOS_TEXTURECUBE_HPP
#define MOS_TEXTURECUBE_HPP

#include <mos/render/texture.hpp>
#include <vector>
#include <string>

namespace mos {

class TextureCube {
public:
  using Pixels = std::vector<unsigned char>;

  TextureCube(const std::string &positive_x_path,
              const std::string &negative_x_path,
              const std::string &positive_y_path,
              const std::string &negative_y_path,
              const std::string &positive_z_path,
              const std::string &negative_z_path,
              const bool mipmaps = true,
              const bool compress = true);
  Pixels positive_x;
  Pixels negative_x;
  Pixels positive_y;
  Pixels negative_y;
  Pixels positive_z;
  Pixels negative_z;
  const bool mipmaps;
  const bool compress;
private:
  unsigned int width_;
  unsigned int height_;
  static std::atomic_uint current_id_;
  unsigned int id_;
  void decode(Pixels &pixels, const std::string &path);
};

}
#endif //MOS_TEXTURECUBE_HPP
