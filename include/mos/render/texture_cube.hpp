#ifndef MOS_TEXTURECUBE_HPP
#define MOS_TEXTURECUBE_HPP

#include <mos/render/texture_2d.hpp>
#include <mos/render/texture.hpp>
#include <vector>
#include <string>

namespace mos {
class TextureCube;
class Texture;
using SharedTextureCube = std::shared_ptr<TextureCube>;
class TextureCube final : public Texture {
public:
  TextureCube(const int width, const int height, const bool mipmaps, const Format &format);

  TextureCube(const std::string &base_path,
              const bool mipmaps = true,
              const Wrap &wrap = Wrap::CLAMP_TO_EDGE);

  TextureCube(const std::string &positive_x_path,
              const std::string &negative_x_path,
              const std::string &positive_y_path,
              const std::string &negative_y_path,
              const std::string &positive_z_path,
              const std::string &negative_z_path,
              const bool mipmaps = true,
              const Wrap &wrap = Wrap::CLAMP_TO_EDGE);

  static SharedTextureCube load(const std::string &positive_x_path,
                                const std::string &negative_x_path,
                                const std::string &positive_y_path,
                                const std::string &negative_y_path,
                                const std::string &positive_z_path,
                                const std::string &negative_z_path,
                                const bool mipmaps = true,
                                const Wrap &wrap = Wrap::CLAMP_TO_EDGE);

  static SharedTextureCube load(const std::string &base_path,
                                const bool mipmaps = true,
                                const Wrap &wrap = Wrap::CLAMP_TO_EDGE);
private:

};

}
#endif //MOS_TEXTURECUBE_HPP
