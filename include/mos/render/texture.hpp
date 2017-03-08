#ifndef MOS_TEXTURE_HPP
#define MOS_TEXTURE_HPP

#include <vector>
#include <atomic>
#include <initializer_list>
#include <lodepng.h>
namespace mos {
class Texture {
  friend class RenderSystem;
public:
  using Data = std::vector<unsigned char>;
  enum class Wrap { REPEAT, CLAMP_TO_EDGE, CLAMP_TO_BORDER };
  enum class Format {RGB, RGBA, SRGB, SRGBA, DEPTH};
  template<class T>
  Texture(T begin, T end,
          const int width,
          const int height,
          const Format &format = Format::SRGBA,
          const Wrap &wrap = Wrap::REPEAT,
          const bool mipmaps = true,
          const bool compress = false) : layers_(begin, end),
                                         width_(width), height_(height),
                                         format(format), wrap(wrap), mipmaps(mipmaps),
                                         compress(compress), id_(current_id_++) {};

  Texture(const std::initializer_list<Data> &layers,
          const int width,
          const int height,
          const Format &format = Format::SRGBA,
          const Wrap &wrap = Wrap::REPEAT,
          const bool mipmaps = true,
          const bool compress = false);

  Texture(const int width,
          const int height,
          const Format &format = Format::SRGBA,
          const Wrap &wrap = Wrap::REPEAT,
          const bool mipmaps = true,
          const bool compress = false);

  Texture(const std::initializer_list<std::string> &paths, const Format &format,
          const Wrap &wrap, const bool mipmaps, const bool compress);

  int id() const;
  int width() const;
  int height() const;
  int depth() const;
  int size(const int layer = 0);
  const unsigned char *data(const int layer = 0) const;
  bool mipmaps; // TODO: const
  const bool compress;
  Wrap wrap; // TODO: const
  Format format; // TODO: const
private:
  static std::atomic_uint current_id_;
  int id_;
  unsigned int width_;
  unsigned int height_;
  std::vector<Data> layers_;
};
}

#endif //MOS_TEXTURE_HPP
