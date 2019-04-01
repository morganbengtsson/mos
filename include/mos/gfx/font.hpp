#pragma once
#include <map>
#include <memory>
#include <mos/gfx/character.hpp>
#include <mos/gfx/texture_2d.hpp>

namespace mos {
namespace gfx {

/** Bitmap font. */
class Font final {
public:
  using Char_map = std::map<char, Character>;
  /** @param characters Chars supported.
   * @param texture Image with glyphs. */
  Font(Char_map characters,
       Shared_texture_2D texture,
       float height,
       float base);
  explicit Font(const std::string &path);

  ~Font() = default;

  /** Height. */
  float height() const;

  /** Base line. */
  float base() const;

  /** Texture with characters. */
  Shared_texture_2D texture;

  /** Characters supported. */
  Char_map characters;

private:
  float height_;
  float base_;
};
}
}

