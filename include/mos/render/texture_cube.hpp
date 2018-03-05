#pragma once

#include <mos/render/texture_2d.hpp>
#include <mos/render/texture.hpp>
#include <vector>
#include <string>

namespace mos {
namespace gfx {
class TextureCube;
class Texture;
using SharedTextureCube = std::shared_ptr<TextureCube>;
class TextureCube final : public Texture {
public:
  TextureCube(const int width,
              const int height,
              const bool mipmaps = true,
              const Format &format = Format::RGB);
};
}
}
