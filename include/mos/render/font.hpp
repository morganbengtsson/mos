#pragma once
#include <map>
#include <memory>
#include <mos/render/character.hpp>
#include <mos/render/texture_2d.hpp>

namespace mos {
namespace gfx {
/**
 * @brief Represents a bitmap font.
 */
class Font {
public:
  using CharMap = std::map<char, Character>;

  /**
   * @param characters is the chars supported.
   * @param texture is the image with glyphs.
   * @param height
   * @param ascender
   * @param descender
   */
  Font(const CharMap &characters,
       const SharedTexture2D &texture,
       const float height,
       const float ascender,
       const float descender);
  Font(const std::string &path);
  ~Font();
  SharedTexture2D texture;
  CharMap characters;
  float height() const;
  float ascender() const;
  float descender() const;

private:
  float height_;
  float ascender_;
  float descender_;
};
}
}

