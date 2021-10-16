#pragma once
#include <map>
#include <memory>
#include <mos/gfx/character.hpp>
#include <mos/gfx/texture_2d.hpp>

namespace mos::gfx {

/** Bitmap font. */
class Font final {
public:
  using Char_map = std::map<unsigned int, Character>;
  using Char_pair = std::pair<unsigned int, Character>;
  /** @param characters Chars supported.
   * @param texture Image with glyphs. */
  Font(const Char_map &characters, const Shared_texture_2D &texture,
       float height, float base);

  static auto load(const std::string &path) -> Font;

  /** Height. */
  auto height() const -> float;

  /** Base line. */
  auto base() const -> float;

  /** Texture with characters. */
  Shared_texture_2D texture;

  /** Characters supported. */
  Char_map characters;

private:
  float height_;
  float base_;
};
} // namespace mos::gfx
