#ifndef LIGHTMAPS_HPP
#define LIGHTMAPS_HPP

#include <memory>
#include <mos/graphics/texture2d.hpp>

class Lightmaps {
public:
  using TexPtr = std::shared_ptr<mos::Texture2D>;
  Lightmaps(const TexPtr &lightmap0 = TexPtr(), const TexPtr &lightmap1 = TexPtr(),
            const float mix = 0.0f);
  TexPtr lightmap0;
  TexPtr lightmap1;
  float mix;
};

#endif // LIGHTMAPS_HPP
