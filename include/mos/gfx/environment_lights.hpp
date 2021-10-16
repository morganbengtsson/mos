#pragma once

#include <array>
#include <mos/core/container.hpp>
#include <mos/gfx/environment_light.hpp>

namespace mos::gfx {

/** Environment light container. */
using Environment_lights = std::array<Environment_light, 2>;

} // namespace mos::gfx
