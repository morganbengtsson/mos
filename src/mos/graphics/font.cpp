#include <graphics/font.hpp>

namespace mos {

Font::Font(const Font::CharMap &characters, const Font::TexPtr &texture) :
    characters(characters),
    texture(texture) {
}

Font::~Font(){

}

}

