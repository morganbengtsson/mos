#ifndef FONT_HPP
#define FONT_HPP

#include "texture2d.hpp"
#include "character.hpp"
#include <map>
#include <memory>

namespace mos {

class Font
{
public:
    using CharMap = std::map<char, Character>;
    using TexPtr = std::shared_ptr<Texture2D>;
    Font(const CharMap & characters, const TexPtr & texture);
    ~Font();
    TexPtr texture;
    CharMap characters;
};

}

#endif // FONT_HPP
