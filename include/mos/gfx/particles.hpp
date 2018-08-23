#pragma once

#include <mos/core/tracked_container.hpp>
#include <mos/gfx/particle.hpp>

namespace mos {
namespace gfx {

/** Collection of particles, tracks modifications */
using Particles = TrackedContainer<Particle>;

}
}