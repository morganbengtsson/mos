#ifndef FONT_HPP
#define FONT_HPP

#include <map>
#include <memory>
#include <mos/render/texture.hpp>
#include <mos/render/character.hpp>

namespace mos {

class Font {
public:
  using CharMap = std::map<char, Character>;
  using TexPtr = std::shared_ptr<Texture>;
  Font(const std::string &path);
  Font(const CharMap &characters, const TexPtr &texture, const float height,
       const float ascender, const float descender);
  ~Font();
  TexPtr texture;
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

#endif // FONT_HPP
