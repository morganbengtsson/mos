#pragma once
#include <vector>
#include <atomic>
#include <initializer_list>
#include <memory>
#include <mos/core/tracked_container.hpp>

namespace mos {
namespace gfx {

/** Texture base */
class Texture;
using SharedTexture = std::shared_ptr<Texture>;
class Texture {
  friend class Renderer;
public:
  using Data = std::vector<unsigned char>;
  enum class Wrap { REPEAT, CLAMP_TO_EDGE, CLAMP_TO_BORDER };
  enum class Format {
    R,
    RG,
    RGB,
    RGBA,
    SRGB,
    SRGBA,
    DEPTH,
    COMPRESSED_RGB,
    COMPRESSED_RGBA,
    COMPRESSED_SRGB,
    COMPRESSED_SRGBA,
    R16F,
    RG16F,
    RGB16F,
    RGBA16F,
    R32F,
    RG32F,
    RGB32F,
    RGBA32F
  };
  template<class T>
  Texture(T begin, T end,
          const int width,
          const int height,
          const Format &format = Format::SRGBA,
          const Wrap &wrap = Wrap::REPEAT,
          const bool mipmaps = true) : layers(begin, end),
                                       width_(width), height_(height),
                                       format(format), wrap(wrap), mipmaps(mipmaps), id_(current_id_++) {};

  Texture(const std::initializer_list<Data> &layers,
          const int width,
          const int height,
          const Format &format = Format::SRGBA,
          const Wrap &wrap = Wrap::REPEAT,
          const bool mipmaps = true);

  Texture(const int width,
          const int height,
          const Format &format = Format::SRGBA,
          const Wrap &wrap = Wrap::REPEAT,
          const bool mipmaps = true);

  Texture(const std::initializer_list<std::string> &paths,
          const bool color_data,
          const Wrap &wrap,
          const bool mipmaps);

  int id() const;
  int width() const;
  int height() const;
  int depth() const;

  bool mipmaps; // TODO: const
  Wrap wrap; // TODO: const
  Format format; // TODO: const
  TrackedContainer<Data> layers;
private:
  static std::atomic_uint current_id_;
  int id_;
  int width_;
  int height_;

};
}
}
