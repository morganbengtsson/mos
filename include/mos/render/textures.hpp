#ifndef TEXTURE2DS_HPP
#define TEXTURE2DS_HPP

#include <memory>
#include <mos/render/texture.hpp>

class Textures {
public:
  using TexPtr = std::shared_ptr<mos::Texture>;
  Textures(const TexPtr &first = TexPtr(), const TexPtr &second = TexPtr());
  TexPtr first;
  TexPtr second;
};

#endif // TEXTURE2DS_HPP
