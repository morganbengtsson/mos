#ifndef MOS_LIGHTMAPS_HPP
#define MOS_LIGHTMAPS_HPP

#include <memory>
#include <mos/graphics/texture.hpp>
namespace mos {
class Lightmaps {
public:
  using TexPtr = std::shared_ptr<mos::Texture>;
  Lightmaps(const TexPtr &first = TexPtr(), const TexPtr &second = TexPtr(),
            const float mix = 0.0f);
  TexPtr first;
  TexPtr second;
  float mix;
};
}

#endif // MOS_LIGHTMAPS_HPP
