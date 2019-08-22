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
  enum class Filter { Linear, Closest };
  enum class Format { R, RG, RGB, RGBA, SRGB, SRGBA };

  template <class T>
  Texture(T begin, T end, const int width, const int height,
          const Format &format = Format::SRGBA, const Filter &filter = Filter::Linear, const Wrap &wrap = Wrap::Repeat,
          const bool mipmaps = true)
      : mipmaps(mipmaps), wrap(wrap), format(format), layers(begin, end),
        id_(current_id_++), width_(width), height_(height) {}

  Texture(const std::initializer_list<Data> &layers,
          int width,
          int height,
          const Format &format = Format::SRGBA,
          const Filter &filter = Filter::Linear,
          const Wrap &wrap = Wrap::Repeat,
          bool mipmaps = true);

  Texture(int width,
          int height,
          const Format &format = Format::SRGBA,
          const Filter &filter = Filter::Linear,
          const Wrap &wrap = Wrap::Repeat,
          bool mipmaps = true);

  Texture(const std::initializer_list<std::string> &paths,
          bool color_data,
          const Filter &filter,
          const Wrap &wrap,
          bool mipmaps);

  int id() const;
  int width() const;
  int height() const;
  Tracked_container<Data>::size_type depth() const;

  bool mipmaps; // TODO: const
  Filter filter;
  Wrap wrap; // TODO: const
  Format format; // TODO: const
  Tracked_container<Data> layers;
private:
  static std::atomic_int current_id_;
  int id_;
  int width_;
  int height_;

};
}
}
