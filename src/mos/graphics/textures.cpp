#include <mos/graphics/textures.hpp>

Textures::Textures(const Textures::TexPtr &first,
                     const Textures::TexPtr &second)
    : first(first), second(second) {}
