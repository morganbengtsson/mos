#include <mos/graphics/lightmaps.hpp>

Lightmaps::Lightmaps(const Lightmaps::TexPtr &lightmap0,
                     const Lightmaps::TexPtr &lightmap1, const float mix)
    : first(lightmap0), second(lightmap1), mix(mix) {}
