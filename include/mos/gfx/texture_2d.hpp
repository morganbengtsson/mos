#pragma once

#include <atomic>
#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <mos/gfx/texture.hpp>

namespace mos::gfx {

class Texture_2D;
using Shared_texture_2D = std::shared_ptr<Texture_2D>;

/** Texture in two dimension. Contains chars as data. */
class Texture_2D final : public Texture {
public:
  template <class T>
  Texture_2D(T begin, T end, int width, int height,
             const Format &format = Format::SRGBA,
             const Filter &filter = Filter::Linear,
             const Wrap &wrap = Wrap::Repeat, const bool mipmaps = true)
      : Texture({Data(begin, end)}, width, height, format, filter, wrap,
                mipmaps) {}

  Texture_2D(int width, int height, const Format &format = Format::SRGBA,
             const Filter &filter = Filter::Linear,
             const Wrap &wrap = Wrap::Repeat, bool mipmaps = true);

  /** Load from file */
   static Shared_texture_2D load(const std::string &path,
                                 bool color_data = true, bool mipmaps = true,
                                 const Filter &filter = Filter::Linear,
                                 const Wrap &wrap = Wrap::Repeat);

   /** Create from file. */
   Texture_2D(const std::string &path, bool color_data = true,
              bool mipmaps = true, const Filter &filter = Filter::Linear,
              const Wrap &wrap = Wrap::Repeat);
};
}
