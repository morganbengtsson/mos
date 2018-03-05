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
  /**
   * @brief Particles constructor.
   */
  Particles();

  template<class T>
  /**
   * @brief Particles constructor
   * @param begin First particle iterator.
   * @param end Last particle iterator.
   */
  Particles(T begin, T end) {
    invalidate();
    particles_.assign(begin, end);
  }

  /**
   * @brief ~Particles destructor.
   */
  virtual ~Particles();

  /**
   * @brief Particles container.
   */
  using Parts = std::vector<Particle>;

  /**
   * @brief begin
   * @return Iterator to first particle.
   */
  Parts::const_iterator begin() const { return particles_.begin(); }

  /**
   * @brief end
   * @return Iterator to last particle.
   */
  Parts::const_iterator end() const { return particles_.end(); }

  /**
   * @brief begin
   * @return Iterator to first particle.
   */
  Parts::iterator begin() { return particles_.begin(); }

  /**
   * @brief end
   * @return Iterator last particle.
   */
  Parts::iterator end() { return particles_.end(); }

  template<class T>
  /**
   * @brief Assign new particles.
   * @param begin Iterator to first particle.
   * @param end Iterator to last particle.
   */
  void assign(T begin, T end) {
    particles_.assign(begin, end);
    invalidate();
  }

  /**
   * @brief Clear the container.
   */
  void clear();

  /**
   * @brief Add a particle.
   * @param particle A particle.
   */
  void add(const Particle particle);

  /**
   * @brief back
   * @return Last particle.
   */
  Particle back();

  /**
   * @brief data
   * @return Raw data.
   */
  const Particle *data() const;

  /**
   * @brief Number of particles.
   * @return Number of particles.
   */
  unsigned int size() const;

  /**
   * @brief
   * @return A unique identifier.
   */
  unsigned int id() const;

  /**
   * @brief invalidate
   */
  void invalidate();

  /**
   * @brief sort particles relative to a position
   * @param position
   */
  void sort(const glm::vec3 &position);

  Particle &operator[](size_t index);

  const Particle &operator[](size_t index) const;

  //TODO: Texture array for animation
  /**
   * @brief texture for quad
   */
  SharedTexture2D emission_map;

private:
  std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> modified_;
  Parts particles_;
  static std::atomic_uint current_id_;
  unsigned int id_;
};
}
}