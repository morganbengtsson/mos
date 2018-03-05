#pragma once
#include <vector>
#include <string>
#include <mos/gfx/texture_2d.hpp>
#include <mos/gfx/texture.hpp>

namespace mos {
namespace gfx {
class TextureCube;
class Texture;
using SharedTextureCube = std::shared_ptr<TextureCube>;

/** Cube map texture, for environments. */
class TextureCube final : public Texture {
public:
  TextureCube(const int width,
              const int height,
              const bool mipmaps = true,
              const Format &format = Format::RGB);
};
}
}
