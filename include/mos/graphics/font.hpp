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
    Font(const CharMap & characters, const TexPtr & texture, const float size);
    ~Font();
    TexPtr texture;
    CharMap characters;
    float size() const;
private:
    float size_;
};

}

#endif // FONT_HPP
