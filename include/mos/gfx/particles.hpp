#pragma once
#include <vector>
#include <atomic>
#include <mos/gfx/texture_2d.hpp>
#include <chrono>
#include "particle.hpp"

namespace mos {
namespace gfx {

/** Particles for rendering. */
class Particles final {
public:
  friend class Renderer;
  using Parts = std::vector<Particle>;

  Particles();

  template<class T>
  Particles(T begin, T end) {
    invalidate();
    particles_.assign(begin, end);
  }

  virtual ~Particles();

  Parts::const_iterator begin() const { return particles_.begin(); }
  Parts::const_iterator end() const { return particles_.end(); }
  Parts::iterator begin() { return particles_.begin(); }
  Parts::iterator end() { return particles_.end(); }

  template<class T>
  void assign(T begin, T end) {
    particles_.assign(begin, end);
    invalidate();
  }

  void clear();

  void add(const Particle particle);

  Particle back();

  const Particle *data() const;

  unsigned int size() const;

  unsigned int id() const;

  void invalidate();

  /** Sort particles relative to a position. */
  void sort(const glm::vec3 &position);

  Particle &operator[](size_t index);

  const Particle &operator[](size_t index) const;

  /** Texture for all particles. */
  SharedTexture2D emission_map;
private:
  std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> modified_;
  Parts particles_;
  static std::atomic_uint current_id_;
  unsigned int id_;
};
}
}