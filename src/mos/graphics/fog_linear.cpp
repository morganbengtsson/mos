#include <mos/graphics/fog_linear.hpp>

namespace mos {
FogLinear::FogLinear(const glm::vec3 &color, const float near, const float far)
    : color(color), near(near), far(far) {
}

FogLinear::~FogLinear() {}
}
