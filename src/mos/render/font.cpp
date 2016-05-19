#include <mos/render/font.hpp>

namespace mos {

Font::Font(const Font::CharMap &characters, const Font::TexPtr &texture,
           const float height, const float ascender, const float descender)
    : characters(characters), texture(texture), height_(height),
      ascender_(ascender), descender_(descender) {}

Font::~Font() {}

float Font::height() const { return height_; }

float Font::ascender() const { return ascender_; }

float Font::descender() const { return descender_; }
}
