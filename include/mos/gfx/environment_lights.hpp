#pragma once

#include <array>
#include <mos/core/container.hpp>
#include <mos/gfx/environment_light.hpp>

namespace mos {
namespace gfx {

/** Environment light container. */
using EnvironmentLights = std::array<EnvironmentLight, 2>;

}
}