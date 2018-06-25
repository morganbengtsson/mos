#pragma once
#include <vector>
#include <atomic>
#include <mos/gfx/texture_2d.hpp>
#include <chrono>
#include <mos/gfx/particle.hpp>
#include <mos/core/tracked_container.hpp>

namespace mos {
namespace gfx {

/** Particles for rendering. */
class ParticleCloud final {
public:
  ParticleCloud();

  template<class T>
  ParticleCloud(T begin, T end) {
    particles.assign(begin, end);
  }

  ~ParticleCloud();

  /** Unique id. */
  unsigned int id() const;

  /** Sort particles relative to a position. */
  void sort(const glm::vec3 &position);

  /** Texture for all particles. */
  SharedTexture2D emission_map;

  /** Particles. */
  TrackedContainer<Particle> particles;
private:
  static std::atomic_uint current_id_;
  unsigned int id_;
};
}
}