#include <mos/render/font.hpp>

namespace mos {

Font::Font(const Font::CharMap &characters, const SharedTexture &texture,
           const float height, const float ascender, const float descender)
    : characters(characters), texture(texture), height_(height),
      ascender_(ascender), descender_(descender) {
  texture->wrap = Texture::Wrap::CLAMP;
}

Font::~Font() {}

float Font::height() const { return height_; }

float Font::ascender() const { return ascender_; }

float Font::descender() const { return descender_; }
}