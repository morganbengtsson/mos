#include <mos/graphics/lightmaps.hpp>

Lightmaps::Lightmaps(const Lightmaps::TexPtr &lightmap0,
                     const Lightmaps::TexPtr &lightmap1, const float mix)
    : lightmap0(lightmap0), lightmap1(lightmap1), mix(mix) {}
