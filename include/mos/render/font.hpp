#ifndef MOS_FONT_HPP
#define MOS_FONT_HPP

#include <map>
#include <memory>
#include <mos/render/texture.hpp>
#include <mos/render/character.hpp>
#include <mos/render/render_types.hpp>

namespace mos {

class Font {
public:
  using CharMap = std::map<char, Character>;
  Font(const CharMap &characters, const SharedTexture &texture, const float height,
       const float ascender, const float descender);
  ~Font();
  SharedTexture texture;
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

#endif // MOS_FONT_HPP
