#pragma once
#include <vector>
#include <atomic>
#include <mos/gfx/texture_2d.hpp>
#include <chrono>
#include <mos/gfx/particle.hpp>
#include <mos/gfx/container.hpp>

namespace mos {
namespace gfx {

/** Particles for rendering. */
class Particles final {
public:
  friend class Renderer;

  Particles();

  template<class T>
  Particles(T begin, T end) {
    particles.assign(begin, end);
  }

  ~Particles();

  unsigned int id() const;

  /** Sort particles relative to a position. */
  void sort(const glm::vec3 &position);

  /** Texture for all particles. */
  SharedTexture2D emission_map;

  /** Particles. */
  Container<Particle> particles;
private:
  static std::atomic_uint current_id_;
  unsigned int id_;
};
}
}