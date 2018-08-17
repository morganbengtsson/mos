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

/** Collection of same particles for rendering. */
class ParticleCloud final : public Shape {
public:
  ParticleCloud();

  ParticleCloud(const SharedTexture2D& emission_map, const Particles& particles);

  ~ParticleCloud();

  /** Sort particles relative to a position. */
  void sort(const glm::vec3 &position);

  /** Texture for all particles. */
  SharedTexture2D emission_map;

  /** Particles. */
  Particles particles;
};
}
}