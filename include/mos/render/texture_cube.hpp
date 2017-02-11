#ifndef MOS_TEXTURECUBE_HPP
#define MOS_TEXTURECUBE_HPP

#include <mos/render/texture.hpp>
#include <vector>
#include <string>

namespace mos {
class TextureCube;
using SharedTextureCube = std::shared_ptr<TextureCube>;
class TextureCube {
public:
  using Data = std::vector<unsigned char>;
  enum class Wrap { REPEAT, CLAMP_TO_EDGE, CLAMP_TO_BORDER };

  TextureCube(int width, int height, const bool mipmaps)
      : width_(width), height_(height), mipmaps(mipmaps), compress(false), id_(current_id_++) {
  }

  TextureCube(const std::string &base_path,
              const bool mipmaps = true,
              const bool compress = false,
              const Wrap &wrap = Wrap::REPEAT);

  TextureCube(const std::string &positive_x_path,
              const std::string &negative_x_path,
              const std::string &positive_y_path,
              const std::string &negative_y_path,
              const std::string &positive_z_path,
              const std::string &negative_z_path,
              const bool mipmaps = true,
              const bool compress = false,
              const Wrap &wrap = Wrap::REPEAT);
  const unsigned char *data_positive_x();
  const unsigned char *data_negative_x();
  const unsigned char *data_positive_y();
  const unsigned char *data_negative_y();
  const unsigned char *data_positive_z();
  const unsigned char *data_negative_z();
  const unsigned char *data(const int i);

  const bool mipmaps;
  const bool compress;
  Wrap wrap;
  unsigned int width() const;
  unsigned int height() const;
  unsigned int id() const;
  static SharedTextureCube load(const std::string &positive_x_path,
                                const std::string &negative_x_path,
                                const std::string &positive_y_path,
                                const std::string &negative_y_path,
                                const std::string &positive_z_path,
                                const std::string &negative_z_path,
                                const bool mipmaps = true,
                                const bool compress = false,
                                const Wrap &wrap = Wrap::REPEAT);

  static SharedTextureCube load(const std::string &base_path,
                                const bool mipmaps = true,
                                const bool compress = false,
                                const Wrap &wrap = Wrap::REPEAT);
private:
  std::array<Data, 6> layers_;
  unsigned int width_;
  unsigned int height_;
  static std::atomic_uint current_id_;
  const unsigned int id_;
  void decode(Data &pixels, const std::string &path);
};

}
#endif //MOS_TEXTURECUBE_HPP
