#pragma once
#include <vector>
#include <atomic>
#include <chrono>
#include <mos/core/tracked_container.hpp>
#include <mos/core/container.hpp>
#include <mos/gfx/texture_2d.hpp>
#include <mos/gfx/particles.hpp>
#include <mos/gfx/shape.hpp>

namespace mos {
namespace gfx {

/** Collection of particles for rendering, uses same texture. */
class Particle_cloud final : public Shape {
public:
  Particle_cloud();

  Particle_cloud(const Shared_texture_2D& emission_map, const Particles& particles);

  ~Particle_cloud() = default;

  /** Sort particles relative to a position. */
  void sort(const glm::vec3 &position);

  /** Texture for all particles. */
  Shared_texture_2D emission_map;

  /** Particles. */
  Particles particles;
};
}
}