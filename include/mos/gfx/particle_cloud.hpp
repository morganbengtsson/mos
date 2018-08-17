#pragma once
#include <vector>
#include <atomic>
#include <chrono>
#include <mos/core/tracked_container.hpp>
#include <mos/core/container.hpp>
#include <mos/gfx/texture_2d.hpp>
#include <mos/gfx/particles.hpp>

namespace mos {
namespace gfx {

/** Particles for rendering. */
class ParticleCloud final {
public:
  ParticleCloud();

  ParticleCloud(const SharedTexture2D& emission_map, const Particles & particles);

  ~ParticleCloud();

  /** Unique id. */
  unsigned int id() const;

  /** Sort particles relative to a position. */
  void sort(const glm::vec3 &position);

  /** Texture for all particles. */
  SharedTexture2D emission_map;

  /** Particles. */
  Particles particles;
private:
  static std::atomic_uint current_id_;
  unsigned int id_;
};
}
}