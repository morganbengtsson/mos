#include <graphics/font.hpp>

namespace mos {

Font::Font(const Font::CharMap & characters,
           const Font::TexPtr & texture,
           const float size) :
    characters(characters),
    texture(texture),
    size_(size){
}

Font::~Font(){

}

float Font::size() const {
    return size_;
}

}

