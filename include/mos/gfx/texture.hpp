#pragma once
#include <vector>
#include <atomic>
#include <initializer_list>
#include <memory>
#include <mos/core/tracked_container.hpp>

namespace mos {
namespace gfx {

class Texture;
using Shared_texture = std::shared_ptr<Texture>;

/** Texture base */
class Texture {
public:
  using Data = std::vector<unsigned char>;
  enum class Wrap { Repeat, Clamp };
  enum class Format {
    R,
    RG,
    RGB,
    RGBA,
    SRGB,
    SRGBA
  };
  template<class T>
  Texture(T begin, T end,
          const int width,
          const int height,
          const Format &format = Format::SRGBA,
          const Wrap &wrap = Wrap::Repeat,
          const bool mipmaps = true) : layers(begin, end),
                                       width_(width), height_(height),
                                       format(format), wrap(wrap), mipmaps(mipmaps), id_(current_id_++) {};

  Texture(const std::initializer_list<Data> &layers,
          int width,
          int height,
          const Format &format = Format::SRGBA,
          const Wrap &wrap = Wrap::Repeat,
          bool mipmaps = true);

  Texture(int width,
          int height,
          const Format &format = Format::SRGBA,
          const Wrap &wrap = Wrap::Repeat,
          bool mipmaps = true);

  Texture(const std::initializer_list<std::string> &paths,
          bool color_data,
          const Wrap &wrap,
          bool mipmaps);

  int id() const;
  int width() const;
  int height() const;
  int depth() const;

  bool mipmaps; // TODO: const
  Wrap wrap; // TODO: const
  Format format; // TODO: const
  Tracked_container<Data> layers;
private:
  static std::atomic_uint current_id_;
  int id_;
  int width_;
  int height_;

};
}
}
